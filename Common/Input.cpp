#include "Input.h"

#include "IInput.h"

Input::Input(const IKeyboard* const keyboard, const IMouse* const mouse) :
	mouse(mouse),
	keyboard(keyboard)
{
}

Input::~Input()
{
	delete(mouse);
	delete(keyboard);
}


const IKeyboard& Input::GetKeyboard() const
{
	return *keyboard;
}

const IMouse& Input::GetMouse() const
{
	return *mouse;
}

glm::vec2 IMouse::GetPositionDelta() const
{
	return m_currentPosition - m_prevPosition;
}

void IMouse::Update() const
{
	m_prevPosition = m_currentPosition;
	m_currentPosition = impl_GetPosition();
}

void IMouse::Init() const
{
	m_currentPosition = impl_GetPosition();
	m_prevPosition = m_currentPosition;
}

glm::vec2 IMouse::GetPosition() const
{
	return m_currentPosition;
}