#include "Config.h"

#include <stdlib.h>
#include <stdio.h>

#include "stdio_fl.h"
#include "string_fl.h"
#include "memory_fl.h"
#include "AlvaThread.h"

#define LOG_TAG "DataQueue"
#include "Log.h"
#include "Dump.h"
#include "ErrorTools.h"

#include "DataQueueInterface.h"
#include "DataQueue.h"

#undef FILE_NUM
#define FILE_NUM 0xD800

#define FrameListNum 2                              //frameList个数
#define AlgorithmNum 10                             //算法个数

typedef struct _FrameNode_       FrameNode;
typedef struct _FrameListResult_ FrameListResult;
typedef struct _FrameResult_     FrameResult;
typedef struct _FrameListInfo_   FrameListInfo;
typedef struct __AlgorithmInfo__ AlgorithmInfo;

//全局结果结构体
struct _FrameResult_ {
    void* frameResult;                              //结果指针
    int   resultSize;                               //结果大小
};

//结果结构体
struct _FrameListResult_ {
    void* frameResult;                              //结果指针
    int   resultSize;                               //结果大小
    int   isSync;                                   //是否是同步
    atomic_int_Alva isProcess;                      //这个算法是否处理过
};

//算法依赖信息
struct __AlgorithmInfo__ {
    char  name[256];                                //算法的名字
    char  dependName[256];                          //算法依赖的算法的名字
    int   dependIndex;                              //算法依赖的算法的索引
    void* dependResultPtr;                          //算法依赖的算法的结果指针
};

//链表节点
struct _FrameNode_ {
    FrameListResult gframeListResult[AlgorithmNum];
    unsigned char* frameData;                       //传入的图像数据
    float matrix[16];                               //纹理的变化矩阵
    int ImageDataSize;                              //传入的图像数据大小
    atomic_int_Alva processing;                     //是不是正在处理
    atomic_int_Alva isCopy;                         //是否copy内存值
    atomic_int_Alva frameListStatusNum;             //节点被几个同步算法处理过
    atomic_int_Alva isDeal;                         //是否处理完
    struct _FrameNode_* next;                       //当前节点的下一个
};

//链表信息
struct _FrameListInfo_ {
    int frameListSize;                              //环形链表的总长度
    int regIsFlag[AlgorithmNum];                    //算法注册索引
    atomic_int_Alva frameFlag;                      //锁标志位
    atomic_int_Alva regAlgorithmNum;                //注册了几个算法
    atomic_int_Alva regSyncAlgorithmNum;            //同步算法注册了几个
    FrameNode* headNode;                            //头指针
    FrameNode* tailNode;                            //尾指针
    FrameNode* curFrame;                            //当前指针
    FrameNode* preFrame;                            //前一次处理过的帧
    AlgorithmInfo algorithmInfo[AlgorithmNum];
};

typedef enum _FrameStatus_ {
    FS_Ready  = 0,
    FS_Busy   = 1,
}FrameStatus;

typedef enum _FrameInitStatus_ {
    FIS_Free   = 0,
    FIS_Inited = 1,
}FrameInitStatus;

typedef enum _ThreadMessage_ {
    TM_NONE = 0,
    TM_STOP = 1,
}ThreadMessage;

static FrameListInfo gFrameListInfo;
static FrameResult   gFrameResult[AlgorithmNum];

static atomic_int_Alva mFrameStatus   = FIS_Free;   //链表状态标志
static atomic_int_Alva mThreadMessage = TM_NONE;    //主线程消息标志

//链表加锁函数
void lockFrameList() {
#undef FUNC_CODE
#define FUNC_CODE 0x0001

    long ready = FS_Ready;
    long busy  = FS_Busy;

    while (atomic_compare_exchange_Alva(&(gFrameListInfo.frameFlag), &ready, busy)) {
        mSleep_Alva(1);
    }
}

//链表解锁函数
void unlockFrameList() {
#undef FUNC_CODE
#define FUNC_CODE 0x0002

    atomic_store_Alva(&(gFrameListInfo.frameFlag), FS_Ready);
}

