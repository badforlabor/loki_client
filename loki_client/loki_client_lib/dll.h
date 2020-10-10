/**,
 * Auth :   liubo
 * Date :   2020/10/10 15:21
 * Comment: dll导出  
 */

#pragma once

#if __cplusplus
#define EXTERN_C extern "C"
#else
#define	EXTERN_C
#endif

 //#define DllImport extern "C"  __declspec( dllimport )
 //#define DllExport extern "C"  __declspec( dllexport )

#ifndef LOKICPP_DLL_API
#ifdef MY_STATIC_LIB
#define LOKICPP_DLL_API EXTERN_C
#else
#ifdef LOKICPP_DLL_EXPORT
#define LOKICPP_DLL_API EXTERN_C __declspec( dllexport )
#else
#define LOKICPP_DLL_API EXTERN_C __declspec( dllimport )
#endif
#endif
#endif