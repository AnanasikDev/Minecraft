#pragma once

class IMouse;
class IKeyboard;

class Input
{
public:
	Input(IKeyboard* const keyboard, IMouse* const mouse);
	~Input();

	IKeyboard& GetKeyboard();
	IMouse& GetMouse();

private:
	IMouse* const m_mouse;
	IKeyboard* const m_keyboard;
};
