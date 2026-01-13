#include "Health.h"

void Health::Init(float max, float value, float autoregen)
{
	m_max = max;
	m_value = value;
	m_autoregenPerSec = autoregen;
}

void Health::Change(float delta)
{
	m_value += delta;
	if (m_value > m_max) m_value = m_max;
	if (m_value < 0) m_value = 0;
}

void Health::Update(float dt)
{
	m_value += m_autoregenPerSec * dt;
}

float Health::GetValue() const
{
	return m_value;
}

int Health::GetIntValue() const
{
	return static_cast<int>(m_value);
}

float& Health::GetValueRef()
{
	return m_value;
}