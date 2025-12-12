#include "Input.h"

#include "IInput.h"

Input::Input(IKeyboard* const keyboard, IMouse* const mouse) :
	mouse(mouse),
	keyboard(keyboard)
{
}

Input::~Input()
{
	delete(mouse);
	delete(keyboard);
}


IKeyboard& Input::GetKeyboard()
{
	return *keyboard;
}

IMouse& Input::GetMouse()
{
	return *mouse;
}

glm::vec2 IMouse::GetPositionDelta() const
{
	return m_currentPosition - m_prevPosition;
}

void IMouse::Update()
{
	m_prevPosition = m_currentPosition;
	m_currentPosition = impl_GetPosition();
}

void IMouse::Init()
{
	m_currentPosition = impl_GetPosition();
	m_prevPosition = m_currentPosition;
}

glm::vec2 IMouse::GetPosition() const
{
	return m_currentPosition;
}