// Example low level rendering Unity plugin
#pragma warning(disable:4244)
#pragma warning(disable:4197)
#pragma warning(disable:4800)
#include "Config.h"
#include "PlatformBase.h"
#include "RenderAPI.h"
#include <Windows.h>
#include <assert.h>
#include <math.h>
#include "capture.h"
#include <vector>
#include <mutex>      
#include <stdio.h>
#include <string>
#include <strmif.h>
#include <uuids.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include "DataQueueInterface.h"

#define LOG_TAG "ReaderingPlugin"
#undef AVS_Clamp
#define AVS_Clamp(x, minValue, maxValue) ((x < minValue) ? (minValue) : ((x > maxValue) ? (maxValue) : (x)))
#define BUF_NUM 1
#define VER_UNITY
/*testCode:*/
ofstream fin1("test1.log");

    
typedef enum _arModule 
{
    AR_NONE = -1,//无模块
    AR_IR = 0,   //IR模块
    AR_SLAM =1, //SLAM 模块
    AR_CR =2 ,  //CR模块
    AR_FR=3,    //FR模块
    AR_IR_SEARCH_IMAGE = 5, //IR模块中图像识别模块
    AR_CAD = 6, //CAD模块
} arModule;

typedef enum _arMode 
{
    MODE_REC = 0,    //0: 嵌入 unity ，rec 调用识别核心
    MODE_UNITY = 1,  //1: 嵌入 aradk.net, 自己控制摄头控制

} arMode;

// ====================
#ifdef VER_REC
#include "REC_DLL.h"
#endif
CCapture * cap;
int m_CameraId = -1;
int m_moudle = -2;
HANDLE handle;
bool start_preview = false;
HANDLE hMutex;
HANDLE hMutex1;
typedef void(*capCallBackFunc)(LPVOID lParam);

// --------------------------------------------------------------------------
// SetTimeFromUnity, an example function we export which is called by one of the scripts.

static float g_Time;
std::mutex g_mtx;           // locks access to counter


//static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;



// --------------------------------------------------------------------------
// SetTextureFromUnity, an example function we export which is called by one of the scripts.

static void* g_TextureHandle = NULL;
static float   g_TextureWidth  = 1280.0f;
static float   g_TextureHeight = 720.0f;
static char * g_pTexBuf = NULL;
static unsigned int g_texBuf_len = 0;
static char * g_pGray = NULL;
static char* dest = NULL;
static float g_pArr[150] = {0};
static unsigned int g_arr_len = 24;
static char g_tar_name[50] = { 0 };
static char* g_tempBuf = NULL;

/*
0: 嵌入 unity ，rec 调用识别核心
1: 嵌入 aradk.net, 自己控制摄头控制
*/
static  arMode g_mode;

HANDLE g_hThread;
static bool g_quit = false;
static int g_preview = 0;
static int g_camera_index = -1;
static RenderAPI* s_CurrentAPI = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;




//void UNITY_INTERFACE_API OnRenderEvent(int eventID);
//int video_rec_callback(char* pVideo1, char* pVideo2, char* pVideo3, INT32 width, INT32 height, float* arr, INT32 * arr_len);


// --------------------------------------------------------------------------
// SetMeshBuffersFromUnity, an example function we export which is called by one of the scripts.

static void* g_VertexBufferHandle = NULL;
static int g_VertexBufferVertexCount;

struct MeshVertex
{
    float pos[3];
    float normal[3];
    float uv[2];
};
static std::vector<MeshVertex> g_VertexSource;


/**
* 
未被使用
@pragma1
        0: 嵌入 unity ，rec 调用识别核心
        1: 嵌入 aradk.net, 自己控制摄头控制
*/
extern "C" void  SetMode(arMode mode)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0001
	g_mode = mode;
}