//创建frameList链表
static int createFrameRingDeque(int ImageSize) {
#undef FUNC_CODE
#define FUNC_CODE 0x0003

    int i = 0;

    FrameNode* headNode = (FrameNode*)calloc(1, sizeof(FrameNode));
    headNode->frameData = (unsigned char*)calloc(1, ImageSize);
    if ((NULL == headNode) || (NULL == headNode->frameData)) {
        return -1;
    }
    memset(headNode->matrix, 0, 16 * sizeof(float));
    memset(headNode->frameData, 0, ImageSize);
    headNode->frameListStatusNum = 0;
    headNode->isDeal = 0;
    headNode->isCopy = 0;
    headNode->processing = 0;
    headNode->ImageDataSize = ImageSize;
    headNode->next = headNode;
    for (i = 1; i < FrameListNum; i++) {
        FrameNode* newNode = (FrameNode*)calloc(1, sizeof(FrameNode));
        newNode->frameData = (unsigned char*)calloc(1, ImageSize);
        if ((NULL == newNode) || (NULL == newNode->frameData)) {
            return -1;
        }
        memset(newNode->matrix, 0, 16 * sizeof(float));
        memset(newNode->frameData, 0, ImageSize);
        newNode->frameListStatusNum = 0;
        newNode->isDeal = 0;
        newNode->isCopy = 0;
        newNode->processing = 0;
        newNode->ImageDataSize = ImageSize;
        newNode->next = headNode->next;
        headNode->next = newNode;
    }
    gFrameListInfo.headNode = headNode;
    gFrameListInfo.tailNode = headNode;
    gFrameListInfo.curFrame = headNode;
    return 0;
}

EXPORT_API int FrameInit(int ImageSize) {
#undef FUNC_CODE
#define FUNC_CODE 0x0004

    long status = FIS_Inited;

    if (!atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        if (gFrameListInfo.preFrame->ImageDataSize == ImageSize) {
            LOGE("Already Inited!\n");
            return 0;
        }
        else {
            OWN_ERROR_RETURN(0x003, "Initialized, ImageSize is not equal with last initial.\n");
        }
    }
    int err = 0;
    int i = 0;

    for (i = 0; i < AlgorithmNum; i++) {
        gFrameListInfo.algorithmInfo[i].dependIndex = -1;
        memset(gFrameListInfo.algorithmInfo[i].dependName, 0, 256 * sizeof(char));
        memset(gFrameListInfo.algorithmInfo[i].name, 0, 256 * sizeof(char));
        gFrameListInfo.algorithmInfo[i].dependResultPtr = NULL;
    }
    gFrameListInfo.frameListSize = FrameListNum;
    atomic_store_Alva(&(gFrameListInfo.frameFlag), FS_Ready);
    gFrameListInfo.headNode = NULL;
    gFrameListInfo.tailNode = NULL;
    gFrameListInfo.curFrame = NULL;
    gFrameListInfo.preFrame = (FrameNode*)calloc(1, sizeof(FrameNode));
    gFrameListInfo.preFrame->isDeal = 0;
    gFrameListInfo.preFrame->isCopy = 0;
    gFrameListInfo.preFrame->ImageDataSize = ImageSize;
    gFrameListInfo.preFrame->frameListStatusNum = 0;
    gFrameListInfo.preFrame->processing = 0;
    gFrameListInfo.preFrame->frameData = (unsigned char*)calloc(1, ImageSize);
    gFrameListInfo.preFrame->next = NULL;
    gFrameListInfo.regAlgorithmNum = 0;
    gFrameListInfo.regSyncAlgorithmNum = 0;
    memset(gFrameListInfo.regIsFlag, -1, AlgorithmNum * sizeof(int));
    if ((NULL == gFrameListInfo.preFrame) || (NULL == gFrameListInfo.preFrame->frameData)) {
        OWN_ERROR_RETURN(0x001, "FrameInit frameData is NULL");
    }
    err = createFrameRingDeque(ImageSize);
    if (-1 == err) {
        OWN_ERROR_RETURN(0x002, "FrameInit frameData is NULL");
    }
    atomic_store_Alva(&(mFrameStatus), FIS_Inited);
    atomic_store_Alva(&(mThreadMessage), TM_NONE);
    return 0;
}

