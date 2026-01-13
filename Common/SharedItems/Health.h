#pragma once

struct Health
{
public:
	void Init(float max, float value, float autoregen);
	void Change(float delta);
	void Update(float dt);
	float GetValue() const;
	int GetIntValue() const;
	float& GetValueRef();

private:
	float m_max{ 0.0f };
	float m_value{ 0.0f };
	float m_autoregenPerSec{ 0.0f };
};
