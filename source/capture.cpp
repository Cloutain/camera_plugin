/// Capture.cpp for class CCapture implement
//
///
////////////////////////////////////////
#include "capture.h"
#include <atlconv.h>
#include <fstream>
#include <iostream>
//#include "VideoChatDlg.h"
//#include "yuv2bmp.h"
#pragma warning(disable:4996)
#pragma warning(disable:4244) 
#ifndef srelease
#define srelease(x) if (NULL != x)\
{\
	x->Release();\
	x = NULL;\
}
#endif

#ifndef MAX_PATH
#define  MAX_PATH 1024
#endif
BOOL bOneShot = FALSE; // ȫ�ֱ���
capCallBackFunc fun;
#include <initguid.h>



DEFINE_GUID(MEDIASUBTYPE_Avi,
    0xe436eb88, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
DEFINE_GUID(MEDIASUBTYPE_GREY, 0x59455247, 0x0000, 0x0010, 0x80, 0x00,
    0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_Y8, 0x20203859, 0x0000, 0x0010, 0x80, 0x00,
    0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_Y800, 0x30303859, 0x0000, 0x0010, 0x80, 0x00,
    0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

DEFINE_GUID(CLSID_CaptureGraphBuilder2, 0xbf87b6e1, 0x8c27, 0x11d0, 0xb3, 0xf0, 0x00, 0xaa, 0x00, 0x37, 0x61, 0xc5);
DEFINE_GUID(CLSID_FilterGraph, 0xe436ebb3, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
DEFINE_GUID(CLSID_NullRenderer, 0xc1f400a4, 0x3f08, 0x11d3, 0x9f, 0x0b, 0x00, 0x60, 0x08, 0x03, 0x9e, 0x37);
DEFINE_GUID(CLSID_SampleGrabber, 0xc1f400a0, 0x3f08, 0x11d3, 0x9f, 0x0b, 0x00, 0x60, 0x08, 0x03, 0x9e, 0x37);
DEFINE_GUID(CLSID_SystemDeviceEnum, 0x62be5d10, 0x60eb, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);
DEFINE_GUID(CLSID_VideoInputDeviceCategory, 0x860bb310, 0x5d01, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);
DEFINE_GUID(FORMAT_VideoInfo, 0x05589f80, 0xc356, 0x11ce, 0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a);
DEFINE_GUID(IID_IAMAnalogVideoDecoder, 0xc6e13350, 0x30ac, 0x11d0, 0xa1, 0x8c, 0x00, 0xa0, 0xc9, 0x11, 0x89, 0x56);
DEFINE_GUID(IID_IAMCameraControl, 0xc6e13370, 0x30ac, 0x11d0, 0xa1, 0x8c, 0x00, 0xa0, 0xc9, 0x11, 0x89, 0x56);
DEFINE_GUID(IID_IAMCrossbar, 0xc6e13380, 0x30ac, 0x11d0, 0xa1, 0x8c, 0x00, 0xa0, 0xc9, 0x11, 0x89, 0x56);
DEFINE_GUID(IID_IAMStreamConfig, 0xc6e13340, 0x30ac, 0x11d0, 0xa1, 0x8c, 0x00, 0xa0, 0xc9, 0x11, 0x89, 0x56);
DEFINE_GUID(IID_IAMVideoProcAmp, 0xc6e13360, 0x30ac, 0x11d0, 0xa1, 0x8c, 0x00, 0xa0, 0xc9, 0x11, 0x89, 0x56);
DEFINE_GUID(IID_IBaseFilter, 0x56a86895, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
DEFINE_GUID(IID_ICaptureGraphBuilder2, 0x93e5a4e0, 0x2d50, 0x11d2, 0xab, 0xfa, 0x00, 0xa0, 0xc9, 0xc6, 0xe3, 0x8d);
DEFINE_GUID(IID_ICreateDevEnum, 0x29840822, 0x5b84, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);
DEFINE_GUID(IID_IGraphBuilder, 0x56a868a9, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
DEFINE_GUID(IID_IMPEG2PIDMap, 0xafb6c2a1, 0x2c41, 0x11d3, 0x8a, 0x60, 0x00, 0x00, 0xf8, 0x1e, 0x0e, 0x4a);
DEFINE_GUID(IID_IMediaControl, 0x56a868b1, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
DEFINE_GUID(IID_IMediaFilter, 0x56a86899, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
DEFINE_GUID(IID_ISampleGrabber, 0x6b652fff, 0x11fe, 0x4fce, 0x92, 0xad, 0x02, 0x66, 0xb5, 0xd7, 0xc7, 0x8f);
DEFINE_GUID(LOOK_UPSTREAM_ONLY, 0xac798be0, 0x98e3, 0x11d1, 0xb3, 0xf1, 0x00, 0xaa, 0x00, 0x37, 0x61, 0xc5);
DEFINE_GUID(MEDIASUBTYPE_AYUV, 0x56555941, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_IYUV, 0x56555949, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_RGB24, 0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
DEFINE_GUID(MEDIASUBTYPE_RGB32, 0xe436eb7e, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
DEFINE_GUID(MEDIASUBTYPE_RGB555, 0xe436eb7c, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
DEFINE_GUID(MEDIASUBTYPE_RGB565, 0xe436eb7b, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);
DEFINE_GUID(MEDIASUBTYPE_I420, 0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_UYVY, 0x59565955, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_Y211, 0x31313259, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_Y411, 0x31313459, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_Y41P, 0x50313459, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_YUY2, 0x32595559, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_YUYV, 0x56595559, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_YV12, 0x32315659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_YVU9, 0x39555659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_YVYU, 0x55595659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIASUBTYPE_MJPG, 0x47504A4D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); // MGB
DEFINE_GUID(MEDIATYPE_Interleaved, 0x73766169, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(MEDIATYPE_Video, 0x73646976, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(PIN_CATEGORY_CAPTURE, 0xfb6c4281, 0x0353, 0x11d1, 0x90, 0x5f, 0x00, 0x00, 0xc0, 0xcc, 0x16, 0xba);
DEFINE_GUID(PIN_CATEGORY_PREVIEW, 0xfb6c4282, 0x0353, 0x11d1, 0x90, 0x5f, 0x00, 0x00, 0xc0, 0xcc, 0x16, 0xba);
DEFINE_GUID(IID_IVideoWindow, 0x56a868b4, 0x0ad4, 0x11ce, 0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);


class CSampleGrabberCB : public ISampleGrabberCB
{
public:
    long lWidth;
    long lHeight;
    CCapture *pCap;
    char m_szFileName[MAX_PATH]; // λͼ�ļ�����
    CSampleGrabberCB() {
        strcpy(m_szFileName, ".\\sample.bmp");
    }
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv) {
        // if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) {//m1
        if (riid == IID_IUnknown) {
            *ppv = (void *) static_cast<ISampleGrabberCB*> (this);
            return NOERROR;
        }
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE SampleCB(
        double SampleTime,
        IMediaSample *pSample)
    {
        return 0;
    }

    HRESULT STDMETHODCALLTYPE BufferCB(
        double SampleTime,
        BYTE *pBuffer,
        LONG BufferLen)
    {
        if (!bOneShot)
            return 0;
        if (!pBuffer)
        {
            printf("Save Bmp File Failure!");
            return E_POINTER;
        }
        if (pBuffer != NULL && pCap)
        {
//             BYTE *pBuffer_temp = (BYTE*)malloc(sizeof(char)*BufferLen);
//             memset(pBuffer_temp, 0, BufferLen);
//             memcpy(pBuffer_temp, pBuffer, BufferLen);
            outBmpBuf(pBuffer, pCap);  // ��һ֡ͼ�����ݴ�����ʾ����                          //			((CVideoNetDlg *)pCap->m_dlgParent)->SendVideo(pBuffer, (int)lBufferSize);
        }
        return 0;
    }

    void outBmpBuf(BYTE *buf, CCapture* cap)
    {
        cap->calFunc(buf);
    }
};
CSampleGrabberCB samCB;
// interface ISampleGrabberCB : public IUnknown
// {
//     virtual HRESULT STDMETHODCALLTYPE SampleCB(
//         double SampleTime,
//         IMediaSample *pSample) = 0;
// 
//     virtual HRESULT STDMETHODCALLTYPE BufferCB(
//         double SampleTime,
//         BYTE *pBuffer,
//         LONG BufferLen) = 0;
// 
//     virtual ~ISampleGrabberCB() {}
// 
void CCapture::setSize(int w = 1280, int h=720) {
    samCB.lHeight = h;
    samCB.lWidth = w;
    samCB.pCap = (CCapture*)this;

    this->width = w;
    this->height = h;
}
void getMediaSubtypeAsString(GUID type, char * typeAsString) {

    char tmpStr[8];
    if (type == MEDIASUBTYPE_RGB24)     sprintf(tmpStr, "RGB24");
    else if (type == MEDIASUBTYPE_RGB32) sprintf(tmpStr, "RGB32");
    else if (type == MEDIASUBTYPE_RGB555)sprintf(tmpStr, "RGB555");
    else if (type == MEDIASUBTYPE_RGB565)sprintf(tmpStr, "RGB565");
    else if (type == MEDIASUBTYPE_YUY2)  sprintf(tmpStr, "YUY2");
    else if (type == MEDIASUBTYPE_YVYU)  sprintf(tmpStr, "YVYU");
    else if (type == MEDIASUBTYPE_YUYV)  sprintf(tmpStr, "YUYV");
    else if (type == MEDIASUBTYPE_IYUV)  sprintf(tmpStr, "IYUV");
    else if (type == MEDIASUBTYPE_UYVY)  sprintf(tmpStr, "UYVY");
    else if (type == MEDIASUBTYPE_YV12)  sprintf(tmpStr, "YV12");
    else if (type == MEDIASUBTYPE_YVU9)  sprintf(tmpStr, "YVU9");
    else if (type == MEDIASUBTYPE_Y411)  sprintf(tmpStr, "Y411");
    else if (type == MEDIASUBTYPE_Y41P)  sprintf(tmpStr, "Y41P");
    else if (type == MEDIASUBTYPE_Y211)  sprintf(tmpStr, "Y211");
    else if (type == MEDIASUBTYPE_AYUV)  sprintf(tmpStr, "AYUV");
    else if (type == MEDIASUBTYPE_MJPG)  sprintf(tmpStr, "MJPG");
    else if (type == MEDIASUBTYPE_Y800)  sprintf(tmpStr, "Y800");
    else if (type == MEDIASUBTYPE_Y8)    sprintf(tmpStr, "Y8");
    else if (type == MEDIASUBTYPE_GREY)  sprintf(tmpStr, "GREY");
    else if (type == MEDIASUBTYPE_I420)  sprintf(tmpStr, "I420");
    else sprintf(tmpStr, "OTHER");

    memcpy(typeAsString, tmpStr, sizeof(char) * 8);
}
static void MyFreeMediaType(AM_MEDIA_TYPE& mt) {
    if (mt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mt.pbFormat);
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL)
    {
        // Unecessary because pUnk should not be used, but safest.
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}


static void MyDeleteMediaType(AM_MEDIA_TYPE *pmt)
{
    if (pmt != NULL)
    {
        MyFreeMediaType(*pmt);
        CoTaskMemFree(pmt);
    }
}


static bool setSizeAndSubtype(IAMStreamConfig* pConfig, AM_MEDIA_TYPE* mediaType, int attemptWidth, int attemptHeight, GUID mediatype) {
    VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);

    //store current size
    //int tmpWidth  = HEADER(pVih)->biWidth;
    //int tmpHeight = HEADER(pVih)->biHeight;
    AM_MEDIA_TYPE * tmpType = NULL;

    HRESULT    hr = pConfig->GetFormat(&tmpType);
    if (hr != S_OK)return false;

    //set new size:
    //width and height
    HEADER(pVih)->biWidth = attemptWidth;
    HEADER(pVih)->biHeight = attemptHeight;

    mediaType->formattype = FORMAT_VideoInfo;
    mediaType->majortype = MEDIATYPE_Video;
    mediaType->subtype = mediatype;

    //buffer size
    if (mediatype == MEDIASUBTYPE_RGB24)
    {
        mediaType->lSampleSize = attemptWidth*attemptHeight * 3;
    }
    else if (mediatype == MEDIASUBTYPE_RGB32)
    {
        mediaType->lSampleSize = attemptWidth*attemptHeight * 4;
    }
    else if (mediatype == MEDIASUBTYPE_YUY2)
    {
        mediaType->lSampleSize = attemptWidth*attemptHeight * 2;
    }
    else
    {
        // For compressed data, the value can be zero.
        mediaType->lSampleSize = 0;
    }

    //set fps if requested
    if (1) {
        pVih->AvgTimePerFrame = 299;
    }
    //okay lets try new size
    hr = pConfig->SetFormat(mediaType);
    if (hr == S_OK) {
        if (tmpType != NULL)MyDeleteMediaType(tmpType);
        return true;
    }
    else {
        pConfig->SetFormat(tmpType);
        if (tmpType != NULL)MyDeleteMediaType(tmpType);
    }
    return false;
}

/////////////////////////////////////////////
/// for class CCapture's Function
///
////////////////////////////////////////////////

CCapture::CCapture()
{
    CoInitialize(NULL);  // ��ʼ��COM��
    m_hWnd = NULL;
    m_pVideoCap = NULL;
    m_pAudioCap = NULL;
    m_pCapGB = NULL;
    m_pGB = NULL;
    width = IMG_WIDTH;
    height = IMG_HEIGHT;
    m_pMC = NULL;
    m_pMux = NULL;
    m_pVW = NULL;
    m_pGrabber = NULL;
    mediaSubtypes[0] = MEDIASUBTYPE_RGB24;
    mediaSubtypes[1] = MEDIASUBTYPE_RGB32;
    mediaSubtypes[2] = MEDIASUBTYPE_RGB555;
    mediaSubtypes[3] = MEDIASUBTYPE_RGB565;
    mediaSubtypes[4] = MEDIASUBTYPE_YUY2;
    mediaSubtypes[5] = MEDIASUBTYPE_YVYU;
    mediaSubtypes[6] = MEDIASUBTYPE_YUYV;
    mediaSubtypes[7] = MEDIASUBTYPE_IYUV;
    mediaSubtypes[8] = MEDIASUBTYPE_UYVY;
    mediaSubtypes[9] = MEDIASUBTYPE_YV12;
    mediaSubtypes[10] = MEDIASUBTYPE_YVU9;
    mediaSubtypes[11] = MEDIASUBTYPE_Y411;
    mediaSubtypes[12] = MEDIASUBTYPE_Y41P;
    mediaSubtypes[13] = MEDIASUBTYPE_Y211;
    mediaSubtypes[14] = MEDIASUBTYPE_AYUV;
    mediaSubtypes[15] = MEDIASUBTYPE_MJPG; // MGB

                                           //non standard
    mediaSubtypes[16] = MEDIASUBTYPE_Y800;
    mediaSubtypes[17] = MEDIASUBTYPE_Y8;
    mediaSubtypes[18] = MEDIASUBTYPE_GREY;
    mediaSubtypes[19] = MEDIASUBTYPE_I420;
    //  m_dlgParent = NULL;  m1
}
bool CCapture::m_bRecording = false;

CCapture::~CCapture()
{
    if (m_pMC) m_pMC->Stop();
    if (m_pVW)
    {
        m_pVW->put_Owner(NULL);
        m_pVW->put_Visible(OAFALSE);
    }
    m_hWnd = NULL;
    srelease(m_pVideoCap);
    srelease(m_pGB);
    srelease(m_pCapGB);
    srelease(m_pMC);
    srelease(m_pVW);
    m_bRecording = false;
    CoUninitialize(); // �ͷ�COM��
}
int CCapture::GetPreWidth()
{
    return this->width;
}
int CCapture::GetPreHeight()
{
    return this->height;
}

void CCapture::SetPreWidth(int width)
{
    this->width = width;
}

void CCapture::SetPreHeight(int height)
{
    this->height = height;
}

int CCapture::EnumDevice(HWND hCmbList, DeviceType deviceType)
{
    if (hCmbList == NULL) return -1;
    int id = 0;
    /////ö�ٲ����豸
    ICreateDevEnum *pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_ICreateDevEnum,
        (LPVOID *)&pCreateDevEnum);
    if (hr != NOERROR) return -1;
    //////// ��ȡ��Ƶ���ö����
    IEnumMoniker *pEm;			 //ö�ټ�����ӿ�
                                 //  if (deviceType == DTypeVideo)
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
    // �����ȡ��Ƶ���ö���� ������Ĵ���

    //     else
    //     {
    //      //   hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEm, 0);
    //     }mod 

    //	if (hr != NOERROR) return -1;
    if (!pEm || FAILED(hr)) return -1;
    ////////////////////////
    pEm->Reset();   // ����ö������λ
    ULONG cFetched;
    IMoniker *pM;    // ������ӿ�ָ��
    while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
    {
        IPropertyBag *pBag;   // ����ҳ�ӿ�ָ��
        hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        // ��ȡ�豸����ҳ
        if (SUCCEEDED(hr))
        {
            VARIANT var;
            var.vt = VT_BSTR;		// ������Ƕ���������
                                    // ��ȡfirendlyName ��ʽ����Ϣ
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if (hr == NOERROR) // ��ȡ�ɹ�
            {
                id++;
                char szDeviceName[256] = { 0 };
                WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, szDeviceName, 80, NULL, NULL);  // �ַ�������ת��UNICODE TO ANSI
                ::SendMessage(hCmbList, CB_ADDSTRING, 0, (LPARAM)szDeviceName);//��ӵ�����б��
                SysFreeString(var.bstrVal);  //�ͷ���Դ���ر�Ҫע��
            }
            pBag->Release();
        }
        pM->Release();
    }
    return 0;
}

void CCapture::ResizeVideoWindow()	// ������Ƶ��ʾ����
{
    return;
}

HRESULT CCapture::SetupVideoWindow()
{
    HRESULT hr;
    //m_hWndΪ��CCapture�ĳ�Ա��������ʹ�øú���ǰ���ʼ��
    hr = m_pVW->put_Visible(OAFALSE);  // ��Ƶ���ڲ��ɼ�
    hr = m_pVW->put_Owner((OAHWND)m_hWnd);  // ������Ƶ����
    if (FAILED(hr)) return hr;
    hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN); //���ô�������
    if (FAILED(hr)) return hr;
    ResizeVideoWindow();   // ���Ĵ��ڴ�С
    hr = m_pVW->put_Visible(OATRUE);  // ��ʾ��Ƶ����
    return hr;
}

HRESULT CCapture::InitCaptureGraphBuilder()
{
    HRESULT hr;
    //����IGraphBuilder�ӿ�(�˲������������) m_pGB
    hr = CoCreateInstance(CLSID_FilterGraph, NULL,
        CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGB);
    if (FAILED(hr)) return hr;
    //����ICaptureGraphBuilder2�ӿڣ���ǿ�Ͳ����˲��������������m_pCapGB
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
        CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&m_pCapGB);
    if (FAILED(hr)) return hr;
    // ����ץȡͼƬ�˲���
    if (m_pGrabber) {
        m_pGrabber.Release();
        m_pGrabber = NULL;
    }
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void **)&m_pGrabber);
    //	hr = m_pGrabber.CoCreateInstance( CLSID_SampleGrabber ); 
    if (FAILED(hr)) return hr;
    // ��ʼ���˲������������IGraphBuilder
    m_pCapGB->SetFiltergraph(m_pGB);
    // ��ѯý����ƽӿ�
    hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
    if (FAILED(hr))  return hr;
    // ��ѯ��Ƶ���ڽӿ�
    hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *)&m_pVW);
    if (FAILED(hr)) return hr;
    /////
    return hr;
}
static void findClosestSizeAndSubtype(IAMStreamConfig* pConfig, int widthIn, int heightIn, int &widthOut, int &heightOut, GUID & mediatypeOut)
{
    HRESULT hr;

    //find perfect match or closest size
    int nearW = 9999999;
    int nearH = 9999999;
    //bool foundClosestMatch     = true;

    int iCount = 0;
    int iSize = 0;
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);

    if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
    {
        //For each format type RGB24 YUV2 etc
        for (int iFormat = 0; iFormat < iCount; iFormat++)
        {
            VIDEO_STREAM_CONFIG_CAPS scc;
            AM_MEDIA_TYPE *pmtConfig;
            hr = pConfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);

            if (SUCCEEDED(hr)) {

                //his is how many diff sizes are available for the format
                int stepX = scc.OutputGranularityX;
                int stepY = scc.OutputGranularityY;

                int tempW = 999999;
                int tempH = 999999;

                //Don't want to get stuck in a loop
                if (stepX < 1 || stepY < 1) continue;

                //if(verbose)printf("min is %i %i max is %i %i - res is %i %i \n", scc.MinOutputSize.cx, scc.MinOutputSize.cy,  scc.MaxOutputSize.cx,  scc.MaxOutputSize.cy, stepX, stepY);
                //if(verbose)printf("min frame duration is %i  max duration is %i\n", scc.MinFrameInterval, scc.MaxFrameInterval);

                bool exactMatch = false;
                bool exactMatchX = false;
                bool exactMatchY = false;

                for (int x = scc.MinOutputSize.cx; x <= scc.MaxOutputSize.cx; x += stepX) {
                    //If we find an exact match
                    if (widthIn == x) {
                        exactMatchX = true;
                        tempW = x;
                    }
                    //Otherwise lets find the closest match based on width
                    else if (abs(widthIn - x) < abs(widthIn - tempW)) {
                        tempW = x;
                    }
                }

                for (int y = scc.MinOutputSize.cy; y <= scc.MaxOutputSize.cy; y += stepY) {
                    //If we find an exact match
                    if (heightIn == y) {
                        exactMatchY = true;
                        tempH = y;
                    }
                    //Otherwise lets find the closest match based on height
                    else if (abs(heightIn - y) < abs(heightIn - tempH)) {
                        tempH = y;
                    }
                }

                //see if we have an exact match!
                if (exactMatchX && exactMatchY) {
                    //foundClosestMatch = false;
                    exactMatch = true;

                    widthOut = widthIn;
                    heightOut = heightIn;
                    mediatypeOut = pmtConfig->subtype;
                }

                //otherwise lets see if this filters closest size is the closest
                //available. the closest size is determined by the sum difference
                //of the widths and heights
                else if (abs(widthIn - tempW) + abs(heightIn - tempH) < abs(widthIn - nearW) + abs(heightIn - nearH))
                {
                    nearW = tempW;
                    nearH = tempH;

                    widthOut = nearW;
                    heightOut = nearH;
                    mediatypeOut = pmtConfig->subtype;
                }

                MyDeleteMediaType(pmtConfig);

                //If we have found an exact match no need to search anymore
                if (exactMatch)break;
            }
        }
    }

}


HRESULT CCapture::Preview(int iDevVideoID, int iDevAudioID /*= 0*/, HWND hAudio /*= NULL*/)
{
    HRESULT hr;
    if (m_pMC)
        m_pMC->Stop();
    m_bRecording = false;
    // ��ʼ����Ƶ�����˲������������
    hr = InitCaptureGraphBuilder();
    if (FAILED(hr)) return hr;

    // ��ָ������Ƶ�ɼ��豸���˲�������
    if (BindFilter(iDevVideoID, &m_pVideoCap, DTypeVideo))
    {
        // ���˲�����ӵ��˲���������
        hr = m_pGB->AddFilter(m_pVideoCap, L"Video Capture Filter");
        if (FAILED(hr)) return hr;
    }
    else return FALSE;

    if (BindFilter(iDevAudioID, &m_pAudioCap, DTypeAudio))
    {
        hr = m_pGB->AddFilter(m_pAudioCap, L"Audio Capture Filter");
        if (FAILED(hr))
        {
            printf("����Ƶ�豸ʧ��!");
            //	return hr;
        }
    }
    else
    {
        printf("����Ƶ�豸ʧ��!");
        //	return FALSE;
    }
    // ���������ץȡ24λ��RGBͼƬ����������ý��ͼƬ����
  
    CComQIPtr<IBaseFilter, &IID_IBaseFilter> pGrabBase(m_pGrabber);

    //   AM_MEDIA_TYPE mediaType;
    AM_MEDIA_TYPE* mediaType;

    IAMStreamConfig* pConfig = NULL;
    hr = m_pCapGB->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pVideoCap, IID_IAMStreamConfig, (void**)&pConfig);
    hr = pConfig->GetFormat(&mediaType);
    if (!FAILED(hr))
    {
        //NOW LETS DEAL WITH GETTING THE RIGHT SIZE

        //     hr = pConfig->SetFormat(mediaType);
        if (FAILED(hr)) {
            //             if (verbose)printf("ERROR: Couldn't getFormat for pAmMediaType!\n");
            //             stopDevice(deviceID);
            return hr;
        }
        //   CAPTURE_MODE = PIN_CATEGORY_CAPTURE;
    }

    VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat);
    int currentWidth = HEADER(pVih)->biWidth;
    int currentHeight = HEADER(pVih)->biHeight;
    bool foundSize = false;
    char guidStr[8];
    // try specified format and size
    getMediaSubtypeAsString(MEDIASUBTYPE_RGB32,guidStr);
    if (setSizeAndSubtype(pConfig, mediaType, width, height, MEDIASUBTYPE_RGB32)) {
        setSize(width, height);
        cur_media_type = MEDIASUBTYPE_RGB32;
        foundSize = true;
    }
    else {
        // try specified size with all formats
        for (int i = 0; i < VI_NUM_TYPES; i++) {

            getMediaSubtypeAsString(mediaSubtypes[i], guidStr);

            // if (verbose)printf("SETUP: trying format %s @ %i by %i\n", guidStr, VD->tryWidth, VD->tryHeight);
            if (setSizeAndSubtype(pConfig, mediaType, width, height, mediaSubtypes[i])) {
                setSize(width, height);
                this->cur_media_type = mediaSubtypes[i];
                foundSize = true;
                break;
            }
        }
    }
    //if we didn't find the requested size - lets try and find the closest matching size
    if (foundSize == false) {
        int closestWidth = -1;
        int closestHeight = -1;
        GUID newMediaSubtype;

        findClosestSizeAndSubtype(pConfig, width, height, closestWidth, closestHeight, newMediaSubtype);

        if (closestWidth != -1 && closestHeight != -1) {
            getMediaSubtypeAsString(newMediaSubtype, guidStr);

            printf("SETUP: closest supported size is %s @ %i %i\n", guidStr, closestWidth, closestHeight);
            if (setSizeAndSubtype(pConfig, mediaType, closestWidth, closestHeight, newMediaSubtype)) {
                setSize(closestWidth, closestHeight);
                foundSize = true;
            }
        }
    }


    //if we didn't specify a custom size or if we did but couldn't find it lets setup with the default settings
    // if( foundSize == false) {
    //     if (VD->requestedFrameTime != -1) {
    //         pVih->AvgTimePerFrame = VD->requestedFrameTime;
    //         hr = VD->streamConf->SetFormat(VD->pAmMediaType);
    //     }
    //     VD->setSize(currentWidth, currentHeight);
    // }
    // ������Ƶ��ʽ

    AM_MEDIA_TYPE mt;
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));

    mt.majortype = MEDIATYPE_Video;
    // mt.subtype = MEDIASUBTYPE_I420;
