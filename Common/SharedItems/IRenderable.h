#pragma once

class Camera;

class IRenderable
{
	virtual void Render(Camera* camera) = 0;
	virtual void RenderDebug(Camera* camera) { }
};