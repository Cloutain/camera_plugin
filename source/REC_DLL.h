// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� REC_DLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// REC_DLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

// #pragma once
// 
// #include <windows.h>
// 
// #ifdef REC_DLL_EXPORTS
// #define REC_DLL_API __declspec(dllexport)
// #else
// #define REC_DLL_API __declspec(dllimport)
// #endif
// 
// typedef int(__cdecl  *tp_video_rec_callback)(char* pVideo1, char* pVideo2, char* pVideo3, INT32 width, INT32 height, float* arr, INT32 * arr_len);
// 
// 
// REC_DLL_API int __cdecl  REC_Init(int camera_index, tp_video_rec_callback rec_callback);
// REC_DLL_API int __cdecl  REC_Close();
// REC_DLL_API int __cdecl  REC_SetPreview(int preview);



