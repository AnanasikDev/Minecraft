#pragma once
// 2 input classes
#include "IInput.h"
#include <GLFW/glfw3.h>

#include <iostream>

class WindowsKeyboard : public IKeyboard
{
public:
	WindowsKeyboard(GLFWwindow& window);
	KeyState GetKeyState(Key key) const override;
	void Update() const override;

protected:
	KeyState* const GetKeyHandleRW(Key key) const override;
	const KeyState* const GetKeyHandleR(Key key) const override;
private:
	GLFWwindow& window;
};


class WindowsMouse : public IMouse
{
public:
	WindowsMouse(GLFWwindow& window);

	bool GetButtonDown(MouseButtons button) const override;
	glm::vec2 impl_GetPosition() const override;
	float GetScrollDelta() const override;
private:
	GLFWwindow& window;

	static float scrollDelta;
	static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
};
