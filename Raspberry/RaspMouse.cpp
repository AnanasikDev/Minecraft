#include "RaspMouse.h"

static KeyState mouse_keystates[IMouse::MAX_BUTTONS];

RaspMouse::RaspMouse(Display& display, Window& window) :
	display(display),
	window(window)
{
	m_keystate = mouse_keystates;
}

bool RaspMouse::impl_IsButtonDown(MouseButtons button) const
{
	int root_x;
	int root_y;
	int win_x;
	int win_y;
	uint mask_return;
	Window window_returned;
	XQueryPointer(
		&display,
		window,
		&window_returned,
		&window_returned,

		&root_x,
		&root_y,
		&win_x,
		&win_y,
		&mask_return);

	switch (button) 
	{
		case MouseButtons::LEFT:
		{
			return mask_return & Button1MotionMask;
		}
		case MouseButtons::RIGHT:
		{
			return mask_return & Button3MotionMask;
		}
		case MouseButtons::MIDDLE:
		{
			return mask_return & Button2MotionMask;
		}
		default:
		{
			return false;
		}
	}
}

glm::vec2 RaspMouse::impl_GetPosition() const
{
	int root_x;
	int root_y;
	int win_x;
	int win_y;
	uint mask_return;
	Window window_returned;
	XQueryPointer(
		&display,
		window,
		&window_returned,
		&window_returned,

		&root_x,
		&root_y,
		&win_x,
		&win_y,
		&mask_return);

	return glm::vec2{win_x, win_y};
}

float RaspMouse::GetScrollDelta() const
{
	return 0;
}

void RaspMouse::UpdateKey(MouseButtons button, bool state)
{
	const bool isDown = state;
	KeyState* val = &mouse_keystates[static_cast<int>(button)];
	if (isDown)
	{
		switch (*val)
		{
		case KeyState::Idle:	*val = KeyState::Press; break;
		case KeyState::Release: *val = KeyState::Press; break;
		case KeyState::Press:	*val = KeyState::Hold; break;
		case KeyState::Hold:	*val = KeyState::Hold; break;
		default:
			break;
		}
	}
	else if (!isDown)
	{
		switch (*val)
		{
		case KeyState::Idle:	*val = KeyState::Idle; break;
		case KeyState::Release: *val = KeyState::Idle; break;
		case KeyState::Press:	*val = KeyState::Release; break;
		case KeyState::Hold:	*val = KeyState::Release; break;
		default:
			break;
		}
	}
}

void RaspMouse::Update()
{
	IMouse::Update();
	UpdateKey(MouseButtons::LEFT, impl_IsButtonDown(MouseButtons::LEFT));
	UpdateKey(MouseButtons::MIDDLE, impl_IsButtonDown(MouseButtons::MIDDLE));
	UpdateKey(MouseButtons::RIGHT, impl_IsButtonDown(MouseButtons::RIGHT));
}
