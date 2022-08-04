#pragma once
#include <stdio.h>
#include <math.h>
#include <SDL_scancode.h>
#include <map>
#include <vector>
#include "../includes/components.hpp"
#include "../includes/WorldGen.hpp"

extern int WIDTH, HEIGHT, TILESIZE, default_TILESIZE;
class NPC {
    public:
        NPC();

        void tickLife(Life);
};
