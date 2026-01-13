#include "Chunk.h"
#include "Block.h"
#include "Saver.h"
#include "Player.h"
#include "commons.h"

const std::string Saver::SAVE_DIR = "saves/";
const std::string Saver::EXTENSION = ".save";
const std::string Saver::GLOBAL_FILE = "global" + EXTENSION;
const std::string Saver::GAMERULES_FILE = "gamerules";

void Saver::SaveGlobal(const Game* const game)
{
    Global data
    {
        game->m_player->m_transform.GetLocalPosition(),
        game->m_player->m_transform.GetLocalEulerAngles().y,
        game->m_player->m_camera->m_transform.GetLocalEulerAngles().x,
        GameTime::TimeOfDay()
    };

    SaveRaw(GetFullFileName(GLOBAL_FILE), reinterpret_cast<const char*>(&data), sizeof(Global));
}

bool Saver::LoadGlobal(Game* game)
{
    Global data;
    bool success{ LoadRaw(GetFullFileName(GLOBAL_FILE), reinterpret_cast<char*>(&data), sizeof(Global)) };
    if (!success) return false;

    game->m_player->m_transform.SetLocalPosition(data.m_playerPosition);
    game->m_player->m_transform.SetLocalEulerAngles(glm::vec3(0, data.m_playerRotation, 0));
    game->m_player->m_camera->m_transform.SetLocalEulerAngles(glm::vec3(data.m_cameraRotation, 0, 0));
    GameTime::SetDayTime(data.m_dayTime);

    return true;
}

void Saver::SaveChunk(const Chunk* const chunk)
{
    SaveRaw(GetChunkFileName(chunk), reinterpret_cast<const char*>(chunk->m_blocks->data()),
        chunk->m_blocks->size() * sizeof(Block));
}

bool Saver::LoadChunk(Chunk* const chunk)
{
    std::string filename = GetChunkFileName(chunk);
    return LoadRaw(filename, reinterpret_cast<char*>(chunk->m_blocks->data()),
        chunk->m_blocks->size() * sizeof(Block));
}

std::string Saver::GetChunkFileName(const Chunk* const chunk)
{
    return GetFullFileName("chunk_" +
        std::to_string(chunk->m_position.x) + "_" +
        std::to_string(chunk->m_position.y) + "_" +
        std::to_string(chunk->m_position.z) + EXTENSION);
}

std::string Saver::GetFullFileName(std::string filename)
{
    return std::string(SAVE_DIR) + filename;
}

bool Saver::SaveRaw(std::string filename, const char* data, std::streamsize size)
{
    std::filesystem::create_directories(SAVE_DIR);

    std::ofstream outFile(filename, std::ios::binary | std::ios::out);

    if (!outFile)
    {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    outFile.write(data, size);
    outFile.flush();

    outFile.close();
    return true;
}

bool Saver::LoadRaw(std::string filename, char* outdata, std::streamsize size)
{
    if (!std::filesystem::exists(filename))
    {
        return false;
    }

    std::ifstream inFile(filename, std::ios::binary | std::ios::in);
    if (!inFile)
    {
        return false;
    }

    inFile.read(outdata, size);

    inFile.close();
    return true;
}

void Saver::SaveGamerules(const Gamerules& rules)
{
    std::string fullPath = GetPlatformSpecificFileName(GAMERULES_FILE);
    SaveJsonFile(fullPath, rules);
}

bool Saver::LoadGamerules(Gamerules& outRules)
{
    std::string fullPath = GetPlatformSpecificFileName(GAMERULES_FILE);
    return LoadJsonFile(fullPath, outRules);
}

std::string Saver::GetPlatformSpecificFileName(std::string baseName)
{
    std::string suffix;

#if defined(_WIN32)
    suffix = "_windows";
#elif defined(__linux__) || defined(__unix__)
    suffix = "_pi";
#else
    suffix = "_generic";
#endif

    return GetFullFileName(baseName + suffix + ".json");
}
