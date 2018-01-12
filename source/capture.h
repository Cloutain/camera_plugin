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
class CSampleGrabberCB; // ���ڲ���֡���ݱ���ͼƬ�Ľӿ�
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
    // ���ûص����� ���ڴ����ȡ��ͼƬ֡����
    //  CDialog *m_dlgParent;
    capCallBackFunc calFunc;
    void SetCallBKFun(capCallBackFunc f);
    /////////////////////////////////
    CCapture();
    virtual ~CCapture();
    int EnumDevice(HWND hCmbList, DeviceType deviceType); // �豸ö��
                                                          //	void SaveGraph(TCHAR *wFileName);	// �����˲�������
    void SetCameraFormat(HWND hwndParent);	// ��������ͷ����Ƶ��ʽ
    void SetCameraFilter(HWND hwndParent);	// ��������ͷ��ͼ�����
    HRESULT CaptureVideo(string inFileName);	// ���񱣴���Ƶ
    HRESULT CaptureImage(string inFileName);	// ץȡ����ͼƬ
    HRESULT CaptureImage(); // ץȡͼƬ����ʾ
    HRESULT Preview(int iDevVideoID, int iDevAudioID = 0, HWND hAudio = NULL);	// �ɼ�Ԥ����Ƶ
    HRESULT InitCaptureGraphBuilder();	// �����˲�������������ѯ����ֿ��ƽӿ�
    void StopCapture();  // ֹͣ����
    void FreeMediaType(AM_MEDIA_TYPE &mt);  // �ͷŶ����ڴ�
    void setSize(int w, int h);//���ò�׽��Ƶ�ĸ�ʽ��С
    int GetPreWidth();
    int GetPreHeight();
    void SetPreWidth(int width);
    void SetPreHeight(int height);
    void SetOnShot(BOOL bFlag);   // �����Ƿ񲶻�֡����
                                  //    void SetParent(CDialog *pdlg);
protected:
    bool BindFilter(int iDeviceID, IBaseFilter **pOutFilter, DeviceType deviceType); // ��ָ�����豸�˲�������������
    void ResizeVideoWindow();			// ������Ƶ��ʾ����
    HRESULT SetupVideoWindow();			// ������Ƶ��ʾ���ڵ�����
                                        //   static UINT ThreadFunDrawText(LPVOID lParam);
private:
    HWND m_hWnd;			// ��Ƶ��ʾ���ڵľ��
    IBaseFilter *m_pVideoCap;		// ��Ƶ�����˲���
    IBaseFilter *m_pAudioCap;		// ��Ƶ�����˲���
    CComPtr<ISampleGrabber> m_pGrabber;		// ץȡͼƬ�˲���
    IBaseFilter *m_pMux;	// д�ļ��˲���
    ICaptureGraphBuilder2 *m_pCapGB;	// ��ǿ�Ͳ����˲����������
    IGraphBuilder *m_pGB;	// �˲����������
    IVideoWindow *m_pVW;	// ��Ƶ��ʾ���ڽӿ�
    IMediaControl *m_pMC;	// ý����ƽӿ�
    static bool m_bRecording;		// ¼����Ƶ��־
    float width;
    float height;
    IBaseFilter *m_pXviDCodec;   //mpeg4 �˲���
    GUID CAPTURE_MODE;
    GUID mediaSubtypes[VI_NUM_TYPES];
    GUID cur_media_type;
    //   HANDLE handlel;
};
#endif

