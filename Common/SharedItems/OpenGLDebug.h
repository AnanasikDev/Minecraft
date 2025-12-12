#pragma once

#include "BaseDebug.h"

class OpenGLDebug : public BaseDebug
{
public:
	OpenGLDebug()
	{
		if (instance == nullptr)
		{
			instance = this;
		}
	}
	bool StartErrorCapture() override;
	bool EndErrorCapture() override;

private:
	bool ClearErrors();
};