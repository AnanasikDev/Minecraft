#pragma once

#include "Input.h"
#include "IInput.h"
#include "WindowsInput.h"
#include <GLFW/glfw3.h>
#include <iostream>

static KeyState keyboard_keystates[IKeyboard::MAX_KEYS];
static KeyState mouse_keystates[IMouse::MAX_BUTTONS];

WindowsMouse::WindowsMouse(GLFWwindow& window) : window(window)
{
	glfwSetScrollCallback(&window, ScrollCallback);
	glfwSetMouseButtonCallback(&window, MouseButtonCallback);
	m_keystate = mouse_keystates;
}

glm::vec2 WindowsMouse::impl_GetPosition() const
{
	double xPosition, yPosition;
	glfwGetCursorPos(&window, &xPosition, &yPosition);
	return glm::vec2{ xPosition, yPosition };
}

float WindowsMouse::GetScrollDelta() const
{
	return scrollDelta;
}

float WindowsMouse::scrollDelta{ 0.0f };

void WindowsMouse::ScrollCallback(GLFWwindow* /*window*/, double /*xOffset*/, double yOffset)
{
	scrollDelta = static_cast<float>(yOffset);
}

void WindowsMouse::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		//printf("press\n");
		if (mouse_keystates[button] == KeyState::Idle || mouse_keystates[button] == KeyState::Release)
		{
			mouse_keystates[button] = KeyState::Press;
		}
		else if (mouse_keystates[button] == KeyState::Hold || mouse_keystates[button] == KeyState::Press)
		{
			mouse_keystates[button] = KeyState::Hold;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		//printf("release\n");
		if (mouse_keystates[button] == KeyState::Idle || mouse_keystates[button] == KeyState::Release)
		{
			mouse_keystates[button] = KeyState::Idle;
		}
		else if (mouse_keystates[button] == KeyState::Hold || mouse_keystates[button] == KeyState::Press)
		{
			mouse_keystates[button] = KeyState::Release;
		}
	}
}

static void KeyEventCallback(GLFWwindow*, int key, int, int action, int)
{
	if (action == GLFW_PRESS)
	{
		//printf("press\n");
		if (keyboard_keystates[key] == KeyState::Idle || keyboard_keystates[key] == KeyState::Release)
		{
			keyboard_keystates[key] = KeyState::Press;
		}
		else if (keyboard_keystates[key] == KeyState::Hold || keyboard_keystates[key] == KeyState::Press)
		{
			keyboard_keystates[key] = KeyState::Hold;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		//printf("release\n");
		if (keyboard_keystates[key] == KeyState::Idle || keyboard_keystates[key] == KeyState::Release)
		{
			keyboard_keystates[key] = KeyState::Idle;
		}
		else if (keyboard_keystates[key] == KeyState::Hold || keyboard_keystates[key] == KeyState::Press)
		{
			keyboard_keystates[key] = KeyState::Release;
		}
	}
}

WindowsKeyboard::WindowsKeyboard(GLFWwindow& window) : window(window)
{
	glfwSetKeyCallback(&window, KeyEventCallback);
	m_keystate = keyboard_keystates;
}

KeyState WindowsKeyboard::GetKeyState(Key key) const
{
	return *GetKeyHandleR(key);
}

KeyState* const WindowsKeyboard::GetKeyHandleRW(Key key) const
{
	switch (key)
	{
		case Key::A:			return &m_keystate[GLFW_KEY_A];
		case Key::B:			return &m_keystate[GLFW_KEY_B];
		case Key::C:			return &m_keystate[GLFW_KEY_C];
		case Key::D:			return &m_keystate[GLFW_KEY_D];
		case Key::E:			return &m_keystate[GLFW_KEY_E];
		case Key::F:			return &m_keystate[GLFW_KEY_F];
		case Key::G:			return &m_keystate[GLFW_KEY_G];
		case Key::H:			return &m_keystate[GLFW_KEY_H];
		case Key::I:			return &m_keystate[GLFW_KEY_I];
		case Key::J:			return &m_keystate[GLFW_KEY_J];
		case Key::K:			return &m_keystate[GLFW_KEY_K];
		case Key::L:			return &m_keystate[GLFW_KEY_L];
		case Key::M:			return &m_keystate[GLFW_KEY_M];
		case Key::N:			return &m_keystate[GLFW_KEY_N];
		case Key::O:			return &m_keystate[GLFW_KEY_O];
		case Key::P:			return &m_keystate[GLFW_KEY_P];
		case Key::Q:			return &m_keystate[GLFW_KEY_Q];
		case Key::R:			return &m_keystate[GLFW_KEY_R];
		case Key::S:			return &m_keystate[GLFW_KEY_S];
		case Key::T:			return &m_keystate[GLFW_KEY_T];
		case Key::U:			return &m_keystate[GLFW_KEY_U];
		case Key::V:			return &m_keystate[GLFW_KEY_V];
		case Key::W:			return &m_keystate[GLFW_KEY_W];
		case Key::X:			return &m_keystate[GLFW_KEY_X];
		case Key::Y:			return &m_keystate[GLFW_KEY_Y];
		case Key::Z:			return &m_keystate[GLFW_KEY_Z];
		case Key::NUM_0:		return &m_keystate[GLFW_KEY_0];
		case Key::NUM_1:		return &m_keystate[GLFW_KEY_1];
		case Key::NUM_2:		return &m_keystate[GLFW_KEY_2];
		case Key::NUM_3:		return &m_keystate[GLFW_KEY_3];
		case Key::NUM_4:		return &m_keystate[GLFW_KEY_4];
		case Key::NUM_5:		return &m_keystate[GLFW_KEY_5];
		case Key::NUM_6:		return &m_keystate[GLFW_KEY_6];
		case Key::NUM_7:		return &m_keystate[GLFW_KEY_7];
		case Key::NUM_8:		return &m_keystate[GLFW_KEY_8];
		case Key::NUM_9:		return &m_keystate[GLFW_KEY_9];
		case Key::TAB:			return &m_keystate[GLFW_KEY_TAB];
		case Key::CAPS_LOCK:	return &m_keystate[GLFW_KEY_CAPS_LOCK];
		case Key::SHIFT_LEFT:	return &m_keystate[GLFW_KEY_LEFT_SHIFT];
		case Key::CTRL_LEFT:	return &m_keystate[GLFW_KEY_LEFT_CONTROL];
		case Key::ALT_LEFT:		return &m_keystate[GLFW_KEY_LEFT_ALT];
		case Key::ESCAPE:		return &m_keystate[GLFW_KEY_ESCAPE];
		case Key::RIGHT_SHIFT:	return &m_keystate[GLFW_KEY_RIGHT_SHIFT];
		case Key::ENTER:		return &m_keystate[GLFW_KEY_ENTER];
		case Key::ARROW_UP:		return &m_keystate[GLFW_KEY_UP];
		case Key::ARROW_RIGHT:	return &m_keystate[GLFW_KEY_RIGHT];
		case Key::ARROW_DOWN:	return &m_keystate[GLFW_KEY_DOWN];
		case Key::ARROW_LEFT:	return &m_keystate[GLFW_KEY_LEFT];
		case Key::SPACE:		return &m_keystate[GLFW_KEY_SPACE];
		default: std::cout << "ERROR::INPUT::WINDOWS Keycode not supported: " << static_cast<int>(key) << std::endl; return nullptr;
	}
}

const KeyState* const WindowsKeyboard::GetKeyHandleR(Key key) const
{
	return GetKeyHandleRW(key);
}

void WindowsKeyboard::Update()
{
	Flush();
}