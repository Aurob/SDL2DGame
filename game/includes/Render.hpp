#pragma once
#include <SDL2/SDL.h>
#include <SDL_scancode.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <map>
#include <string> //for std::hash
#include <algorithm> //used for the sort function
#include <vector>
#include <utility>  //declarations of unique_ptr
#include "../includes/components.hpp"
#include "../includes/WorldGen.hpp"
#include "../includes/Factories.hpp"
#include "../includes/View.hpp"

extern int WIDTH, HEIGHT, TILESIZE, default_TILESIZE;
extern View view;
extern std::map<SDL_Scancode, bool> keyStates;
extern WorldGen WorldGenerator;
extern Factories Factory;
class Render {
    public:
        SDL_Renderer *renderer;
        SDL_Window *window;
        SDL_Texture *screenTexture;
        int rows;
        int cols;
        int seed;
        Render();
        void render();
        void renderView();
        void renderNPCs();
        void renderPlayer();
        void renderRTCPlayers();
        void renderCrosshair();
        void renderSuperFormula();
        void renderPolygonTest();
        void recalculateTiles(int);
        
        static bool comparePositions(std::pair<std::array<int, 2>, float>, std::pair<std::array<int, 2>, float>);

        int getTilesize() {
            return TILESIZE;
        }
        void setSeed(int s) {
            seed = s;
            srand(seed);
        }

    private:
        SDL_Surface *surface;
        SDL_Rect rect1;
        SDL_Rect tileRect;
        SDL_Color color;
        std::hash<std::string> hasher;
};
