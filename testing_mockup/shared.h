#pragma once

#define TESTING

// Dimensions of our grid
namespace Constants
{
    inline constexpr int WIDTH = 3;
    inline constexpr int HEIGHT = 3;
}

typedef struct
{
    long durationMs;
    long targets[Constants::HEIGHT][Constants::WIDTH];
} configuration_t;

inline bool operator==(const configuration_t &lhs, const configuration_t &rhs)
{
    if (lhs.durationMs != rhs.durationMs)
    {
        return false;
    }

    for (int y = 0; y < Constants::HEIGHT; ++y)
    {
        for (int x = 0; x < Constants::WIDTH; ++x)
        {
            if (lhs.targets[y][x] != rhs.targets[y][x])
            {
                return false;
            }
        }
    }
    return true;
}

typedef struct
{
    configuration_t config;
    long phaseStart;
} full_state_t;

inline bool operator==(const full_state_t &lhs, const full_state_t &rhs)
{
    return lhs.phaseStart == rhs.phaseStart && lhs.config == rhs.config;
}

full_state_t updateFSM(full_state_t currState, unsigned long clock, bool emergencyStop, bool hasNewConfiguration, configuration_t newestConfiguration);