int FL_regAlgorithm(char algorithmName[256], int resultSize, AlgorithmType algorithmType, void* initResult) {
#undef FUNC_CODE
#define FUNC_CODE 0x0005

    lockFrameList();
    long status = TM_STOP;

    if (!atomic_compare_exchange_Alva(&(mThreadMessage), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x001, "mainThreadQuit");
    }
    status = FIS_Inited;
    if (atomic_compare_exchange_Alva(&mFrameStatus, &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x002, "uninitialized");
    }
    int i = 0;
    int algorithmIndex = -1;

    for (i = 0; i < AlgorithmNum; i++) {
        if (-1 == gFrameListInfo.regIsFlag[i]) {
            algorithmIndex = i;
            gFrameListInfo.regIsFlag[i] = i;
            break;
        }
    }
    i = 0;
    if (-1 == algorithmIndex) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x003, "Reg too many algorithm\n");
    }
    memcpy_fl(gFrameListInfo.algorithmInfo[algorithmIndex].name, 256 * sizeof(char), algorithmName, 256 * sizeof(char));
    if (AT_Sync == algorithmType) {
        gFrameListInfo.regSyncAlgorithmNum = gFrameListInfo.regSyncAlgorithmNum + 1;
    }
    gFrameListInfo.regAlgorithmNum = gFrameListInfo.regAlgorithmNum + 1;
    for (i = 0; i < FrameListNum; i++) {
        gFrameListInfo.tailNode->gframeListResult[algorithmIndex].resultSize  = resultSize;
        gFrameListInfo.tailNode->gframeListResult[algorithmIndex].isSync      = algorithmType;
        gFrameListInfo.tailNode->gframeListResult[algorithmIndex].isProcess   = 0;
        gFrameListInfo.tailNode->gframeListResult[algorithmIndex].frameResult = (void*)calloc(1, resultSize);
        memset(gFrameListInfo.tailNode->gframeListResult[algorithmIndex].frameResult, 0, resultSize);
        gFrameListInfo.tailNode = gFrameListInfo.tailNode->next;
    }
    gFrameResult[algorithmIndex].resultSize  = resultSize;
    gFrameResult[algorithmIndex].frameResult = (void*)calloc(1, resultSize);
    memcpy_fl(gFrameResult[algorithmIndex].frameResult,resultSize,initResult,resultSize);
    unlockFrameList();
    return algorithmIndex;
}

EXPORT_API int UpdateImage(unsigned char* frameData, float matrix[16]) {
#undef FUNC_CODE
#define FUNC_CODE 0x0006

    if (NULL == frameData) {
        OWN_ERROR_RETURN(0x002, "UpdateImage frameData is NULL");
    }
    if (NULL == matrix) {
        OWN_ERROR_RETURN(0x004, "UpdateImage matrix is NULL");
    }
    lockFrameList();
    long status = TM_STOP;

    if (!atomic_compare_exchange_Alva(&(mThreadMessage), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x001, "mainThreadQuit");
    }
    status = FIS_Inited;
    if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x003, "uninitialized");
    }

    if ((0 == gFrameListInfo.tailNode->processing) && (0 == gFrameListInfo.tailNode->isDeal)) {
        memcpy_fl(gFrameListInfo.tailNode->frameData, gFrameListInfo.tailNode->ImageDataSize, frameData, gFrameListInfo.tailNode->ImageDataSize);
        memcpy_fl(gFrameListInfo.tailNode->matrix, 16 * sizeof(float), matrix, 16 * sizeof(float));
        gFrameListInfo.tailNode->isCopy = 1;
        gFrameListInfo.tailNode = gFrameListInfo.tailNode->next;
    }
    else if ((0 == gFrameListInfo.tailNode->next->processing) && (0 == gFrameListInfo.tailNode->next->isDeal)) {
        gFrameListInfo.tailNode = gFrameListInfo.tailNode->next;
        memcpy_fl(gFrameListInfo.tailNode->frameData, gFrameListInfo.tailNode->ImageDataSize, frameData, gFrameListInfo.tailNode->ImageDataSize);
        memcpy_fl(gFrameListInfo.tailNode->matrix, 16 * sizeof(float), matrix, 16 * sizeof(float));
        gFrameListInfo.tailNode->isCopy = 1;
        gFrameListInfo.tailNode = gFrameListInfo.tailNode->next;
    }
    if ((gFrameListInfo.tailNode == gFrameListInfo.headNode) && (0 == gFrameListInfo.tailNode->processing) && (0 == gFrameListInfo.tailNode->isDeal)) {
        gFrameListInfo.headNode = gFrameListInfo.headNode->next;
    }
    unlockFrameList();
    return 0;
}

