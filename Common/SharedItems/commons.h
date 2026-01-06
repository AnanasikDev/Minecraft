#pragma once

#include <chrono>
#include <stdio.h>
#include <glh.h>
#include "glm/vec3.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <type_traits>

class Timer
{
public:
	Timer() : accumulated_time(0), running(false) {}

	void start()
	{
		if (!running)
		{
			start_time = std::chrono::high_resolution_clock::now();
			running = true;
		}
	}

	void stop()
	{
		if (running)
		{
			accumulated_time += current_session_time();
			running = false;
		}
	}

	void reset()
	{
		accumulated_time = 0;
		running = false;
	}

	void set_value(double seconds)
	{
		accumulated_time = seconds;
		if (running)
		{
			start_time = std::chrono::high_resolution_clock::now();
		}
	}

	double elapsed() const
	{
		double total = accumulated_time;
		if (running)
		{
			total += current_session_time();
		}
		return total;
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
	double accumulated_time;
	bool running;

	double current_session_time() const
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = now - start_time;
		return diff.count();
	}
};

template <typename GLT>
constexpr GLenum GetGLEnumType() {
	if (std::is_same_v<GLT, float>) return GL_FLOAT;

	if (std::is_same_v<GLT, int>) return GL_INT;
	if (std::is_same_v<GLT, unsigned int>) return GL_UNSIGNED_INT;

	if (std::is_same_v<GLT, short>) return GL_SHORT;
	if (std::is_same_v<GLT, unsigned short>) return GL_UNSIGNED_SHORT;

	if (std::is_same_v<GLT, char>) return GL_BYTE;
	if (std::is_same_v<GLT, unsigned char>) return GL_UNSIGNED_BYTE;

	printf("Unknown GLenum in %s:%d", __FILE__, __LINE__);
	return GL_FLOAT;
}

constexpr bool IsGLEnumTypeInt(GLenum type)
{
	return 
		type == GL_BYTE || 
		type == GL_UNSIGNED_BYTE || 
		type == GL_SHORT || 
		type == GL_UNSIGNED_SHORT || 
		type == GL_INT || 
		type == GL_UNSIGNED_INT;
}

enum class GridVec : unsigned char
{
	Invalid,
	Top,
	Bottom,
	Front,
	Back,
	Right,
	Left
};

inline GridVec directions[6]{
	GridVec::Top,
	GridVec::Bottom,
	GridVec::Right,
	GridVec::Left,
	GridVec::Back,
	GridVec::Front
};

glm::ivec3 GridVecToVec3(GridVec vec);
GridVec Vec3ToGridVec(glm::ivec3 vec);
GridVec InvertGridVec(GridVec vec);
glm::ivec3 InvertVec3(glm::ivec3 vec);

#define UnwrapGridVec(vec) vec.x, vec.y, vec.y

struct GameTime
{
private:
	static inline std::chrono::system_clock::time_point m_StartTime;
	static Timer m_Timer;

public:
	static float DAY_SECONDS;

	static void Init()
	{
		m_StartTime = std::chrono::system_clock::now();
		m_Timer.reset();
	}

	static void SetDayTime(float time01)
	{
		m_Timer.set_value(time01 * DAY_SECONDS);
	}

	static void ToggleDayCycle(bool value)
	{
		if (value)
		{
			m_Timer.start();
		}
		else
		{
			m_Timer.stop();
		}
	}

	static float SinceStartup()
	{
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsed = now - m_StartTime;
		return elapsed.count();
	}

	static long long SinceUnixEpoch()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count();
	}

	static float TimeOfDay()
	{
		return fmodf(TimeOfDayUnclamped(), 1.0f);
	}

	static float GetSunIntensity()
	{
		const float t{ TimeOfDay() };
		if (t > 0.75f) return powf(t - 0.75f, 2.0f)/0.125f;
		return 0.5f * (sinf(glm::pi<float>() * 2.0f * t) + 1.0f);
	}

	static float TimeOfDayUnclamped()
	{
		return m_Timer.elapsed() / DAY_SECONDS;
	}
};