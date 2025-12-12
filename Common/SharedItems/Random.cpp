#include "Random.h"
#include <cstdlib>
#include <ctime>
#include "amath.h"

FastNoiseLite Random::valueNoise;


void Random::Init()
{
	srand(static_cast<int>(time(0)));
	valueNoise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Value);
	valueNoise.SetFrequency(1.0f);
}

int Random::GetInt(int minIncluding, int maxExcluding)
{
	return rand() % (maxExcluding - minIncluding) + minIncluding;
}

float Random::GetFloat()
{
	return static_cast <float>(rand()) / static_cast <float> (RAND_MAX);
}

float Random::GetFloat2D(float x, float y)
{
	return (valueNoise.GetNoise(x, y) + 1.0f) / 2.0f;
}

int Random::GetInt2D(int x, int y, int minIncluding, int maxExcluding)
{
	return static_cast<int>(AMath::Remap(Random::GetFloat2D(x, y), 0.0f, 1.0f, minIncluding, maxExcluding));
}

float Random::GetFloat(float min, float max)
{
	return GetFloat() * (max - min) + min;
}