EXPORT_API int getBackground(unsigned char * frameData) {
#undef FUNC_CODE
#define FUNC_CODE 0x0007

    if (NULL == frameData) {
        OWN_ERROR_RETURN(0x002, "getBackground frameData is NULL");
    }
    lockFrameList();
    long status = TM_STOP;

    if (!atomic_compare_exchange_Alva(&(mThreadMessage), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x001, "mainThreadQuit");
    }
    status = FIS_Inited;
    if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x003, "uninitialized");
    }

    int j = 0;
    if (0 != gFrameListInfo.regSyncAlgorithmNum) {
        if (1 == gFrameListInfo.headNode->isDeal || gFrameListInfo.headNode->frameListStatusNum >= gFrameListInfo.regSyncAlgorithmNum) {
            memcpy_fl(frameData, gFrameListInfo.headNode->ImageDataSize, gFrameListInfo.headNode->frameData, gFrameListInfo.headNode->ImageDataSize);
            gFrameListInfo.headNode->frameListStatusNum = 0;
            gFrameListInfo.headNode->isCopy = 0;
            gFrameListInfo.headNode->isDeal = 0;
            gFrameListInfo.headNode->processing = 0;
            for (j = 0; j < AlgorithmNum; j++) {
                if ((-1 != gFrameListInfo.regIsFlag[j]) && (AT_Sync == gFrameListInfo.headNode->gframeListResult[j].isSync)) {
                    gFrameListInfo.headNode->gframeListResult[j].isProcess = 0;
                    memcpy_fl(gFrameResult[j].frameResult, gFrameResult[j].resultSize, gFrameListInfo.headNode->gframeListResult[j].frameResult, gFrameListInfo.headNode->gframeListResult[j].resultSize);
                }
            }
            memcpy_fl(gFrameListInfo.preFrame->frameData, gFrameListInfo.headNode->ImageDataSize, gFrameListInfo.headNode->frameData, gFrameListInfo.headNode->ImageDataSize);
            gFrameListInfo.preFrame->isCopy = 1;
        }
        else {
            if (1 == gFrameListInfo.preFrame->isCopy) {
                memcpy_fl(frameData, gFrameListInfo.preFrame->ImageDataSize, gFrameListInfo.preFrame->frameData, gFrameListInfo.preFrame->ImageDataSize);
            }
        }
        if (1 == gFrameListInfo.headNode->isDeal) {
            gFrameListInfo.headNode = gFrameListInfo.headNode->next;
        }
    }
    else {
        memcpy_fl(frameData, gFrameListInfo.headNode->ImageDataSize, gFrameListInfo.headNode->frameData, gFrameListInfo.headNode->ImageDataSize);
    }
    unlockFrameList();
    return 0;
}

