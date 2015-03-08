/****************************************************
*    Author: Tianhao Wang
*    Date:   2015-03-06   
*    Desc:   һ��ͨ�õ�d3d9�࣬ģ������ڶ������
*****************************************************/   
#include "d3d9App.h"

LPDIRECT3D9 g_pd3dObject = 0;
LPDIRECT3DDEVICE9 g_pd3dDevice = 0;
d3d9App* g_pd3d9App = 0;

// �ص�������d3d9App���������ó�Ա��������������Ĭ�ϵ�DefWindowProc
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

// ��ʼ��D3D
void d3d9App::InitD3D()
{
    // ����IDirect3D9����
    if (FAILED(g_pd3dObject = Direct3DCreate9(D3D_SDK_VERSION))) {
        MessageBox(0, "Direct3DCreate9 FAILED", 0, 0);
        PostQuitMessage(0);
    }

    // ��дD3DPRESENT_PARAMETERS�ṹ��
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

    // ����IDirect3DDevice����
    HR(g_pd3dObject->CreateDevice(D3DADAPTER_DEFAULT,
                                  m_devType,
                                  m_mainWnd,
                                  m_vertexProc,
                                  &m_d3dPP,
                                  &g_pd3dDevice));
}

// ��Ϣ����
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

// ɾ�������Ľӿ�
d3d9App::~d3d9App()
{
    if (g_pd3dObject)
        g_pd3dObject->Release();
    if (g_pd3dDevice)
        g_pd3dDevice->Release();
}

// ��ʼ��Windows����
void d3d9App::InitWindow()
{
    std::string winClass("MainWindows");    // ��������
    WNDCLASS wc;
    wc.style         = 0;       // ���ڷ�񣬴˴�Ϊȱʡ����
    wc.lpfnWndProc   = WinProc; // ѡ��ô��ڵ���Ϣ������
    wc.cbClsExtra    = 0;       // ����������չ
    wc.cbWndExtra    = 0;       // ����ʵ������չ
    wc.hInstance     = m_appInst;
    wc.hIcon         = LoadIcon(0, IDI_APPLICATION);          // ���ڵ���С��ͼ��Ϊȱʡ
    wc.hCursor       = LoadCursor(0, IDC_ARROW);              // ���ü�ͷ���
    wc.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH)); // ���ڱ���Ϊ��ɫ
    wc.lpszMenuName  = 0;                                     // �����޲˵�
    wc.lpszClassName = winClass.c_str();                      // ��������

    // ע�ᴰ��
    if (FAILED(RegisterClass(&wc))) {
        MessageBox(0, "RegisterClass FAILED", 0, 0);
        PostQuitMessage(0);
    }
    // ��������ʵ��
    m_mainWnd = CreateWindow(winClass.c_str(),    // ��������
                             m_winTitle.c_str(),  // ���ڱ���
                             WS_OVERLAPPEDWINDOW, // ���ڷ��
                             100,                 // ���Ͻ�����
                             100,
                             800,                 // �ߺͿ�
                             600,
                             0,                   // �޸�����
                             0,                   // ���Ӳ˵�
                             m_appInst,           // �����ô��ڵĳ�����
                             0);                  // ��ʹ�ø�ֵ
    if (FAILED(m_mainWnd)) {
        MessageBox(0, "CreateWindow FAILED", 0, 0);
        PostQuitMessage(0);
    }

    ShowWindow(m_mainWnd, SW_SHOW); // ��ʾ����
    UpdateWindow(m_mainWnd);        // �����û���
}

// ��Ϣѭ��������������豸�������Ϣ
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
            TranslateMessage(&msg); // ת����Ϣ��
            DispatchMessage(&msg);  // ����Ϣ���ݸ��ص�����
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