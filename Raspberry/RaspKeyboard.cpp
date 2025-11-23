#include "RaspKeyboard.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <linux/input.h>
#include "IInput.h"
#include <malloc.h>

static KeyState keystates[IKeyboard::MAX_KEYS];
static FILE* fKeyboard;

KeyState* const RaspKeyboard::GetKeyHandleRW(Key key) const
{
	switch (key)
	{
	case Key::A:			return &m_keystate[KEY_A];
	case Key::B:			return &m_keystate[KEY_B];
	case Key::C:			return &m_keystate[KEY_C];
	case Key::D:			return &m_keystate[KEY_D];
	case Key::E:			return &m_keystate[KEY_E];
	case Key::F:			return &m_keystate[KEY_F];
	case Key::G:			return &m_keystate[KEY_G];
	case Key::H:			return &m_keystate[KEY_H];
	case Key::I:			return &m_keystate[KEY_I];
	case Key::J:			return &m_keystate[KEY_J];
	case Key::K:			return &m_keystate[KEY_K];
	case Key::L:			return &m_keystate[KEY_L];
	case Key::M:			return &m_keystate[KEY_M];
	case Key::N:			return &m_keystate[KEY_N];
	case Key::O:			return &m_keystate[KEY_O];
	case Key::P:			return &m_keystate[KEY_P];
	case Key::Q:			return &m_keystate[KEY_Q];
	case Key::R:			return &m_keystate[KEY_R];
	case Key::S:			return &m_keystate[KEY_S];
	case Key::T:			return &m_keystate[KEY_T];
	case Key::U:			return &m_keystate[KEY_U];
	case Key::V:			return &m_keystate[KEY_V];
	case Key::W:			return &m_keystate[KEY_W];
	case Key::X:			return &m_keystate[KEY_X];
	case Key::Y:			return &m_keystate[KEY_Y];
	case Key::Z:			return &m_keystate[KEY_Z];
	case Key::NUM_0:		return &m_keystate[KEY_0];
	case Key::NUM_1:		return &m_keystate[KEY_1];
	case Key::NUM_2:		return &m_keystate[KEY_2];
	case Key::NUM_3:		return &m_keystate[KEY_3];
	case Key::NUM_4:		return &m_keystate[KEY_4];
	case Key::NUM_5:		return &m_keystate[KEY_5];
	case Key::NUM_6:		return &m_keystate[KEY_6];
	case Key::NUM_7:		return &m_keystate[KEY_7];
	case Key::NUM_8:		return &m_keystate[KEY_8];
	case Key::NUM_9:		return &m_keystate[KEY_9];
	case Key::TAB:			return &m_keystate[KEY_TAB];
	case Key::CAPS_LOCK:	return &m_keystate[KEY_CAPSLOCK];
	case Key::SHIFT_LEFT:	return &m_keystate[KEY_LEFTSHIFT];
	case Key::CTRL_LEFT:	return &m_keystate[KEY_LEFTCTRL];
	case Key::ALT_LEFT:		return &m_keystate[KEY_LEFTALT];
	case Key::ESCAPE:		return &m_keystate[KEY_ESC];
	case Key::RIGHT_SHIFT:	return &m_keystate[KEY_RIGHTSHIFT];
	case Key::ENTER:		return &m_keystate[KEY_ENTER];
	case Key::ARROW_UP:		return &m_keystate[KEY_UP];
	case Key::ARROW_RIGHT:	return &m_keystate[KEY_RIGHT];
	case Key::ARROW_DOWN:	return &m_keystate[KEY_DOWN];
	case Key::ARROW_LEFT:	return &m_keystate[KEY_LEFT];
	case Key::SPACE:		return &m_keystate[KEY_SPACE];
	default: std::cout << "ERROR::INPUT::Rasp Keycode not supported: " << static_cast<int>(key) << std::endl; return nullptr;
	}
}