int FL_setResultData(int algorithmIndex, void* frameResultData) {
#undef FUNC_CODE
#define FUNC_CODE 0x0008

    if ((algorithmIndex >= AlgorithmNum) || (algorithmIndex < 0)) {
        OWN_ERROR_RETURN(0x002, "FL_setResultData algorithmIndex over the mark");
    }
    if (NULL == frameResultData) {
        OWN_ERROR_RETURN(0x003, "FL_setResultData frameResultData is NULL");
    }
    lockFrameList();
    long status = TM_STOP;

    if (!atomic_compare_exchange_Alva(&(mThreadMessage), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x001, "mainThreadQuit");
    }
    status = FIS_Inited;
    if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x004, "uninitialized");
    }

    if (-1 != gFrameListInfo.regIsFlag[algorithmIndex]) {
        if (AT_Sync == gFrameListInfo.curFrame->gframeListResult[algorithmIndex].isSync) {
            if (0 == gFrameListInfo.curFrame->gframeListResult[algorithmIndex].isProcess) {
                memcpy_fl(gFrameListInfo.curFrame->gframeListResult[algorithmIndex].frameResult, gFrameListInfo.curFrame->gframeListResult[algorithmIndex].resultSize,
                    frameResultData, gFrameListInfo.curFrame->gframeListResult[algorithmIndex].resultSize);
                gFrameListInfo.curFrame->frameListStatusNum++;
                gFrameListInfo.curFrame->gframeListResult[algorithmIndex].isProcess = 1;
                if (gFrameListInfo.curFrame->frameListStatusNum == gFrameListInfo.regSyncAlgorithmNum) {
                    gFrameListInfo.curFrame->isDeal = 1;
                    gFrameListInfo.curFrame->processing = 0;
                }
            }
        }
        else {
            memcpy_fl(gFrameResult[algorithmIndex].frameResult, gFrameResult[algorithmIndex].resultSize, frameResultData, gFrameResult[algorithmIndex].resultSize);
        }
    }
    unlockFrameList();
    return 0;
}

int FL_getData(int algorithmIndex, unsigned char* frameData, float matrix[16]) {
#undef FUNC_CODE
#define FUNC_CODE 0x0009

    if ((algorithmIndex >= AlgorithmNum) || (algorithmIndex < 0)) {
        OWN_ERROR_RETURN(0x002, "FL_getData algorithmIndex over the mark");
    }
    if (NULL == frameData) {
        OWN_ERROR_RETURN(0x003, "FL_getData frameData is NULL");
    }
    if (NULL == matrix) {
        OWN_ERROR_RETURN(0x005, "FL_getData matrix is NULL");
    }
    while (1) {
        lockFrameList();
        long status = TM_STOP;

        if (!atomic_compare_exchange_Alva(&(mThreadMessage), &status, status)) {
            unlockFrameList();
            OWN_ERROR_RETURN(0x001, "mainThreadQuit");
        }
        status = FIS_Inited;
        if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
            unlockFrameList();
            OWN_ERROR_RETURN(0x004, "uninitialized");
        }

        if (-1 == gFrameListInfo.regIsFlag[algorithmIndex]) {
            unlockFrameList();
            OWN_ERROR_RETURN(0x006, "Algorithm united!");
        }
        if (AT_Sync == gFrameListInfo.curFrame->gframeListResult[algorithmIndex].isSync) {
            if (0 == gFrameListInfo.curFrame->processing) {
                gFrameListInfo.curFrame = gFrameListInfo.tailNode->next;
                if ((1 == gFrameListInfo.curFrame->isCopy) && (0 == gFrameListInfo.curFrame->isDeal)) {
                    memcpy_fl(matrix, 16 * sizeof(float), gFrameListInfo.curFrame->matrix, 16 * sizeof(float));
                    memcpy_fl(frameData, gFrameListInfo.curFrame->ImageDataSize, gFrameListInfo.curFrame->frameData, gFrameListInfo.curFrame->ImageDataSize);
                    gFrameListInfo.curFrame->processing = 1;
                    break;
                }
            }
            else {
                if (0 == gFrameListInfo.curFrame->gframeListResult[algorithmIndex].isProcess) {
                    memcpy_fl(matrix, 16 * sizeof(float), gFrameListInfo.curFrame->matrix, 16 * sizeof(float));
                    memcpy_fl(frameData, gFrameListInfo.curFrame->ImageDataSize, gFrameListInfo.curFrame->frameData, gFrameListInfo.curFrame->ImageDataSize);
                    break;
                }
            }
        }
        else {
            if (gFrameListInfo.tailNode->next->isCopy == 1) {
                memcpy_fl(matrix, 16 * sizeof(float), gFrameListInfo.tailNode->next->matrix, 16 * sizeof(float));
                memcpy_fl(frameData, gFrameListInfo.tailNode->next->ImageDataSize, gFrameListInfo.tailNode->next->frameData, gFrameListInfo.tailNode->next->ImageDataSize);
                break;
            }
        }
        unlockFrameList();
        mSleep_Alva(1);
    }
    unlockFrameList();
    return 0;
}

