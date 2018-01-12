#pragma once
// Capture.h for class CCapture

#include <dshow.h>
//#include <qedit.h>
#include <atlbase.h>
#include <string>
#include <iostream>
#define VI_NUM_TYPES    20 //MGB
using namespace std;
#if !defined(CAPTURE_H_________)
#define CAPTURE_H_________

// image size: 160*120  176*144   320*240  640*480  1024*1806
#define IMG_WIDTH 1280
#define IMG_HEIGHT 720

typedef void(*capCallBackFunc)(LPVOID lParam);
enum DeviceType { DTypeVideo, DTypeAudio };
class CSampleGrabberCB; // 用于不会帧数据保存图片的接口
interface ISampleGrabberCB : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SampleCB(
        double SampleTime,
        IMediaSample *pSample) = 0;

    virtual HRESULT STDMETHODCALLTYPE BufferCB(
        double SampleTime,
        BYTE *pBuffer,
        LONG BufferLen) = 0;

    virtual ~ISampleGrabberCB() {}
};

interface ISampleGrabber : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SetOneShot(
        BOOL OneShot) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetMediaType(
        const AM_MEDIA_TYPE *pType) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType(
        AM_MEDIA_TYPE *pType) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetBufferSamples(
        BOOL BufferThem) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer(
        LONG *pBufferSize,
        LONG *pBuffer) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCurrentSample(
        IMediaSample **ppSample) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetCallback(
        ISampleGrabberCB *pCallback,
        LONG WhichMethodToCallback) = 0;

    virtual ~ISampleGrabber() {}
};

class CCapture
{
    friend class CSampleGrabberCB;
public:
    // 设置回调函数 用于处理获取的图片帧数据
    //  CDialog *m_dlgParent;
    capCallBackFunc calFunc;
    void SetCallBKFun(capCallBackFunc f);
    /////////////////////////////////
    CCapture();
    virtual ~CCapture();
    int EnumDevice(HWND hCmbList, DeviceType deviceType); // 设备枚举
                                                          //	void SaveGraph(TCHAR *wFileName);	// 保存滤波器链表
    void SetCameraFormat(HWND hwndParent);	// 设置摄像头的视频格式
    void SetCameraFilter(HWND hwndParent);	// 设置摄像头的图像参数
    HRESULT CaptureVideo(string inFileName);	// 捕获保存视频
    HRESULT CaptureImage(string inFileName);	// 抓取保存图片
    HRESULT CaptureImage(); // 抓取图片并显示
    HRESULT Preview(int iDevVideoID, int iDevAudioID = 0, HWND hAudio = NULL);	// 采集预览视频
    HRESULT InitCaptureGraphBuilder();	// 创建滤波器管理器，查询其各种控制接口
    void StopCapture();  // 停止捕获
    void FreeMediaType(AM_MEDIA_TYPE &mt);  // 释放对象内存
    void setSize(int w, int h);//设置捕捉视频的格式大小
    int GetPreWidth();
    int GetPreHeight();
    void SetPreWidth(int width);
    void SetPreHeight(int height);
    void SetOnShot(BOOL bFlag);   // 设置是否捕获帧数据
                                  //    void SetParent(CDialog *pdlg);
protected:
    bool BindFilter(int iDeviceID, IBaseFilter **pOutFilter, DeviceType deviceType); // 把指定的设备滤波器捆绑到链表中
    void ResizeVideoWindow();			// 更改视频显示窗口
    HRESULT SetupVideoWindow();			// 设置视频显示窗口的特性
                                        //   static UINT ThreadFunDrawText(LPVOID lParam);
private:
    HWND m_hWnd;			// 视频显示窗口的句柄
    IBaseFilter *m_pVideoCap;		// 视频捕获滤波器
    IBaseFilter *m_pAudioCap;		// 音频捕获滤波器
    CComPtr<ISampleGrabber> m_pGrabber;		// 抓取图片滤波器
    IBaseFilter *m_pMux;	// 写文件滤波器
    ICaptureGraphBuilder2 *m_pCapGB;	// 增强型捕获滤波器链表管理
    IGraphBuilder *m_pGB;	// 滤波链表管理器
    IVideoWindow *m_pVW;	// 视频显示窗口接口
    IMediaControl *m_pMC;	// 媒体控制接口
    static bool m_bRecording;		// 录制视频标志
    float width;
    float height;
    IBaseFilter *m_pXviDCodec;   //mpeg4 滤波器
    GUID CAPTURE_MODE;
    GUID mediaSubtypes[VI_NUM_TYPES];
    GUID cur_media_type;
    //   HANDLE handlel;
};
#endif

