#include "RaspKeyboard.h"

#include <iostream>
#include <vector>
#include <map>
#include <atomic>
#include <thread>
#include <mutex>
#include <cstring>
#include <unistd.h> // for usleep

// X11 Headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

// Shared atomic state between the background X11 thread and the main Update loop
// 512 to cover standard keyboards.
static std::atomic<bool> g_PhysicalKeyState[512] = { false };

// Map X11 KeySyms to the Engine's Key Enum
static std::map<KeySym, int> g_KeySymMap;

// Helper to bridge pthread to member function
static void* InternalThreadHelper(void* context)
{
    ((RaspKeyboard*)context)->ProcessKeyboard();
    return nullptr;
}

static void Register(int xkey, Key key)
{
    g_KeySymMap[xkey] = static_cast<int>(key);
}

RaspKeyboard::RaspKeyboard() : keyboardThread(0)
{
    Register(XK_a, Key::A);
    Register(XK_b, Key::B);
    Register(XK_c, Key::C);
    Register(XK_d, Key::D);
    Register(XK_e, Key::E);
    Register(XK_f, Key::F);
    Register(XK_g, Key::G);
    Register(XK_h, Key::H);
    Register(XK_i, Key::I);
    Register(XK_j, Key::J);
    Register(XK_k, Key::K);
    Register(XK_l, Key::L);
    Register(XK_m, Key::M);
    Register(XK_n, Key::N);
    Register(XK_o, Key::O);
    Register(XK_p, Key::P);
    Register(XK_q, Key::Q);
    Register(XK_r, Key::R);
    Register(XK_s, Key::S);
    Register(XK_t, Key::T);
    Register(XK_u, Key::U);
    Register(XK_v, Key::V);
    Register(XK_w, Key::W);
    Register(XK_x, Key::X);
    Register(XK_y, Key::Y);
    Register(XK_z, Key::Z);
    Register(XK_Escape, Key::ESCAPE);
    Register(XK_space, Key::SPACE);
    Register(XK_0, Key::NUM_0);
    Register(XK_0, Key::NUM_0);
    Register(XK_1, Key::NUM_1);
    Register(XK_2, Key::NUM_2);
    Register(XK_3, Key::NUM_3);
    Register(XK_4, Key::NUM_4);
    Register(XK_5, Key::NUM_5);
    Register(XK_6, Key::NUM_6);
    Register(XK_7, Key::NUM_7);
    Register(XK_8, Key::NUM_8);
    Register(XK_9, Key::NUM_9);
    Register(XK_Shift_L, Key::SHIFT_LEFT);
    Register(XK_Shift_R, Key::RIGHT_SHIFT);
    Register(XK_Control_L, Key::CTRL_LEFT);
    Register(XK_Return, Key::ENTER);
    Register(XK_Up, Key::ARROW_UP);
    Register(XK_Down, Key::ARROW_DOWN);
    Register(XK_Right, Key::ARROW_RIGHT);
    Register(XK_Left, Key::ARROW_LEFT);

    std::cout << "[RaspKeyboard] Initializing X11 Event System..." << std::endl;

    int result = pthread_create(&keyboardThread, nullptr, InternalThreadHelper, this);
    if (result != 0)
    {
        std::cerr << "[RaspKeyboard] Error: Failed to create thread." << std::endl;
    }
}

RaspKeyboard::~RaspKeyboard()
{
    if (keyboardThread)
    {
        pthread_cancel(keyboardThread);
        pthread_join(keyboardThread, nullptr);
    }
}

void RaspKeyboard::Update()
{
    const int MAX_KEY_COUNT = 512;

    for (int i = 0; i < MAX_KEY_COUNT; ++i)
    {
        Key key = static_cast<Key>(i);
        KeyState* statePtr = GetKeyHandleRW(key);

        // Skip keys we haven't touched yet
        if (!statePtr) continue;

        bool isPhysicallyDown = g_PhysicalKeyState[i];

        switch (*statePtr)
        {
        case KeyState::Idle:
            if (isPhysicallyDown) *statePtr = KeyState::Press;
            break;

        case KeyState::Press:
            if (isPhysicallyDown) *statePtr = KeyState::Hold;
            else *statePtr = KeyState::Release;
            break;

        case KeyState::Hold:
            if (!isPhysicallyDown) *statePtr = KeyState::Release;
            break;

        case KeyState::Release:
            if (isPhysicallyDown) *statePtr = KeyState::Press;
            else *statePtr = KeyState::Idle;
            break;
        }
    }
}

KeyState RaspKeyboard::GetKeyState(Key key) const
{
    const KeyState* state = GetKeyHandleR(key);
    return state ? *state : KeyState::Idle;
}

static std::map<Key, KeyState> m_StateBuffer;

KeyState* const RaspKeyboard::GetKeyHandleRW(Key key) const
{
    if (m_StateBuffer.find(key) == m_StateBuffer.end())
    {
        m_StateBuffer[key] = KeyState::Idle;
    }
    return &m_StateBuffer[key];
}

const KeyState* const RaspKeyboard::GetKeyHandleR(Key key) const
{
    auto it = m_StateBuffer.find(key);
    if (it != m_StateBuffer.end()) return &it->second;
    static KeyState dummy = KeyState::Idle;
    return &dummy;
}

void RaspKeyboard::ProcessKeyboard()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display)
    {
        std::cerr << "[RaspKeyboard] Error: Cannot open X Display!" << std::endl;
        return;
    }

    Window focusWin = None;
    int revert;

    std::cout << "[RaspKeyboard] Waiting for window focus..." << std::endl;
    while (focusWin == None || focusWin == PointerRoot)
    {
        XGetInputFocus(display, &focusWin, &revert);
        usleep(100000); // Sleep 100ms
    }

    std::cout << "[RaspKeyboard] Attached to Window ID: " << focusWin << std::endl;

    XSelectInput(display, focusWin, KeyPressMask | KeyReleaseMask);

    XEvent event;
    while (true)
    {
        XNextEvent(display, &event);

        if (event.type == KeyPress || event.type == KeyRelease)
        {
            KeySym keysym;
            char buf[8] = { 0 };

            XLookupString(&event.xkey, buf, sizeof(buf), &keysym, nullptr);

            auto it = g_KeySymMap.find(keysym);
            if (it != g_KeySymMap.end())
            {
                int engineKeyIndex = it->second;

                if (event.type == KeyPress)
                {
                    g_PhysicalKeyState[engineKeyIndex] = true;
                }
                else if (event.type == KeyRelease)
                {
                    g_PhysicalKeyState[engineKeyIndex] = false;
                }
            }
        }
    }

    XCloseDisplay(display);
}
