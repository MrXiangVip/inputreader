//
// Created by xshx on 2025/5/26.
//

#ifndef UNTITLED_MANGMIUTILS_H
#define UNTITLED_MANGMIUTILS_H

#include <map>
// 使用 POSIX 的 clock_gettime
#include <time.h>
#include <string>

//////////////////////////////////////////////////////////////////////
//InputId
#define INPUT_ID_LEFT_JOYSTICK 1
#define INPUT_ID_RIGHT_JOYSTICK 2

#define INPUT_ID_UP 11
#define INPUT_ID_DOWN 12
#define INPUT_ID_LEFT 13
#define INPUT_ID_RIGHT 14

#define INPUT_ID_A 21
#define INPUT_ID_B 22
#define INPUT_ID_C 23
#define INPUT_ID_X 24
#define INPUT_ID_Y 25
#define INPUT_ID_Z 26

#define INPUT_ID_L1 31
#define INPUT_ID_L2 32
#define INPUT_ID_L3 33

#define INPUT_ID_R1 41
#define INPUT_ID_R2 42
#define INPUT_ID_R3 43

#define INPUT_ID_SELECT 51
#define INPUT_ID_START 52
#define INPUT_ID_MODE 53

#define INPUT_ID_BACK 61
#define INPUT_ID_HOME 62
#define INPUT_ID_MENU 63

#define INPUT_ID_M1 1001
#define INPUT_ID_M2 1002
#define INPUT_ID_M3 1003
#define INPUT_ID_M4 1004


#define DPAD_UP     103
#define DPAD_DOWN   108
#define DPAD_LEFT   105
#define DPAD_RIGHT  106
#define BUTTON_L1 310
#define BUTTON_L2 312
#define BUTTON_R1 311
#define BUTTON_R2 313
#define BTN_THUMBL 100
#define BTN_TR 100
#define BTN_TR2 100
#define BTN_THUMBR 100
#define BTN_SELECT 100
#define BTN_START 100
#define BTN_MODE 100
#define KEY_BACK 100
#define KEY_MENU 100

//////////////////////////////////////////////////////////////////////
//从 android/view/KeyEvent.java 得到的
#define KEYCODE_BUTTON_A 96
#define KEYCODE_BUTTON_B 97
#define KEYCODE_BUTTON_X 99
#define KEYCODE_BUTTON_Y 100

//从frameworks/base/data/keyboards/Generic.kl 得到的
#define BUTTON_A 304
#define BUTTON_B 305
#define BUTTON_C 306
#define BUTTON_X 307
#define BUTTON_Y 308
#define BUTTON_Z 309


//////////////////////////////////////////////////////////////////////
//KeyType
#define KEY_CATEGORY_GAMEPAD 1000
#define KEY_TYPE_GAMEPAD_CLICK_STANDARD (KEY_CATEGORY_GAMEPAD + 1)
#define KEY_TYPE_GAMEPAD_CLICK_TURBO (KEY_CATEGORY_GAMEPAD + 2)
#define KEY_TYPE_GAMEPAD_CLICK_TURBO_AUTO (KEY_CATEGORY_GAMEPAD + 3)
#define KEY_TYPE_GAMEPAD_CLICK_WHEN_PRESS (KEY_CATEGORY_GAMEPAD + 4)
#define KEY_TYPE_GAMEPAD_CLICK_WHEN_RELEASE (KEY_CATEGORY_GAMEPAD + 5)

#define KEY_CATEGORY_KEYBOARD 2000
#define KEY_TYPE_KEYBOARD_CLICK_STANDARD (KEY_CATEGORY_KEYBOARD + 1)
#define KEY_TYPE_KEYBOARD_CLICK_TURBO (KEY_CATEGORY_KEYBOARD + 2)
#define KEY_TYPE_KEYBOARD_CLICK_TURBO_AUTO (KEY_CATEGORY_KEYBOARD + 3)
#define KEY_TYPE_KEYBOARD_CLICK_WHEN_PRESS (KEY_CATEGORY_KEYBOARD + 4)
#define KEY_TYPE_KEYBOARD_CLICK_WHEN_RELEASE (KEY_CATEGORY_KEYBOARD + 5)

#define KEY_CATEGORY_MOUSE 3000
#define KEY_TYPE_MOUSE_CLICK_STANDARD (KEY_CATEGORY_MOUSE + 1)
#define KEY_TYPE_MOUSE_CLICK_TURBO (KEY_CATEGORY_MOUSE + 2)
#define KEY_TYPE_MOUSE_CLICK_TURBO_AUTO (KEY_CATEGORY_MOUSE + 3)
#define KEY_TYPE_MOUSE_CLICK_WHEN_PRESS (KEY_CATEGORY_MOUSE + 4)
#define KEY_TYPE_MOUSE_CLICK_WHEN_RELEASE (KEY_CATEGORY_MOUSE + 5)