#if 0
    mt.subtype = MEDIASUBTYPE_RGB32;
#else
    mt.subtype = MEDIASUBTYPE_YUY2;
#endif

    mt.formattype = FORMAT_VideoInfo;


    hr = m_pGrabber->SetMediaType(&mt);
    if (FAILED(hr)) {
        printf("Fail to set media type!");
        return hr;
    }
    hr = m_pGB->AddFilter(pGrabBase, L"SampleGrabber");
    if (FAILED(hr)) return hr;
    // ��Ⱦý�壬 ���������˲�����������
    //   hr = m_pCapGB->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Audio, m_pAudioCap, NULL, NULL);
    hr = m_pCapGB->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pVideoCap, pGrabBase, NULL);
    if (FAILED(hr))
        hr = m_pCapGB->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pVideoCap, pGrabBase, NULL);
    if (FAILED(hr)) {
        printf("Can��t build the graph");
        return hr;
    }

    ////////// ����ͼƬ��������
    hr = m_pGrabber->GetConnectedMediaType(mediaType);
    if (FAILED(hr)) {
        printf("Failt to read the connected media type");
        return hr;
    }
    //    VIDEOINFOHEADER * pVih = (VIDEOINFOHEADER*)mediaType->pbFormat;
    samCB.lWidth = HEADER(pVih)->biWidth;;
    samCB.lHeight = HEADER(pVih)->biHeight;
    samCB.pCap = (CCapture *)this;
    FreeMediaType(*mediaType);
    hr = m_pGrabber->SetBufferSamples(TRUE);  // ����˴�Ϊfalse ��һ��ץȡͼƬʱʧ��(���ûص���ʽ)
    hr = m_pGrabber->SetOneShot(FALSE);
    hr = m_pGrabber->SetCallback(&samCB, 1);
    SetOnShot(TRUE);// ture ʱ��ʼ������Ƶ֡����
                    // ������Ƶ��ʾ����
                    // m_hWnd = hVideo;
                    //    SetupVideoWindow(); // ������ʾ����
    hr = m_pMC->Run();  // ��ʼ�ɼ���Ԥ����Ƶ������ָ��������ʾ
    if (FAILED(hr))
    {
        printf("������豸�Ƿ�ռ��!");
        return hr;
    }
    return S_OK;
}

