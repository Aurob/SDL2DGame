#pragma once
#include <vector>
#include "math.h"
#include <unordered_map>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "../includes/components.hpp"
#include "../includes/WorldGen.hpp"

extern int WIDTH, HEIGHT, TILESIZE, default_TILESIZE;
extern WorldGen WorldGenerator;
class Factories {
    public:
        // std::vector<Position> npc_pool;
        // std::vector<int> npc_destroy_queue;
        // std::vector<Life> npc_life_pool;

        std::unordered_map<int, NPC> npc_pool;
        int npcIndex;
        
        void makeNPC(int, int);
        Factories();
};