int FL_updateResult(int algorithmIndex, void* frameResultData) {
#undef FUNC_CODE
#define FUNC_CODE 0x000A

    if ((algorithmIndex >= AlgorithmNum) || (algorithmIndex < 0)) {
        OWN_ERROR_RETURN(0x002, "FL_updateResult algorithmIndex over the mark");
    }
    if (NULL == frameResultData) {
        OWN_ERROR_RETURN(0x003, "FL_updateResult frameResultData is NULL");
    }
    lockFrameList();
    long status = TM_STOP;

    if (!atomic_compare_exchange_Alva(&(mThreadMessage), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x001, "mainThreadQuit");
    }
    status = FIS_Inited;
    if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x004, "uninitialized");
    }

    if (-1 != gFrameListInfo.regIsFlag[algorithmIndex]) {
        memcpy_fl(frameResultData, gFrameResult[algorithmIndex].resultSize, gFrameResult[algorithmIndex].frameResult, gFrameResult[algorithmIndex].resultSize);
    }
    else {
        unlockFrameList();
        OWN_ERROR_RETURN(0x005, "Algorithm United!");
    }
    unlockFrameList();
    return 0;
}

void FL_mainThreadQuit() {
#undef FUNC_CODE
#define FUNC_CODE 0x000D

    atomic_store_Alva(&(mThreadMessage), TM_STOP);
}

EXPORT_API int FrameUnit() {
#undef FUNC_CODE
#define FUNC_CODE 0x000B

    long status = FIS_Inited;

    if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        LOGE("uninitialized\n");
        return 0;
    }
    FL_mainThreadQuit();
    int i = 0;
    for (i = 0; i < AlgorithmNum; i++) {
        while (1) {
            lockFrameList();
            if (-1 == gFrameListInfo.regIsFlag[i]) {
                unlockFrameList();
                break;
            }
            unlockFrameList();
            mSleep_Alva(1);
        }
    }
    lockFrameList();
    free(gFrameListInfo.preFrame->frameData);
    gFrameListInfo.preFrame->frameData = NULL;
    free(gFrameListInfo.preFrame);
    gFrameListInfo.preFrame = NULL;
    FrameNode* NewNode = gFrameListInfo.headNode;
    for (i = 0; i < FrameListNum; i++) {
        gFrameListInfo.headNode = gFrameListInfo.headNode->next;
        free(NewNode->frameData);
        NewNode->frameData = NULL;
        NewNode->next = NULL;
        free(NewNode);
        NewNode = gFrameListInfo.headNode;
    }
    NewNode = NULL;
    atomic_store_Alva(&(mFrameStatus), FIS_Free);
    unlockFrameList();

    return 0;
}