#define CLAMP(a, s, m) ((a) < (s)? (s) : ((a) > (m) ? (m) : (a)))
void YUY2_RGBA_1(unsigned char*YUY2buff, unsigned char *RGBAbuff, unsigned char *gray, int width, int height)
{
    //B = 1.164(Y - 16)         + 2.018(U - 128)    
    //G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)    
    //R = 1.164(Y - 16) + 1.596(V - 128)    
    int half = width / 2;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < half; j++)
        {
            unsigned char Y = 0;
            unsigned char Y1 = 0;
            unsigned char U = 0;
            unsigned char V = 0;

            Y = YUY2buff[(i * half + j) * 4 + 0];
            U = YUY2buff[(i * half + j) * 4 + 1];
            Y1 = YUY2buff[(i * half + j) * 4 + 2];
            V = YUY2buff[(i * half + j) * 4 + 3];

            float B = (1.164f * (Y - 16) + 2.018 * (U - 128));
            float G = (1.164f * (Y - 16) - 0.813f * (V - 128) - 0.391f * (U - 128));
            float R = (1.164f * (Y - 16) + 1.596f * (V - 128));


            float B1 = (1.164f * (Y1 - 16) + 2.018 * (U - 128));
            float G1 = (1.164f * (Y1 - 16) - 0.813f * (V - 128) - 0.391f * (U - 128));
            float R1 = (1.164f * (Y1 - 16) + 1.596f * (V - 128));

            int temp_b = (int)B;
            int temp_g = (int)G;
            int temp_r = (int)R;

            RGBAbuff[(i * width + j * 2 + 0) * 4 + 0] = CLAMP(temp_r, 0, 255);
            RGBAbuff[(i * width + j * 2 + 0) * 4 + 1] = CLAMP(temp_g, 0, 255);
            RGBAbuff[(i * width + j * 2 + 0) * 4 + 2] = CLAMP(temp_b, 0, 255);
            RGBAbuff[(i * width + j * 2 + 0) * 4 + 3] = 255;


            temp_b = (int)B1;
            temp_g = (int)G1;
            temp_r = (int)R1;
            RGBAbuff[(i * width + j * 2 + 1) * 4 + 0] = CLAMP(temp_r, 0, 255);
            RGBAbuff[(i * width + j * 2 + 1) * 4 + 1] = CLAMP(temp_g, 0, 255);
            RGBAbuff[(i * width + j * 2 + 1) * 4 + 2] = CLAMP(temp_b, 0, 255);
            RGBAbuff[(i * width + j * 2 + 1) * 4 + 3] = 255;
        }
    }

}



/**
* 预览宽高与纹理宽比相同，最邻近插值
* @return 0:正常返回
*/
static int neighbor_insert(double scale_x, double scale_y, char* src, char* dest, int src_height, int src_width, int dst_height, int dst_width,int channel)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0002
    int ret = 0;
    for (int i = 0; i < dst_height; i++)
    {
        int sy = floor(i*scale_y);
        sy = sy < src_height ? sy : src_height - 1;
        for (int j = 0; j < dst_width; j++)
        {
            int sx = floor(j*scale_x);
            sx = sx < src_width ? sx : src_width - 1;
            for (int k = 0; k < channel; k++)
            {
                *(dest + i*dst_width * channel + j * channel + k) = *(src + sy*src_width * channel + sx * channel+ k);
            }
        }
    }
    return ret;
}


/**
* 裁剪宽以适应纹理要求宽度大小
* @return 0:正常返回
*/
static int cut_width(char* beCutBuff, char* dst, int temp_width,int channel)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0003
    char* temp_cut = NULL;
    char* temp_dst = NULL;

    for (int i = 0; i < (int)g_TextureHeight; i++)
    {
        temp_cut = (beCutBuff + i*temp_width*channel);
        temp_dst = (dst + i * ((int)g_TextureWidth) * channel);
        for (int j = 0; j < (int)g_TextureWidth; j++)
        {
            memcpy(temp_dst, temp_cut, channel);
            temp_cut += channel;
            temp_dst += channel;
        }
    }
    return 0;
}

/**
* 裁剪高以适应纹理要求高度大小
* @return 0:正常返回
*/
static int cut_height(char* beCutBuff,char* dst,int channel)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0004
    int ret = 0;
    memcpy(dst, beCutBuff, sizeof(char)*g_TextureHeight*g_TextureWidth * channel);
    return ret;
}

/**
* 裁剪相机图片大小以适应纹理宽高
* @return 0:正常返回
*/
static int ResizeCameraImage(char* g_pTexBuf,char* dest,int channel)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0005
    int ret = 0;
    double p_width = cap->GetPreWidth() / g_TextureWidth;
    double p_height = cap->GetPreHeight() / g_TextureHeight;
    if (p_width == p_height)
    {
        if (p_width == 1.0f)
        {
            memcpy(dest, g_pTexBuf, g_TextureHeight*g_TextureWidth * 4);
            return ret;
        }
        else
        {
            ret=neighbor_insert(p_width, p_height, g_pTexBuf, dest, cap->GetPreHeight(), cap->GetPreWidth(), g_TextureHeight, g_TextureWidth, channel);
        }
    }
    else if(p_width < p_height)
    {
        int temp_height = floor(cap->GetPreHeight() / p_width);
        char* temp_buf = (char*)malloc(sizeof(char)* temp_height*g_TextureWidth*channel);
        ret = neighbor_insert(p_width, p_width, g_pTexBuf, temp_buf, cap->GetPreHeight(), cap->GetPreWidth(), temp_height, g_TextureWidth, channel);
        ret=cut_height(temp_buf, dest,channel);
        if (temp_buf)
        {
            free(temp_buf);
            temp_buf = NULL;
        }
    }
    else
    {
        int temp_width = floor(cap->GetPreWidth() / p_height);
        char* temp_buf = (char*)malloc(sizeof(char)* temp_width*g_TextureHeight * channel);
        ret=neighbor_insert(p_width, p_width, g_pTexBuf, temp_buf, cap->GetPreHeight(), cap->GetPreWidth(),g_TextureHeight, temp_width, channel);
        ret=cut_width(temp_buf, dest, temp_width,channel);
        if (temp_buf)
        {
            free(temp_buf);
            temp_buf = NULL;
        }
    }
    return ret;
}

