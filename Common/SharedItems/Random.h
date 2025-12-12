#pragma once

#include "FastNoiseLite/FastNoiseLite.h"

class Random
{
public:
	static void Init();
	static int GetInt(int minIncluding, int maxExcluding);
	static float GetFloat(float min, float max);
	static float GetFloat();
	static float GetFloat2D(float x, float y);
	static int GetInt2D(int x, int y, int minIncluding, int maxExcluding);

private:
	static FastNoiseLite valueNoise;
};