int FL_unRegAlgorithm(int algorithmIndex) {
#undef FUNC_CODE
#define FUNC_CODE 0x000C

    if ((algorithmIndex >= AlgorithmNum) || (algorithmIndex < 0)) {
        OWN_ERROR_RETURN(0x001, "FL_unRegAlgorithm algorithmIndex over the mark");
    }
    long status = FIS_Inited;

    if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        OWN_ERROR_RETURN(0x002, "uninitialized");
    }
    lockFrameList();
    int i = 0;
    if (-1 != gFrameListInfo.regIsFlag[algorithmIndex]) {
        free(gFrameResult[algorithmIndex].frameResult);
        gFrameResult[algorithmIndex].frameResult = NULL;
        gFrameListInfo.regAlgorithmNum--;
        if (AT_Sync == gFrameListInfo.tailNode->gframeListResult[algorithmIndex].isSync) {
            gFrameListInfo.regSyncAlgorithmNum--;
        }
        gFrameListInfo.regIsFlag[algorithmIndex] = -1;
        for (i = 0; i < FrameListNum; i++) {
            FrameNode* NewNode = gFrameListInfo.tailNode;
            free(NewNode->gframeListResult[algorithmIndex].frameResult);
            if (1 == NewNode->processing) {
                if (1 == NewNode->gframeListResult[algorithmIndex].isProcess) {
                    NewNode->frameListStatusNum--;
                }
            }
            NewNode->gframeListResult[algorithmIndex].frameResult = NULL;
            gFrameListInfo.tailNode = gFrameListInfo.tailNode->next;
        }
        gFrameListInfo.algorithmInfo[algorithmIndex].dependIndex = -1;
        gFrameListInfo.algorithmInfo[algorithmIndex].dependResultPtr = NULL;
        memset(gFrameListInfo.algorithmInfo[algorithmIndex].dependName, 0, 256 * sizeof(char));
        memset(gFrameListInfo.algorithmInfo[algorithmIndex].name, 0, 256 * sizeof(char));
    }
    unlockFrameList();
    return 0;
}

int FL_setDepend(int algorithmIndex, char name[256]) {
#undef FUNC_CODE
#define FUNC_CODE 0x000E

    if ((algorithmIndex >= AlgorithmNum) || (algorithmIndex < 0)) {
        OWN_ERROR_RETURN(0x002, "setDepend algorithmIndex over the mark");
    }
    lockFrameList();
    long status = TM_STOP;

    if (!atomic_compare_exchange_Alva(&(mThreadMessage), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x001, "mainThreadQuit");
    }
    status = FIS_Inited;
    if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x005, "uninitialized");
    }

    int i = 0;

    if (-1 == gFrameListInfo.regIsFlag[algorithmIndex]) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x003, "setDepend algorithmIndex is not register");
    }
    if (name == NULL) {
        gFrameListInfo.algorithmInfo[algorithmIndex].dependResultPtr = NULL;
        gFrameListInfo.algorithmInfo[algorithmIndex].dependIndex = -1;
        memset(gFrameListInfo.algorithmInfo[algorithmIndex].dependName, 0, 256 * sizeof(char));
        unlockFrameList();
        return 0;
    }
    for (i = 0; i < AlgorithmNum; i++) {
        if (0 == strcmp(gFrameListInfo.algorithmInfo[i].name, name)) {
            if (AT_Sync == gFrameListInfo.curFrame->gframeListResult[i].isSync) {
                gFrameListInfo.algorithmInfo[algorithmIndex].dependResultPtr = gFrameListInfo.curFrame->gframeListResult[i].frameResult;
            }
            else {
                gFrameListInfo.algorithmInfo[algorithmIndex].dependResultPtr = gFrameResult[i].frameResult;
            }
            gFrameListInfo.algorithmInfo[algorithmIndex].dependIndex = i;
            memcpy_fl(gFrameListInfo.algorithmInfo[algorithmIndex].dependName, 256 * sizeof(char), name, 256 * sizeof(char));
            unlockFrameList();
            return 0;
        }
    }
    unlockFrameList();
    OWN_ERROR_RETURN(0x004, "setDepend name is not register");
}

