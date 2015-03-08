/*****************************************
*
*     Direct的输入处理
*     基本照搬FRANK D.LUNA的龙书第二版
*
******************************************/
#include "d3dUtil.h"
#include "DirectInput.h"
#include "d3d9App.h"

DirectInput* g_pDxInput = 0;

DirectInput::DirectInput(DWORD keyboardCoopFlags, DWORD mouseCoopFlags)
{
    ZeroMemory(m_aKeyBoardState, sizeof(m_aKeyBoardState));
    ZeroMemory(&m_cMouseState, sizeof(m_cMouseState));

    HR(DirectInput8Create(g_pd3d9App->GetAppInst(),
                          DIRECTINPUT_VERSION, 
                          IID_IDirectInput8,
                          (void**)&m_pDxInput,
                          0));
    // 创建驱动、设置数据格式、设置Cooperative Level（不会翻译）
    HR(m_pDxInput->CreateDevice(GUID_SysKeyboard, &m_pKeyBoardDevice, 0));
    HR(m_pKeyBoardDevice->SetDataFormat(&c_dfDIKeyboard));
    HR(m_pKeyBoardDevice->SetCooperativeLevel(g_pd3d9App->GetMainWnd(), keyboardCoopFlags));
    HR(m_pKeyBoardDevice->Acquire());

    HR(m_pDxInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, 0));
    HR(m_pMouseDevice->SetDataFormat(&c_dfDIMouse2));
    HR(m_pMouseDevice->SetCooperativeLevel(g_pd3d9App->GetMainWnd(), mouseCoopFlags));
    HR(m_pMouseDevice->Acquire());
}

DirectInput::~DirectInput()
{
    ReleaseCOM(m_pDxInput);
    m_pKeyBoardDevice->Unacquire();
    m_pMouseDevice->Unacquire();
    ReleaseCOM(m_pKeyBoardDevice);
    ReleaseCOM(m_pMouseDevice);
}
// 获得键盘/鼠标设备所输入的信息
void DirectInput::GetCurState()
{
    // 将键盘设备输入的信息转移到m_aKeyBoardState数组中
    HRESULT hr = m_pKeyBoardDevice->GetDeviceState(sizeof(m_aKeyBoardState), (void**)&m_aKeyBoardState);

    // 失败之后则清空数组，并重新抓取
    if (FAILED(hr)) {
        ZeroMemory(m_aKeyBoardState, sizeof(m_aKeyBoardState));
        hr = m_pKeyBoardDevice->Acquire();
    }

    hr = m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&m_cMouseState);
    if (FAILED(hr)) {
        ZeroMemory(&m_cMouseState, sizeof(m_cMouseState));
        hr = m_pMouseDevice->Acquire();
    }
}

bool DirectInput::KeyPress(char key)
{
    return (m_aKeyBoardState[key] & 0x80) != 0;
}

bool DirectInput::MouseButtonPress(int button)
{
    return (m_cMouseState.rgbButtons[button] & 0x80) != 0;
}

float DirectInput::MouseX()
{
    return (float)m_cMouseState.lX;
}

float DirectInput::MouseY()
{
    return (float)m_cMouseState.lY;
}

float DirectInput::MouseZ()
{
    return (float)m_cMouseState.lZ;
}