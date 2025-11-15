#pragma once

class Camera;

class IRenderable
{
	virtual void Render(Camera* camera) = 0;
};