/**
* 设置camera捕捉到一帧数据后回调函数
* @return;
*/


static void userUpdateCb(LPVOID lParam)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0006
    int ret = 0;
    float matrix[16] = { 0.0f };
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
            {
                matrix[i * 4 + j] = 1;
            }
        }
    }
    char* cur_buffer = (char*)(lParam);
    if (!s_CurrentAPI)
    {
        return;
    }

    WaitForSingleObject(hMutex1, INFINITE);
   
    YUY2_RGBA_1((unsigned char*)cur_buffer, (unsigned char*)(g_pTexBuf), NULL, cap->GetPreWidth(), cap->GetPreHeight());

    ResizeCameraImage(g_pTexBuf,dest, 4);

    //剪裁之后的纹理放入链表
    ret = UpdateImage((unsigned char *)dest, matrix);
    ReleaseMutex(hMutex1);
    return ;
}

/**
* 新建相机的预览捕捉线程
* @return 
*/
static DWORD WINAPI ClientThread(LPVOID lpParam)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0007
    bool start_preview_temp = false;
    while (1)
    {
        WaitForSingleObject(hMutex, INFINITE);
        start_preview_temp = start_preview;
        ReleaseMutex(hMutex);
        if (start_preview_temp)
            break;
    }
    if (!FAILED(cap->Preview(m_CameraId)))
        return 1;
    return 0;
}



/**
* 获取相机预览时的图片宽度
* @return 图片宽度
*/
extern "C" int getCameraPreviewWidth()
{
#undef FUNC_CODE
#define FUNC_CODE 0x0008
    return cap->GetPreWidth();
}

/**
* 获取相机预览时的图片高度
* @return 图片高度
*/
extern "C" int getCameraPreviewHeight()
{
#undef FUNC_CODE
#define FUNC_CODE 0x0009
    return cap->GetPreHeight();
}


/**
* 初始化相机
* @return
*/
extern "C" bool  InitCamera(int camera_index)
{
#undef FUNC_CODE
#define FUNC_CODE 0x000A
    hMutex = CreateMutex(nullptr, false, nullptr);
    hMutex1 = CreateMutex(nullptr, false, nullptr);
    cap = new CCapture();
    m_CameraId = camera_index;
    m_moudle = 0;
    start_preview = false;

    cap->SetPreWidth(1280);
    cap->SetPreHeight(720);

    g_pTexBuf = (char*)malloc(sizeof(char)* cap->GetPreWidth()*cap->GetPreHeight() * 4);
    memset(g_pTexBuf, 0, cap->GetPreWidth()*cap->GetPreHeight() * 4);
    g_pGray= (char*)malloc(sizeof(char)* cap->GetPreWidth()*cap->GetPreHeight() * 1);
    memset(g_pGray, 0, cap->GetPreWidth()*cap->GetPreHeight() * 1);

    if (FAILED(cap->InitCaptureGraphBuilder()))
    {
        return false;
    }
    //设置相机捕捉帧后的回调函数
    cap->SetCallBKFun(userUpdateCb);
    //初始化子线程，预览时候函数调用
    handle = CreateThread(NULL, 0, ClientThread, NULL, 0, NULL);
    if (!handle)
        return false;
    return false;
}

/**
* 设置预览，相机开始捕捉帧图片
* @return
*/
extern "C" void  SetPreview(int preview)
{
#undef FUNC_CODE
#define FUNC_CODE 0x000B
    WaitForSingleObject(hMutex, INFINITE);
    start_preview = (bool)preview;
    ReleaseMutex(hMutex);
    return;
}


