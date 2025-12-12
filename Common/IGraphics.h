// a simple interface class its basically just simple access to graphics
#pragma once
class IGraphics
{
protected:
	bool m_isCursorLocked{ false };
public:
	virtual ~IGraphics() = default;
	virtual void SwapBuffer() = 0;
	virtual void Quit() = 0;
	virtual void LockCursor() = 0;
	virtual void UnlockCursor() = 0;
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void InitGUI() = 0;
	inline bool IsCursorLocked() const { return m_isCursorLocked; }
};