#define KEY_CATEGORY_TOUCHSCREEN 4000
#define KEY_TYPE_TOUCHSCREEN_CLICK_STANDARD (KEY_CATEGORY_TOUCHSCREEN + 1)
#define KEY_TYPE_TOUCHSCREEN_CLICK_TURBO (KEY_CATEGORY_TOUCHSCREEN + 2)
#define KEY_TYPE_TOUCHSCREEN_CLICK_TURBO_AUTO (KEY_CATEGORY_TOUCHSCREEN + 3)
#define KEY_TYPE_TOUCHSCREEN_CLICK_WHEN_PRESS (KEY_CATEGORY_TOUCHSCREEN + 4)
#define KEY_TYPE_TOUCHSCREEN_CLICK_WHEN_RELEASE (KEY_CATEGORY_TOUCHSCREEN + 5)

#define KEY_CATEGORY_MOBA 5000
#define KEY_TYPE_MOBA_ATTACK (KEY_CATEGORY_MOBA + 1) //ignore
#define KEY_TYPE_MOBA_CANCEL_SKILL (KEY_CATEGORY_MOBA + 2)
#define KEY_TYPE_MOBA_LEFT_JOYSTICK_ASSOCIATION (KEY_CATEGORY_MOBA + 3)
#define KEY_TYPE_MOBA_RIGHT_JOYSTICK_ASSOCIATION (KEY_CATEGORY_MOBA + 4)
#define KEY_TYPE_MOBA_SMART_JOYSTICK_ASSOCIATION (KEY_CATEGORY_MOBA + 5)
#define KEY_TYPE_MOBA_VIEW_MAP (KEY_CATEGORY_MOBA + 6)

//////////////////////////////////////////////////////////////////////
//JoystickType
#define JOYSTICK_CATEGORY_GAMEPAD 1000
#define JOYSTICK_TYPE_GAMEPAD_LEFT_JOYSTICK (JOYSTICK_CATEGORY_GAMEPAD + 1)
#define JOYSTICK_TYPE_GAMEPAD_RIGHT_JOYSTICK (JOYSTICK_CATEGORY_GAMEPAD + 2)
#define JOYSTICK_TYPE_GAMEPAD_ARROW_KEYS (JOYSTICK_CATEGORY_GAMEPAD + 3) //ignore

#define JOYSTICK_CATEGORY_KEYBOARD 2000
#define JOYSTICK_TYPE_KEYBOARD_ARROW_KEYS (JOYSTICK_CATEGORY_KEYBOARD + 1)

#define JOYSTICK_CATEGORY_MOUSE 3000
#define JOYSTICK_TYPE_MOUSE_POINTER (JOYSTICK_CATEGORY_MOUSE + 1)
#define JOYSTICK_TYPE_MOUSE_WHEEL (JOYSTICK_CATEGORY_MOUSE + 2)

#define JOYSTICK_CATEGORY_TOUCHSCREEN 4000
#define JOYSTICK_TYPE_TOUCHSCREEN_JOYSTICK (JOYSTICK_CATEGORY_TOUCHSCREEN + 1)
#define JOYSTICK_TYPE_TOUCHSCREEN_SWIPE (JOYSTICK_CATEGORY_TOUCHSCREEN + 2) //ignore
#define JOYSTICK_TYPE_TOUCHSCREEN_SKILL_SHOT (JOYSTICK_CATEGORY_TOUCHSCREEN + 3)
#define JOYSTICK_TYPE_TOUCHSCREEN_ADJUST_VIEW (JOYSTICK_CATEGORY_TOUCHSCREEN + 4)

/////////////////////////////////////////////////////////////////////////////////
#define ABS_X			0x00  // 水平轴（X轴）
#define ABS_Y			0x01  // 垂直轴（Y轴）
#define ABS_Z			0x02  // Z轴（如游戏手柄的油门）
#define ABS_RX			0x03  // 旋转X轴（右摇杆X）
#define ABS_RY			0x04  // 旋转Y轴（右摇杆Y）
#define ABS_RZ			0x05  // 旋转Z轴
#define ABS_HAT0X		0x10  // 方向键X
#define ABS_HAT0Y		0x11  // 方向键Y
#define ABS_GAS         0x09  //L2
#define ABS_BREAK       0x0A  //R2

//////////////////////////////////////////////////////////////////////////////////
#define TOUCH_NONE 0
#define TOUCH_DOWN 1
#define TOUCH_MOVE 2
#define TOUCH_UP   3

/*
 *  工具类 : 从 (android侧)的 KeyCode 映射到 (linux侧) 的ScanCode ,
 * */
class MangmiUtils{

public :
    static int initKeyCodeToScanCodeMap( );
    static int  getScanCodeFromKeyCode(int keyCode);

    static int initInputIdMaps();
    static int getInputIdFromEvcode(int evCode);
    static int getEvcodeFromInputId(int inputId);
    static long long getSystemTimePosix();

    static bool isNumeric(const std::string& str);

private:
//
    static std::map<int, int> keyCodeScanCodeMap;
    static std::map<int , int > inputIdMaps;// 第一个int 是 inputID, 第二个int 是 keycode

};

#endif //UNTITLED_MANGMIUTILS_H