/**
* 关闭相机，释放每一帧的资源和捕捉的Capture类
* @return 图片宽度
*/
extern "C" void  CloseCamera()
{
#undef FUNC_CODE
#define FUNC_CODE 0x000C
    WaitForSingleObject(hMutex, INFINITE);
    start_preview = false;
    ReleaseMutex(hMutex);
    CloseHandle(handle);
    if (g_pTexBuf)
    {
       free(g_pTexBuf);
       g_pTexBuf = NULL;
    }
    if (g_pGray)
    {
        free(g_pGray);
        g_pGray = NULL;
    }
    if (dest)
    {
        free(dest);
        dest = NULL;
    }
    free(cap);
}


/**
* 获取一帧图片的长度信息
* @return
*/
extern "C" void  GetArr(float** arr, unsigned int &arr_Len)
{
#undef FUNC_CODE
#define FUNC_CODE 0x000D
	*arr = g_pArr;
	arr_Len = g_arr_len;
}


/**
* unity渲染有关的函数调用
* @return
*/

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
#undef FUNC_CODE
#define FUNC_CODE 0x000E
    // Create graphics API implementation upon initialization
    if (eventType == kUnityGfxDeviceEventInitialize)
    {
        assert(s_CurrentAPI == NULL);
        s_DeviceType = s_Graphics->GetRenderer();
        s_CurrentAPI = CreateRenderAPI(s_DeviceType);
        // REC_Init(-1, video_rec_callback);
    }
    // Let the implementation process the device related events
    if (s_CurrentAPI)
    {
        s_CurrentAPI->ProcessDeviceEvent(eventType, s_UnityInterfaces);
    }
    // Cleanup graphics API implementation upon shutdown
    if (eventType == kUnityGfxDeviceEventShutdown)
    {
        delete s_CurrentAPI;
        s_CurrentAPI = NULL;
        s_DeviceType = kUnityGfxRendererNull;
        CloseCamera();
    }
}


/**
* 响应渲染事件更新纹理像素
* @return
*/
static void UpdateTexturePixels()
{
#undef FUNC_CODE
#define FUNC_CODE 0x0010
    if (NULL == g_TextureHandle || 0 == g_texBuf_len)
    {
        return;
    }
    void* textureHandle = g_TextureHandle;
    if (!textureHandle) 
    {
        return;
    }
    WaitForSingleObject(hMutex1, INFINITE);
    //链表中拿数据
    Sleep(5);
    getBackground((unsigned char*)dest);
    s_CurrentAPI->ModifyTexture(textureHandle, g_TextureWidth,g_TextureHeight, g_TextureWidth * 4, dest);
    ReleaseMutex(hMutex1);
    Sleep(5);
  }


/**
* unity处理渲染的事件
* @return
*/
void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0011
    // Unknown / unsupported graphics device type? Do nothing
    if (s_CurrentAPI == NULL)
        return;
    UpdateTexturePixels();
}


/**
* 更新纹理
* @return
*/
extern "C" void  UpdateTexture()
{
#undef FUNC_CODE
#define FUNC_CODE 0x0012
    OnRenderEvent(1);
}


/**
* 设置纹理ID和纹理宽高，分配unity需要的纹理空间
* @return
*/
extern "C" void  SetTextureFromUnity(void* textureHandle,int w, int h)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0013
	g_TextureHandle = textureHandle;
	g_TextureWidth = w;
	g_TextureHeight = h;
	g_texBuf_len = w*h *4;
    //unity需要的纹理空间
    dest = (char*)malloc(sizeof(char)*g_texBuf_len);
    //初始化unity传进来的要渲染的纹理大小
    FrameInit(g_texBuf_len);
}


/**
* unity插件加载函数
* @return
*/
extern "C" void	 UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0014
	s_UnityInterfaces = unityInterfaces;
	s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
	s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
	
	// Run OnGraphicsDeviceEvent(initialize) manually on plugin load
	OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

/**
* unity插件卸载函数
* @return
*/
extern "C" void UnityPluginUnload()
{
#undef FUNC_CODE
#define FUNC_CODE 0x0015
	s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
	CloseCamera();
}


/**
* 
  unity渲染事件处理函数
* @return
*/
extern "C" UnityRenderingEvent  GetRenderEventFunc()
{
#undef FUNC_CODE
#define FUNC_CODE 0x0016
	return OnRenderEvent;
}


extern "C" void  GetImg(char** pImg, char** pGray)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0017
    UpdateTexture();
}



/**
* 未被使用
unity设置中渲染的时间
* @return
*/
extern "C" void  SetTimeFromUnity(float t)
{
#undef FUNC_CODE
#define FUNC_CODE 0x0018
    g_Time = t;
}





/*

*/
