/*****************************************
*
*     Direct的输入处理
*     基本照搬FRANK D.LUNA的龙书第二版
*
******************************************/
#ifndef _DINPUT_H
#define _DINPUT_H

#include <dinput.h>

class DirectInput {
public:
    DirectInput(DWORD keyboardCoopFlags, DWORD mouseCoopFlags);
    ~DirectInput();

    void GetCurState();
    bool KeyPress(char key);
    bool MouseButtonPress(int button);
    float MouseX();
    float MouseY();
    float MouseZ();
private:
    IDirectInput8* m_pDxInput;
    IDirectInputDevice8* m_pKeyBoardDevice;
    IDirectInputDevice8* m_pMouseDevice;
    char m_aKeyBoardState[256];
    DIMOUSESTATE2 m_cMouseState;
};

extern DirectInput* g_pDxInput;

#endif