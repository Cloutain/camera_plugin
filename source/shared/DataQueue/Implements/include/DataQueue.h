#pragma once
#ifndef  __DATAQUEUE_H__
#define  __DATAQUEUE_H__

/**
 * @file      DataQueue.h
 * @brief     使用时，先初始化链表(FrameInit),然后注册算法(FL_regAlgorithm),注册完算法可设置依赖的
              算法(FL_setDepend),接下来往链表中添加数据(UpdateImage),然后从链表中拿数据处理(FL_get
              Data),然后设置结果(FL_setResultData),如果有依赖算法，则可调用(FL_getDependName)获取依
              赖算法的名字，然后可以获取依赖算法结果大小(FL_getDependResultSize)和依赖算法的结果(FL
              _getDependResult),接着UI从链表中拿处理过的数据(getBackground),并拿此数据的处理结果(FL
              _updateResult),用户可以随时调用反注册函数来反注册注册过的算法(FL_unRegAlgorithm)，最后
              反初始化函数(FrameUnit).
 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief 枚举类型,为注册函数FL_regAlgorithm的第二个参数
     */
    typedef enum _AlgorithmType_ {
        AT_Sync = 0,           /*!< 同步算法 */
        AT_Asyn = 1,           /*!< 异步算法 */
    }AlgorithmType;

    /**
     * @brief 注册算法
     * @param algorithmName: 算法名字
     * @param resultSize: 算法处理之后结果大小
     * @param algorithmType: AT_Sync 同步  AT_Asyn 异步 @see AlgorithmType
     * @param initResult: 算法处理结果的初始值
     * @return: int型，注册算法的索引号algorithmIndex
     *         -<em> < 0 </em> fail，错误码
     *         -<em> >=0 </em> succeed，算法索引号
     */
    int FL_regAlgorithm(char algorithmName[256], int resultSize, AlgorithmType algorithmType, void* initResult);

    /**
     * @brief 反注册算法,释放注册算法时申请的空间
     * @param algorithmIndex: 注册算法时FL_regAlgorithm函数的返回值 @see FL_regAlgorithm
     * @return: int型
     *         -<em> <0 </em> fail，错误码
     *         -<em>  0 </em> succeed
     */
    int FL_unRegAlgorithm(int algorithmIndex);

    /**
     * @brief 从链表中拿一帧最新数据处理
     * @param algorithmIndex: 注册算法时FL_regAlgorithm函数的返回值
     * @param frameData: 调用者申请的内存空间,大小为FrameInit参数ImageSize的值 @see FrameInit
     * @param matrix[16]: 纹理的变化矩阵
     * @return: int型
     *         -<em> <0 </em> fail，错误码
     *         -<em>  0 </em> succeed
     */
    int FL_getData(int algorithmIndex, unsigned char* frameData, float matrix[16]);

    /**
     * @brief 向处理帧添加处理结果
     * @param algorithmIndex: 注册算法时FL_regAlgorithm函数的返回值
     * @param frameResultData: 调用者申请的内存空间,大小为FL_regAlgorithm函数参数resultSize的值 @see FL_regAlgorithm
     * @return: int型
     *         -<em> <0 </em> fail，错误码
     *         -<em>  0 </em> succeed
     */
    int FL_setResultData(int algorithmIndex, void * frameResultData);

    /**
     * @brief UI获取数据结果并显示
     * @param algorithmIndex: 注册算法时FL_regAlgorithm函数的返回值
     * @param frameResultData: 调用者申请的内存空间,大小为FL_regAlgorithm函数参数resultSize的值 @see FL_regAlgorithm
     * @return: int型
     *         -<em> <0 </em> fail，错误码
     *         -<em>  0 </em> succeed
     */
    int FL_updateResult(int algorithmIndex, void* frameResultData);

    /**
     * @brief 设置算法依赖
     * @param algorithmIndex: 注册算法时FL_regAlgorithm函数的返回值 @see FL_regAlgorithm
     * @param name: 要依赖的算法名字,name为NULL时,不设置依赖并返回0
     * @return: int型
     *         -<em> <0 </em> fail，错误码
     *         -<em>  0 </em> succeed
     */
    int FL_setDepend(int algorithmIndex, char name[256]);

    /**
     * @brief 获取所依赖算法的名字
     * @param algorithmIndex: 注册算法时FL_regAlgorithm函数的返回值 @see FL_regAlgorithm
     * @param name: 获取依赖的名字的空间
     * @return: int型
     *         -<em> <0 </em> fail，错误码
     *         -<em>  0 </em> succeed
     */
    int FL_getDependName(int algorithmIndex, char name[256]);

    /**
     * @brief 获取所依赖算法的结果大小
     * @param algorithmIndex: 注册算法时FL_regAlgorithm函数的返回值 @see FL_regAlgorithm
     * @return: int型
     *         -<em> <0 </em> fail，错误码
     *         -<em> >0 </em> succeed,结果大小
     */
    int FL_getDependResultSize(int algorithmIndex);

    /**
     * @brief 从链表中拿一帧最新数据处理
     * @param algorithmIndex: 注册算法时FL_regAlgorithm函数的返回值 @see FL_regAlgorithm
     * @param reuslt: 调用者申请的内存空间,大小为FL_getDependResultSize函数的返回值 @see FL_getDependResultSize
     * @return: int型
     *         -<em> <0 </em> fail，错误码
     *         -<em>  0 </em> succeed
     */
    int FL_getDependResult(int algorithmIndex, void* reuslt);

#ifdef __cplusplus

}
#endif

#endif
