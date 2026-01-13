#pragma once
#include <IInput.h>
#include <string>

class RaspKeyboard : public IKeyboard
{
protected:
	KeyState* const GetKeyHandleRW(Key key) const override;
	const KeyState* const GetKeyHandleR(Key key) const override;

public:
	RaspKeyboard();
	~RaspKeyboard();
	KeyState GetKeyState(Key key) const override;
	void Update() override;

	pthread_t keyboardThread;
	std::string keyboardLocation;

	void ProcessKeyboard();
};

