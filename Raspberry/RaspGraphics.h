#pragma once
#include "IGraphics.h"

class XWindow;
struct EGLState;

class RaspGraphics : public IGraphics
{
public:
	RaspGraphics();
	~RaspGraphics() {};

	void Quit() override;

	void SwapBuffer() override;
	void PollEvents();

	void LockCursor() override;
	void UnlockCursor() override;
	void BeginFrame() override;
	void EndFrame() override;
	void InitGUI() override;

	XWindow& Window() const;

private:
	XWindow* window;
};

