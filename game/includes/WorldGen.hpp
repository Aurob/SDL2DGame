#pragma once
#include <time.h>
#include "math.h"
#include "../includes/components.hpp"
#include "../includes/FastNoise.hpp"
class WorldGen {
    public:
        WorldGen();
        FastNoise terrainNoise;

        
        float getTerrainSimplex(int, int);
        float getTerrainCubic(int, int);
        float superformula(float, float, float, float, float, float);
        Vector2f superformulaStep(Superformula, int);
        
        void setTerrainSeed(int s) {
            terrainNoise.SetSeed(s);
        }
};
