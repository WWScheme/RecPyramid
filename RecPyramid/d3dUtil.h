/**************************************************
*
*  一些实用函数，基本照搬FRANK D.LUNA的龙书第二版
*
***************************************************/
#ifndef _D3DUTIL_H
#define _D3DUTIL_H

#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>
#include <dinput.h>
#include <string>

class d3d9App;
extern d3d9App* g_pd3d9App;
extern LPDIRECT3DDEVICE9 g_pd3dDevice;

#define ReleaseCOM(x) { if (x) { x->Release(); x = 0; } }

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
    #define HR(x)                                         \
    {                                                     \
        HRESULT hr = x;                                   \
        if (FAILED(hr)) {                                 \
            DXTrace(__FILE__, __LINE__, hr, #x, TRUE);    \
        }                                                 \
    }
#endif

#else
    #ifndef HR
    #define HR(x) x;
    #endif
#endif

#endif