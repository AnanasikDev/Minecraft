#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include "Transform.h"
#include "Gamerules.h"
#include "json.hpp"

class Chunk;
class Game;

class Saver
{
public:
    struct Global
    {
        Transform::Position m_playerPosition;
        float m_playerRotation;
        float m_cameraRotation;
        float m_dayTime;
    };

    static const std::string SAVE_DIR;
    static const std::string EXTENSION;
    static const std::string GLOBAL_FILE;
    static const std::string GAMERULES_FILE;

    void SaveGlobal(const Game* const game);
    bool LoadGlobal(Game* game);
    void SaveChunk(const Chunk* const chunk);
    bool LoadChunk(Chunk* const chunk);

    static void SaveGamerules(const Gamerules& rules);
    static bool LoadGamerules(Gamerules& outRules);

private:
    static std::string GetFullFileName(std::string filename);
    static std::string GetChunkFileName(const Chunk* const chunk);

    static std::string GetPlatformSpecificFileName(std::string baseName);

    static bool SaveRaw(std::string filename, const char* data, std::streamsize size);
    static bool LoadRaw(std::string filename, char* outdata, std::streamsize size);

    template <typename T>
    static bool SaveJsonFile(const std::string& filename, const T& data)
    {
        std::filesystem::create_directories(SAVE_DIR);
        std::ofstream outFile(filename);
        if (!outFile)
        {
            std::cerr << "Failed to open JSON file for writing: " << filename << std::endl;
            return false;
        }

        nlohmann::json j = data;
        outFile << j.dump(4);
        return true;
    }

    template <typename T>
    static bool LoadJsonFile(const std::string& filename, T& outData)
    {
        if (!std::filesystem::exists(filename)) return false;

        std::ifstream inFile(filename);
        if (!inFile) return false;

        try
        {
            nlohmann::json j;
            inFile >> j;
            outData = j;
            return true;
        }
        catch (const nlohmann::json::exception& e)
        {
            std::cerr << "JSON Parse Error in " << filename << ": " << e.what() << std::endl;
            return false;
        }
    }
};