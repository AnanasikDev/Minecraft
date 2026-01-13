#pragma once

#include "json.hpp"

using json = nlohmann::json;

struct Gamerules
{
    enum class PhysicsMode
    {
        Flight,
        Normal
    };

    float m_defaultTime{ 0.0f };

    bool m_doDayNightCycle{ true };
    int m_generationDistance{ 8 };
    float m_gravity{ 24.0f };
    bool m_enableFrustumCulling{ false };
    bool m_renderWater{ true };
    float m_mouseSensitivity{ 1.0f };
    float m_TNTExplosionRadius{ 6 };
    PhysicsMode m_physicsMode{ PhysicsMode::Flight };

    friend void to_json(nlohmann::json& j, const Gamerules& g)
    {
        j = nlohmann::json{
            {"DoDayNight",          g.m_doDayNightCycle},
            {"GenerationDistance",  g.m_generationDistance},
            {"Gravity",             g.m_gravity},
            {"FrustumCulling",      g.m_enableFrustumCulling},
            {"RenderWater",         g.m_renderWater},
            {"PhysicsMode",         g.m_physicsMode},
            {"Sensitivity",         g.m_mouseSensitivity},
            {"TNTExplosionRadius",  g.m_TNTExplosionRadius},
        };
    }

    friend void from_json(const nlohmann::json& j, Gamerules& g)
    {
        g.m_doDayNightCycle = j.value("DoDayNight", true);
        g.m_generationDistance = j.value("GenerationDistance", 8);
        g.m_gravity = j.value("Gravity", 24.0f);
        g.m_enableFrustumCulling = j.value("FrustumCulling", false);
        g.m_renderWater = j.value("RenderWater", true);
        g.m_physicsMode = j.value("PhysicsMode", Gamerules::PhysicsMode::Flight);
        g.m_mouseSensitivity = j.value("Sensitivity", 1.0f);
        g.m_TNTExplosionRadius = j.value("TNTExplosionRadius", 6.0f);
    }
};

NLOHMANN_JSON_SERIALIZE_ENUM(Gamerules::PhysicsMode, {
    {Gamerules::PhysicsMode::Normal, "Normal"},
    {Gamerules::PhysicsMode::Flight, "Flight"}
    })
