#pragma once;

class Chunk;

struct RemeshRequest
{
	Chunk* chunk;
	Chunk* neighbourNorth;
	Chunk* neighbourSouth;
	Chunk* neighbourUp;
	Chunk* neighbourDown;
	Chunk* neighbourEast;
	Chunk* neighbourWest;

	RemeshRequest(Chunk* chunk, Chunk* neighbourNorth, Chunk* neighbourSouth, Chunk* neighbourUp, Chunk* neighbourDown, Chunk* neighbourEast, Chunk* neighbourWest)
		: chunk(chunk), neighbourNorth(neighbourNorth), neighbourSouth(neighbourSouth), neighbourUp(neighbourUp), neighbourDown(neighbourDown), neighbourEast(neighbourEast), neighbourWest(neighbourWest)
	{
		SetControl();
	}

	void SetControl();
	void ResetControl();

	RemeshRequest() = default;
};