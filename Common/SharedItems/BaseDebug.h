#pragma once

class BaseDebug
{
public:
	static BaseDebug* instance;
	static constexpr bool enabled{ false };
	static bool show;

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

#ifdef _DEBUG
	#define START_ERROR_CAPTURE() BaseDebug::instance->StartErrorCapture();
	#define END_ERROR_CAPTURE() if (!BaseDebug::instance->EndErrorCapture()) { std::cout << " In file: " << __FILE__ << " at line " << __LINE__ << "\n"; }
#else
	#define START_ERROR_CAPTURE() do{}while(false)
	#define END_ERROR_CAPTURE() do{}while(false)
#endif