#pragma once

class BaseDebug
{
public:
	static BaseDebug* instance;
	BaseDebug()
	{
		if (instance == nullptr)
		{
			instance = this;
		}
	}

	virtual bool StartErrorCapture() = 0;
	virtual bool EndErrorCapture() = 0;
};

#define START_ERROR_CAPTURE() BaseDebug::instance->StartErrorCapture();

#define END_ERROR_CAPTURE() if (!BaseDebug::instance->EndErrorCapture()) { std::cout << " In file: " << __FILE__ << " at line " << __LINE__ << "\n"; }