#pragma once

class Random
{
public:
	static void Init();
	static int GetInt(int minIncluding, int maxExcluding);
	static float GetFloat(float min, float max);
	static float GetFloat();
};