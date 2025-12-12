#pragma once
#include "IInput.h"

#include <X11/Xlib.h>

class RaspMouse : public IMouse
{
public:
	RaspMouse(Display& display, Window& window);

	bool impl_IsButtonDown(MouseButtons button) const;
	glm::vec2 impl_GetPosition() const override;
	float GetScrollDelta() const override;
	void UpdateKey(MouseButtons button, bool state);
	void Update() override;

private:
	Display& display ;
	Window& window ; 
};