#if 1  // avi video format
HRESULT CCapture::CaptureVideo(string inFileName)  // ¼����Ƶ
{
    HRESULT hr = 0;
    m_bRecording = false;
    m_pMC->Stop();   // ��ֹͣ��Ƶ�ɼ�

                     // �����ļ�����ע��ڶ�����������
    LPWSTR file = CA2W(inFileName.c_str());
    //    hr = m_pCapGB->SetOutputFileName(&MEDIASUBTYPE_Avi, file.AllocSysString(), &m_pMux, NULL);

    //��Ⱦý�� ���Ӳ�������AVI Muxer������ 
    hr = m_pCapGB->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pVideoCap, NULL, m_pMux);
    //    hr = m_pCapGB->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, m_pAudioCap, NULL, m_pMux);mod
    //������Ƶ��Ϊ���� 
    //   IConfigAviMux *pConfigMux; mod
    //    m_pMux->QueryInterface(IID_IConfigAviMux, (void **)&pConfigMux); mod
    // hr = pConfigMux->SetMasterStream(0);   // 0 Ϊ��Ƶ  1Ϊ��Ƶ

    //   pConfigMux->Release();
    m_pMux->Release();
    m_bRecording = true;
    //     hThread = CreateThread(NULL, 0,
    //         (LPTHREAD_START_ROUTINE)ThreadFunDrawText,
    //         (LPVOID)m_hWnd,
    //         0, &dwId);
    m_pMC->Run();  // �ָ���Ƶ�ɼ���ͬʱд���ļ�

    return hr;
}
#else  // mpeg4 format video
HRESULT CCapture::CaptureVideo(CString inFileName)
{
    HRESULT hr = 0;

    m_pMC->Stop();

    m_pGB->AddFilter(m_pXviDCodec, L"99 Xvid MPEG-4 Codec");
    m_pXviDCodec->Release();

    hr = m_pCapGB->SetOutputFileName(&MEDIASUBTYPE_Avi, inFileName.AllocSysString(), &m_pMux, NULL);
    hr = ConnectFilters(m_pGB, m_pSmartTee_1, m_pXviDCodec, 0);    //0������capture����
    hr = ConnectFilters(m_pGB, m_pXviDCodec, m_pMux, 2);    //2��Ĭ����Ȼ����
    m_pMux->Release();

    m_pMC->Run();

    return hr;
}
#endif

