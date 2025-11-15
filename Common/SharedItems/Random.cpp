#include "Random.h"
#include <cstdlib>
#include <ctime>

void Random::Init()
{
	srand(time(0));
}

int Random::GetInt(int minIncluding, int maxExcluding)
{
	return rand() % (maxExcluding - minIncluding) + minIncluding;
}

float Random::GetFloat()
{
	return static_cast <float>(rand()) / static_cast <float> (RAND_MAX);
}

float Random::GetFloat(float min, float max)
{
	return GetFloat() * (max - min) + min;
}