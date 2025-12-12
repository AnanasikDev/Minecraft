#pragma once
// group these 2 togeter

#include <stdio.h>
#include <glm/vec2.hpp>
#ifdef WINDOWS
#include <GLFW/glfw3.h>
#endif

enum class KeyState
{
	Idle = 0,
	Press,
	Hold,
	Release
};

enum class MouseButtons
{
	BUTTON_FIRST = 0,
	LEFT = 0,	
	RIGHT,
	MIDDLE,
	BUTTON_LAST = MIDDLE,
};

class IMouse {
public:
	static constexpr int MAX_BUTTONS{ 3 };

private:
	glm::vec2 m_prevPosition{ 0, 0 };
	glm::vec2 m_currentPosition{0, 0};
	inline KeyState* const GetButtonHandleRW(MouseButtons button) const
	{
		return &m_keystate[static_cast<int>(button)];
	}
	inline KeyState* const GetButtonHandleR(MouseButtons button) const
	{
		return &m_keystate[static_cast<int>(button)];
	}

protected:
	virtual glm::vec2 impl_GetPosition() const = 0;
	KeyState* m_keystate{ nullptr };

public:
	inline bool IsButtonDown(MouseButtons button) const
	{
		const KeyState state{ *GetButtonHandleR(button) };
		return state == KeyState::Hold || state == KeyState::Press;
	}
	inline bool IsButtonUp(MouseButtons button) const
	{
		const KeyState state{ *GetButtonHandleR(button) };
		return state == KeyState::Idle || state == KeyState::Release;
	}
	inline bool IsButtonPressed(MouseButtons button) const
	{
		const KeyState state{ *GetButtonHandleR(button) };
		return state == KeyState::Press;
	}
	inline bool IsButtonReleased(MouseButtons button) const
	{
		const KeyState state{ *GetButtonHandleR(button) };
		return state == KeyState::Release;
	}
	inline bool IsButtonHeld(MouseButtons button) const
	{
		const KeyState state{ *GetButtonHandleR(button) };
		return state == KeyState::Hold;
	}
	inline bool IsButtonIdle(MouseButtons button) const
	{
		const KeyState state{ *GetButtonHandleR(button) };
		return state == KeyState::Idle;
	}
	virtual glm::vec2 GetPositionDelta() const;
	virtual glm::vec2 GetPosition() const;
	virtual void Update();
	void Init();
	virtual float GetScrollDelta() const = 0;

	inline void Flush()
	{
		for (int key = static_cast<int>(MouseButtons::BUTTON_FIRST); key < IMouse::MAX_BUTTONS; key++)
		{
			KeyState* val = &m_keystate[static_cast<int>(key)];
			if (!val)
			{
				printf("Invalid key\n");
				continue;
			}
			switch (*val)
			{
			case KeyState::Idle:	*val = KeyState::Idle; break;
			case KeyState::Release: *val = KeyState::Idle; break;
			case KeyState::Press:	*val = KeyState::Hold;  break;
			case KeyState::Hold:	*val = KeyState::Hold;  break;
			default: break;
			}
		}
	}

	virtual ~IMouse() = default;
};



enum class Key
{
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	NUM_0,
	NUM_1,
	NUM_2,
	NUM_3,
	NUM_4,
	NUM_5,
	NUM_6,
	NUM_7,
	NUM_8,
	NUM_9,
	TAB,
	CAPS_LOCK,
	SHIFT_LEFT,
	CTRL_LEFT,
	ALT_LEFT,
	ESCAPE,
	RIGHT_SHIFT,
	ENTER,
	ARROW_UP,
	ARROW_RIGHT,
	ARROW_DOWN,
	ARROW_LEFT,
	SPACE,

	LAST_KEY
};

class IKeyboard
{
public:
	static constexpr int MAX_KEYS = 512;
	virtual KeyState GetKeyState(Key key) const = 0;
	virtual void Update() = 0;
	inline bool IsKeyDown(Key key) const
	{
		const KeyState state{ *GetKeyHandleR(key) };
		return state == KeyState::Hold || state == KeyState::Press;
	}
	inline bool IsKeyUp(Key key) const
	{
		const KeyState state{ *GetKeyHandleR(key) };
		return state == KeyState::Idle || state == KeyState::Release;
	}
	inline bool IsKeyHeld(Key key) const
	{
		const KeyState state{ *GetKeyHandleR(key) };
		return state == KeyState::Hold;
	}
	inline bool IsKeyReleased(Key key) const
	{
		const KeyState state{ *GetKeyHandleR(key) };
		return state == KeyState::Release;
	}
	inline bool IsKeyPressed(Key key) const
	{
		const KeyState state{ *GetKeyHandleR(key) };
		return state == KeyState::Press;
	}
	inline bool IsKeyIdle(Key key) const
	{
		const KeyState state{ *GetKeyHandleR(key) };
		return state == KeyState::Idle;
	}

	virtual ~IKeyboard() = default;

protected:
	KeyState* m_keystate{ nullptr };
	virtual KeyState* const GetKeyHandleRW(Key key) const = 0;
	virtual const KeyState* const GetKeyHandleR(Key key) const = 0;

	inline void Flush()
	{
		for (int key = 0; key < static_cast<int>(Key::LAST_KEY); key++)
		{
			KeyState& val = *GetKeyHandleRW(static_cast<Key>(key));
			switch (val)
			{
			case KeyState::Idle:	val = KeyState::Idle; break;
			case KeyState::Release: val = KeyState::Idle; break;
			case KeyState::Press:	val = KeyState::Hold;  break;
			case KeyState::Hold:	val = KeyState::Hold;  break;
			default: break;
			}
		}
	}
};