int FL_getDependName(int algorithmIndex, char name[256]) {
#undef FUNC_CODE
#define FUNC_CODE 0x000F

    if ((algorithmIndex >= AlgorithmNum) || (algorithmIndex < 0)) {
        OWN_ERROR_RETURN(0x002, "getDependName algorithmIndex over the mark");
    }
    lockFrameList();
    long status = TM_STOP;

    if (!atomic_compare_exchange_Alva(&(mThreadMessage), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x001, "mainThreadQuit");
    }
    status = FIS_Inited;
    if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x005, "uninitialized");
    }

    if (-1 == gFrameListInfo.regIsFlag[algorithmIndex]) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x003, "getDependName algorithmIndex is not register");
    }
    if (-1 == gFrameListInfo.algorithmInfo[algorithmIndex].dependIndex) {
        memset(name, 0, 256 * sizeof(char));
        unlockFrameList();
        OWN_ERROR_RETURN(0x004, "getDependName algorithmIndex is not depend");
    }
    else {
        int dependIndex = gFrameListInfo.algorithmInfo[algorithmIndex].dependIndex;
        memcpy_fl(name, 256 * sizeof(char), gFrameListInfo.algorithmInfo[dependIndex].name, 256 * sizeof(char));
    }
    unlockFrameList();
    return 0;
}

int FL_getDependResultSize(int algorithmIndex) {
#undef FUNC_CODE
#define FUNC_CODE 0x0010

    if ((algorithmIndex >= AlgorithmNum) || (algorithmIndex < 0)) {
        OWN_ERROR_RETURN(0x002, "getDependResultSize algorithmIndex over the mark");
    }
    lockFrameList();
    long status = TM_STOP;

    if (!atomic_compare_exchange_Alva(&(mThreadMessage), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x001, "mainThreadQuit");
    }
    status = FIS_Inited;
    if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x005, "uninitialized");
    }
    if (-1 == gFrameListInfo.regIsFlag[algorithmIndex]) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x003, "getDependResultSize algorithmIndex is not register");
    }
    if (-1 == gFrameListInfo.algorithmInfo[algorithmIndex].dependIndex) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x004, "getDependResultSize algorithmIndex is not depend");
    }

    int dependIndex = gFrameListInfo.algorithmInfo[algorithmIndex].dependIndex;
    int dependResultSize = gFrameListInfo.curFrame->gframeListResult[dependIndex].resultSize;
    unlockFrameList();
    return dependResultSize;
}

int FL_getDependResult(int algorithmIndex, void* reuslt) {
#undef FUNC_CODE
#define FUNC_CODE 0x0011

    if ((algorithmIndex >= AlgorithmNum) || (algorithmIndex < 0)) {
        OWN_ERROR_RETURN(0x002, "getDependResult algorithmIndex over the mark");
    }
    lockFrameList();
    long status = TM_STOP;

    if (!atomic_compare_exchange_Alva(&(mThreadMessage), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x001, "mainThreadQuit");
    }
    status = FIS_Inited;
    if (atomic_compare_exchange_Alva(&(mFrameStatus), &status, status)) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x006, "uninitialized");
    }
    if (-1 == gFrameListInfo.regIsFlag[algorithmIndex]) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x003, "getDependResult algorithmIndex is not register");
    }
    if (reuslt == NULL) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x004, "getDependResult reuslt is NULL");
    }
    if (-1 == gFrameListInfo.regIsFlag[gFrameListInfo.algorithmInfo[algorithmIndex].dependIndex]) {
        unlockFrameList();
        OWN_ERROR_RETURN(0x005, "getDependResult algorithmIndex is not depend");
    }
    int dependIndex = gFrameListInfo.algorithmInfo[algorithmIndex].dependIndex;
    int dependResultSize = gFrameListInfo.curFrame->gframeListResult[dependIndex].resultSize;
    memcpy_fl(reuslt, dependResultSize, gFrameListInfo.algorithmInfo[algorithmIndex].dependResultPtr, dependResultSize);
    unlockFrameList();
    return 0;
}
