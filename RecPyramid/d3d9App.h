/****************************************************
*    Author: Tianhao Wang
*    Date:   2015-03-06   
*    Desc:   一个通用的d3d9类，模仿龙书第二版完成
*****************************************************/   
#ifndef _D3D9APP_H
#define _D3D9APP_H

#include "d3dUtil.h"

class d3d9App {
public:
    d3d9App(HINSTANCE hInstance, std::string winTitle, D3DDEVTYPE devType, DWORD vertexProc);
    virtual ~d3d9App();

    // Direct3D的初始化
    virtual void InitD3D();
    // Windows主窗口的初始化
    virtual void InitWindow();
    // 消息处理
    virtual LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);
    // 消息流的查看及处理
    virtual int MsgLoop();
    HINSTANCE GetAppInst() { return m_appInst; }
    HWND GetMainWnd() { return m_mainWnd; }

    // 以下必须在继承时自行定义
    virtual void DrawScene()            { }
    virtual void UpdateScene(float dt)    { }
protected:
    D3DDEVTYPE m_devType;          // 驱动类型
    DWORD m_vertexProc;            // 定点缓存
    HINSTANCE m_appInst;           // 程序句柄的拷贝
    HWND m_mainWnd;                // 窗口句柄
    D3DPRESENT_PARAMETERS m_d3dPP;
    std::string m_winTitle;        // 程序标题
};

extern LPDIRECT3D9 g_pd3dObject;
extern LPDIRECT3DDEVICE9 g_pd3dDevice;
extern d3d9App* g_pd3d9App;

#endif