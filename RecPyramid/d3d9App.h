/****************************************************
*    Author: Tianhao Wang
*    Date:   2015-03-06   
*    Desc:   һ��ͨ�õ�d3d9�࣬ģ������ڶ������
*****************************************************/   
#ifndef _D3D9APP_H
#define _D3D9APP_H

#include "d3dUtil.h"

class d3d9App {
public:
    d3d9App(HINSTANCE hInstance, std::string winTitle, D3DDEVTYPE devType, DWORD vertexProc);
    virtual ~d3d9App();

    // Direct3D�ĳ�ʼ��
    virtual void InitD3D();
    // Windows�����ڵĳ�ʼ��
    virtual void InitWindow();
    // ��Ϣ����
    virtual LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);
    // ��Ϣ���Ĳ鿴������
    virtual int MsgLoop();
    HINSTANCE GetAppInst() { return m_appInst; }
    HWND GetMainWnd() { return m_mainWnd; }

    // ���±����ڼ̳�ʱ���ж���
    virtual void DrawScene()            { }
    virtual void UpdateScene(float dt)    { }
protected:
    D3DDEVTYPE m_devType;          // ��������
    DWORD m_vertexProc;            // ���㻺��
    HINSTANCE m_appInst;           // �������Ŀ���
    HWND m_mainWnd;                // ���ھ��
    D3DPRESENT_PARAMETERS m_d3dPP;
    std::string m_winTitle;        // �������
};

extern LPDIRECT3D9 g_pd3dObject;
extern LPDIRECT3DDEVICE9 g_pd3dDevice;
extern d3d9App* g_pd3d9App;

#endif