const KeyState* const RaspKeyboard::GetKeyHandleR(Key key) const
{
	return GetKeyHandleRW(key);
}

RaspKeyboard::RaspKeyboard()
{
	m_keystate = keystates;
	FindKeyboardLocation();
	fKeyboard = fopen(keyboardLocation.c_str(), "r");

	if (fKeyboard == nullptr)
	{
		printf("Keyboard file not found.\n");
	}
}

RaspKeyboard::~RaspKeyboard()
{
	fclose(fKeyboard);
}

KeyState RaspKeyboard::GetKeyState(Key key) const
{
	return *GetKeyHandleR(key);
}

void RaspKeyboard::Update() const
{
	ProcessKeyboard();
}

void RaspKeyboard::FindKeyboardLocation()
{
	std::string ev{FindActiveKeyboardEv()};
	keyboardLocation = "/dev/input/" + ev;
}

std::string RaspKeyboard::FindActiveKeyboardEv()
{
	std::ifstream devicesFile;
	devicesFile.open("/proc/bus/input/devices");

	std::stringstream devicesStream;

	devicesStream << devicesFile.rdbuf();
	devicesFile.close();

	std::string devices = devicesStream.str();

	// The index in the file for our current new line position.
	std::size_t newLinePos{0};

	// The EV value for the current device.
	std::string ev{};

	// Moves every new line in devices file.
	do
	{
		// Checks for the Handlers field.
		// Always caches it, so we can use it if we find the correct device.
		if(devices.substr(newLinePos + 1, 11) == "H: Handlers")
		{
			// Index of the end of the line.
			std::size_t nextNewLine{devices.find('\n', newLinePos + 1) - 1};

			// The contents of the EV line.
			std::string line{devices.substr(newLinePos + 1, nextNewLine - (newLinePos + 1))};

			// The index of the last space, so we can get the last value.
			std::size_t lastSpace{line.rfind(' ')};

			// The very last value, which is the event.
			ev = line.substr(lastSpace + 1, line.length() - lastSpace + 1);
		}

		// Checks if current line start with the EV field.
		if(devices.substr(newLinePos + 1, 5) == "B: EV")
		{
			// The start of the value, while skipping the 'B: EV'.
			std::size_t start{newLinePos + 7};

			newLinePos = devices.find('\n', newLinePos + 1);

			// Grabs the string value of the EV field.
			std::string stringValue{devices.substr(start, newLinePos - start)};

			// Moves the hex string into an uint.
			unsigned int value;
			std::stringstream ss;
			ss << std::hex << stringValue;
			ss >> value;

			// Defines the mask for a keyboard.
			constexpr unsigned int keyboardMask{0x120013};

			if((value & keyboardMask) == keyboardMask)
			{
				break;
			}

			continue;
		}


		newLinePos = devices.find('\n', newLinePos + 1);
	} while(newLinePos != std::string::npos);

	return ev;
}

void RaspKeyboard::ProcessKeyboard() const
{
	Flush();

	input_event event;
	fread(&event, sizeof(input_event), 1, fKeyboard);

	if (event.type == (__u16)EV_KEY)
	{
		const bool isDown = event.value > 0;
		KeyState& val = m_keystate[event.code];
		if (isDown)
		{
			switch (val)
			{
			case KeyState::Idle:	val = KeyState::Press; break;
			case KeyState::Release: val = KeyState::Press; break;
			case KeyState::Press:	val = KeyState::Hold; break;
			case KeyState::Hold:	val = KeyState::Hold; break;
			default:
				break;
			}
		}
		else if (!isDown)
		{
			switch (val)
			{
			case KeyState::Idle:	val = KeyState::Idle; break;
			case KeyState::Release: val = KeyState::Idle; break;
			case KeyState::Press:	val = KeyState::Release; break;
			case KeyState::Hold:	val = KeyState::Release; break;
			default:
				break;
			}
		}
	}
}