///////////////////////////////

HRESULT CCapture::CaptureImage() // ץȡ����ʾͼ��
{ // ����CB�ӿڻص������洢ͼƬ
    bOneShot = TRUE;
    return 0;
}

HRESULT CCapture::CaptureImage(string inFileName) // ץȡͼ��
{
    HRESULT hr;
    AM_MEDIA_TYPE mediaType;
    hr = m_pGrabber->GetConnectedMediaType(&mediaType);
    if (FAILED(hr))  return hr;
    VIDEOINFOHEADER *pVih;
    if (mediaType.formattype == FORMAT_VideoInfo &&
        (mediaType.cbFormat >= sizeof(VIDEOINFOHEADER)) &&
        mediaType.pbFormat != NULL)
    {
        pVih = (VIDEOINFOHEADER *)mediaType.pbFormat;
    }
    else
        return VFW_E_INVALIDMEDIATYPE;

    //	hr = m_pGrabber->SetOneShot(TRUE);
    if (SUCCEEDED(m_pGrabber->SetBufferSamples(TRUE)))  // ����Ϊ������ʽ)
    {
        long cbBuffer = 0;
        hr = m_pGrabber->GetCurrentBuffer(&cbBuffer, NULL);
        BYTE *pBuffer = new BYTE[cbBuffer];
        if (!pBuffer) return -1;
        // ��ȡһ֡ý�������
        hr = m_pGrabber->GetCurrentBuffer(&cbBuffer, (long *)pBuffer);
        if (FAILED(hr))  return hr;
        //  		if (pBuffer != NULL)
        //  		{
        //  			calFunc(pBuffer);   // ��һ֡ͼ�����ݴ�����ʾ����
        // 		}

        ///-------------------------��������������rgb��ʽ����yuv��ʽ--------
        long n1, n2;
        int datalen = width*height * 3;
        BYTE *rgb = new BYTE[datalen];

        //      YUV422_C_RGB(pBuffer, rgb, IMG_HEIGHT, IMG_WIDTH);
        n1 = (long)strlen((char *)pBuffer);
        n2 = (long)strlen((char *)rgb);
        //		((CVideoNetDlg *)(m_dlgParent))->SendVideo((BYTE *)pBuffer, (int)cbBuffer);
        ///------------------------------------------------------------------
        ///////////////////////////////////////////////////////////////
        // Create a file to hold the bitmap
        LPWSTR file_name = CA2W(inFileName.c_str());
        //        HANDLE hf = CreateFile(file_name, GENERIC_WRITE, FILE_SHARE_READ,
        //            NULL, CREATE_ALWAYS, NULL, NULL);

        //         if (hf == INVALID_HANDLE_VALUE) {
        //             MessageBox(NULL, _T("Create bmp file failure!"), _T(""), MB_OK | MB_ICONINFORMATION);
        //             return 0;
        //         }

        // Write out the file header
        //
        // ��Ϣͷ
        BITMAPFILEHEADER bfh;
        memset(&bfh, 0, sizeof(bfh));
        bfh.bfType = 'MB';
        bfh.bfSize = sizeof(bfh) + cbBuffer + sizeof(BITMAPINFOHEADER);
        bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

        DWORD Written = 0;
        //        WriteFile(hf, &bfh, sizeof(bfh), &Written, NULL);

        // Write the bitmap format
        //�ļ�ͷ
        BITMAPINFOHEADER bih;
        memset(&bih, 0, sizeof(bih));
        bih.biSize = sizeof(bih);
        bih.biWidth = pVih->bmiHeader.biWidth;
        bih.biHeight = pVih->bmiHeader.biHeight;
        bih.biPlanes = 1;
        bih.biBitCount = 24;

        Written = 0;
        //       WriteFile(hf, &bih, sizeof(bih), &Written, NULL);

        // Write the bitmap bits
        //
        Written = 0;
        //        WriteFile(hf, rgb, datalen, &Written, NULL);
        //       CloseHandle(hf);
        delete pBuffer;
        //         MessageBox(NULL, _T("Save photo succeeded!"), _T("ץȡͼƬ��ʾ"), MB_OK | MB_ICONINFORMATION);
    }
    m_pGrabber->SetOneShot(FALSE);
    m_pGrabber->SetBufferSamples(FALSE);
    FreeMediaType(mediaType);
    return 0;
}

