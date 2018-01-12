// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 REC_DLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// REC_DLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

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



