/****************************************************
*    Author: Tianhao Wang
*    Date:   2015-03-06   
*    Desc:   一个通用的d3d9类，模仿龙书第二版完成
*****************************************************/   
#include "d3d9App.h"

LPDIRECT3D9 g_pd3dObject = 0;
LPDIRECT3DDEVICE9 g_pd3dDevice = 0;
d3d9App* g_pd3d9App = 0;

// 回调函数，d3d9App类存在则调用成员函数，否则便调用默认的DefWindowProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (g_pd3d9App != 0)
        return g_pd3d9App->MsgProc(msg, wParam, lParam);
    else
        return DefWindowProc(hwnd, msg, wParam, lParam);
}

d3d9App::d3d9App(HINSTANCE hInstance, std::string winTitle, D3DDEVTYPE devType, DWORD vertexProc)
{
    m_devType = devType;
    m_vertexProc = vertexProc;
    m_appInst = hInstance;
    m_winTitle = winTitle;
    m_mainWnd = 0;
    ZeroMemory(&m_d3dPP, sizeof(m_d3dPP));

    InitWindow();
    InitD3D();
}

// 初始化D3D
void d3d9App::InitD3D()
{
    // 创建IDirect3D9对象
    if (FAILED(g_pd3dObject = Direct3DCreate9(D3D_SDK_VERSION))) {
        MessageBox(0, "Direct3DCreate9 FAILED", 0, 0);
        PostQuitMessage(0);
    }

    // 填写D3DPRESENT_PARAMETERS结构体
    m_d3dPP.BackBufferWidth        = 0;
    m_d3dPP.BackBufferHeight       = 0;
    m_d3dPP.BackBufferCount        = 1;
    m_d3dPP.BackBufferFormat       = D3DFMT_UNKNOWN;
    m_d3dPP.MultiSampleType        = D3DMULTISAMPLE_NONE;
    m_d3dPP.MultiSampleQuality     = 0;
    m_d3dPP.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    m_d3dPP.hDeviceWindow          = m_mainWnd;
    m_d3dPP.Windowed               = true;
    m_d3dPP.EnableAutoDepthStencil = true;
    m_d3dPP.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_d3dPP.Flags                  = 0;
    m_d3dPP.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

    // 创建IDirect3DDevice对象
    HR(g_pd3dObject->CreateDevice(D3DADAPTER_DEFAULT,
                                  m_devType,
                                  m_mainWnd,
                                  m_vertexProc,
                                  &m_d3dPP,
                                  &g_pd3dDevice));
}

// 消息处理
LRESULT d3d9App::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            DestroyWindow(m_mainWnd);
            return 0;
    }
    return DefWindowProc(m_mainWnd, msg, wParam, lParam);
}

// 删除创建的接口
d3d9App::~d3d9App()
{
    if (g_pd3dObject)
        g_pd3dObject->Release();
    if (g_pd3dDevice)
        g_pd3dDevice->Release();
}

// 初始化Windows窗口
void d3d9App::InitWindow()
{
    std::string winClass("MainWindows");    // 窗口类名
    WNDCLASS wc;
    wc.style         = 0;       // 窗口风格，此处为缺省类型
    wc.lpfnWndProc   = WinProc; // 选择该窗口的消息处理函数
    wc.cbClsExtra    = 0;       // 窗口类无扩展
    wc.cbWndExtra    = 0;       // 窗口实例无扩展
    wc.hInstance     = m_appInst;
    wc.hIcon         = LoadIcon(0, IDI_APPLICATION);          // 窗口的最小化图标为缺省
    wc.hCursor       = LoadCursor(0, IDC_ARROW);              // 采用箭头光标
    wc.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH)); // 窗口背景为白色
    wc.lpszMenuName  = 0;                                     // 窗口无菜单
    wc.lpszClassName = winClass.c_str();                      // 窗口类名

    // 注册窗口
    if (FAILED(RegisterClass(&wc))) {
        MessageBox(0, "RegisterClass FAILED", 0, 0);
        PostQuitMessage(0);
    }
    // 创建窗口实例
    m_mainWnd = CreateWindow(winClass.c_str(),    // 窗口类名
                             m_winTitle.c_str(),  // 窗口标题
                             WS_OVERLAPPEDWINDOW, // 窗口风格
                             100,                 // 左上角坐标
                             100,
                             800,                 // 高和宽
                             600,
                             0,                   // 无父窗口
                             0,                   // 无子菜单
                             m_appInst,           // 创建该窗口的程序句柄
                             0);                  // 不使用该值
    if (FAILED(m_mainWnd)) {
        MessageBox(0, "CreateWindow FAILED", 0, 0);
        PostQuitMessage(0);
    }

    ShowWindow(m_mainWnd, SW_SHOW); // 显示窗口
    UpdateWindow(m_mainWnd);        // 绘制用户区
}

// 消息循环，检查由输入设备传入的信息
int d3d9App::MsgLoop()
{
    MSG msg;
    msg.message = WM_NULL;

    __int64 cntPerSec = 0;
    QueryPerformanceFrequency((LARGE_INTEGER*)&cntPerSec);
    float secsPerCnt = 1.0f / (float)cntPerSec;

    __int64 timeNow = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg); // 转化消息对
            DispatchMessage(&msg);  // 将消息传递给回调函数
        } else {
            __int64 timeAfter = 0;
            QueryPerformanceCounter((LARGE_INTEGER*)&timeAfter);
            float dt = (timeAfter - timeNow) * secsPerCnt;

            UpdateScene(dt);
            DrawScene();

            timeNow = timeAfter;
        }
    }

    return (int)msg.wParam;
}