bool CCapture::BindFilter(int iDeviceID, IBaseFilter **pOutFilter, DeviceType deviceType)
{
    if (iDeviceID < 0) return false;
    // ö�����е���Ƶ�豸
    ICreateDevEnum *pCreateDevEnum;
    //�����豸ö����pCreateDevEnum
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,
        NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pCreateDevEnum);
    if (hr != NOERROR) return false;
    IEnumMoniker *pEM;
    // ������Ƶ�����豸��ö����

    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEM, 0);
    // ��Ƶ�豸ö����

    //  hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEM, 0);mod
    if (hr != NOERROR) return false;
    pEM->Reset();  // ��λ���豸
    ULONG cFetched;
    IMoniker *pM;
    int indexDev = 0;
    // ��ȡ�豸
    while (hr = pEM->Next(1, &pM, &cFetched), hr == S_OK, indexDev <= iDeviceID)
    {
        IPropertyBag *pBag;
        // ��ȡ���豸���Լ�
        hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if (SUCCEEDED(hr))
        {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if (hr == NOERROR)
            {
                // �ɼ��豸�벶���˲�������
                if (indexDev == iDeviceID) pM->BindToObject(0, 0, IID_IBaseFilter, (void **)pOutFilter);
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        }
        pM->Release();
        indexDev++;
    }
    return true;
}

