#include "Chunk.h"
#include "RemeshRequest.h"

void RemeshRequest::SetControl()
{
	if (neighbourNorth)	neighbourNorth->m_controls++;
	if (neighbourSouth)	neighbourSouth->m_controls++;
	if (neighbourUp)	neighbourUp->m_controls++;
	if (neighbourDown)	neighbourDown->m_controls++;
	if (neighbourEast)	neighbourEast->m_controls++;
	if (neighbourWest)	neighbourWest->m_controls++;
}

void RemeshRequest::ResetControl()
{
	if (neighbourNorth)	neighbourNorth->m_controls--;
	if (neighbourSouth)	neighbourSouth->m_controls--;
	if (neighbourUp)	neighbourUp->m_controls--;
	if (neighbourDown)	neighbourDown->m_controls--;
	if (neighbourEast)	neighbourEast->m_controls--;
	if (neighbourWest)	neighbourWest->m_controls--;
}
