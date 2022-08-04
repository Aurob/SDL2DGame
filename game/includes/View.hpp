#pragma once
#include <stdio.h>
#include <math.h>
#include <SDL_scancode.h>
#include <unordered_map>
#include <map>
#include <array>
#include <vector>
#include <algorithm>
#include "../includes/components.hpp"
#include "../includes/WorldGen.hpp"
#include "../includes/Factories.hpp"

extern int WIDTH, HEIGHT, TILESIZE, default_TILESIZE;
extern int deltaTime;
extern std::map<SDL_Scancode, bool> keyStates;
extern WorldGen WorldGenerator;
extern Factories Factory;

class View {
    public:
        View();
        int xOffset, yOffset;
        Position playerPosition;
        Movement playerMovement;
        std::vector<int> visibleNPCindices;
        std::vector<int> visibleRTCindices;
        // std::vector<Position> RTCPlayers;
        std::unordered_map<int, Position> RTCPlayers;
        int RTCTick;
        int bounds[4][2];
        // std::vector<std::pair<int*, float>> tileNoises;
        std::unordered_map<std::array<int, 2>, std::array<int, 3>, IntTupleHasher> tileNoises;
        int xmod, ymod;
        void update();
        bool updatePlayerPosition();
        void updateVisibleNPCs();
        void updateRTCPlayer(int, int, int, int, int);
        void removeRTCPlayer(int);
        bool positionsCollide(Position, Position);
};