void CCapture::SetCameraFormat(HWND hwndParent) // ������Ƶ��ʽ
{
    HRESULT hr;
    IAMStreamConfig *pSC; // �����ýӿ�
    ISpecifyPropertyPages *pSpec; //����ҳ�ӿ�
    m_pMC->Stop();  // ֻ��ֹͣ����ܽ����������Ե�����
    m_bRecording = false;
    // ���Ȳ�ѯ����CAPTURE����ƵVideo�ӿ�
    hr = m_pCapGB->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
        m_pVideoCap, IID_IAMStreamConfig, (void **)&pSC);

    CAUUID cauuid; // ��������ҳ�ṹ��
    hr = pSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
    if (hr == S_OK)
    {
        // ��ʾ����ҳ����
        hr = pSpec->GetPages(&cauuid);  // ��ȡ��������ҳ
        hr = OleCreatePropertyFrame(hwndParent, 30, 30, NULL, 1,
            (IUnknown **)&pSC, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);
        // �ͷ��ڴ���Դ
        CoTaskMemFree(cauuid.pElems);
        pSpec->Release();
        pSC->Release();
    }
    // �ָ�����
    m_pMC->Run();
}

void CCapture::SetCameraFilter(HWND hwndParent) // ����ͼ�����������
{
    HRESULT hr = 0;
    ISpecifyPropertyPages *pSpec;
    hr = m_pVideoCap->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);
    if (SUCCEEDED(hr))
    {
        // ��ȡ�˲������ƺ�IUnknown �ӿ�ָ��
        FILTER_INFO FilterInfo;
        hr = m_pVideoCap->QueryFilterInfo(&FilterInfo);
        IUnknown *pFilterUnk;
        m_pVideoCap->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);
        // ��ʾ��ҳ
        CAUUID caGUID;
        pSpec->GetPages(&caGUID);
        OleCreatePropertyFrame(hwndParent,
            0, 0,
            FilterInfo.achName,
            1,
            &pFilterUnk,
            caGUID.cElems,
            caGUID.pElems,
            0,
            0, NULL);
        // �ͷ��ڴ���Դ
        CoTaskMemFree(caGUID.pElems);
        pFilterUnk->Release();
        FilterInfo.pGraph->Release();
        pSpec->Release();
    }
}

void CCapture::StopCapture()
{
    m_pMC->Stop();
}

void CCapture::FreeMediaType(AM_MEDIA_TYPE &mt)
{
    if (mt.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)mt.pbFormat);
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL)
    {
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

void CCapture::SetOnShot(BOOL bFlag)
{
    bOneShot = bFlag;
}

void CCapture::SetCallBKFun(capCallBackFunc f)
{
    this->calFunc = f;
    samCB.pCap = static_cast<CCapture *>(this);
}


