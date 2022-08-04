#include <emscripten.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>
#include <string>
#include <SDL_thread.h>
#include "../includes/components.hpp"
#include "../includes/View.hpp"
#include "../includes/WorldGen.hpp"
#include "../includes/Render.hpp"
#include "../includes/Factories.hpp"

extern int WIDTH, HEIGHT, TILESIZE, default_TILESIZE;
extern int lastTime;
extern int deltaTime;
extern View view;
extern WorldGen WorldGenerator;
extern Factories Factory;
extern std::map<SDL_Scancode, bool> keyStates;

int WIDTH = 1024;
int HEIGHT = 768;
int TILESIZE = 8;
int default_TILESIZE = TILESIZE;
int deltaTime = 0;
int lastTime = 0;
View view;
WorldGen WorldGenerator;
Factories Factory;
std::map<SDL_Scancode, bool> keyStates;

using namespace std;

struct context
{
    // SDL_Renderer *renderer;
    SDL_Event event;
    int iteration;
};

SDL_Rect tex_rect;
SDL_Rect screen_rect;

Render renderer;

// JS bindings
int value;
int JSBackward, JSForward, JSLeft, JSRight;
int peerID;
int peerIDNeedsSet;
int seed;
int seedNeedsUpdate;
int playerCount;

void mainloop(void *arg);
void SDLCALL EventHandler(void *userdata, SDL_Event *event);
void JavascriptUpdateSelf();
void JavascriptUpdatePeers();

int main(int argc, char *argv[])
{
    printf("Hello, world!\n");
    seed = time(NULL);
    EM_ASM_INT({
        setSeed($0);
    },seed);
    srand(seed);

    // Initialize the Game object
    TTF_Init();
    SDL_Init(SDL_INIT_VIDEO);

    context ctx;
    renderer.setSeed(seed);
    emscripten_set_main_loop_arg(mainloop, &ctx, 0, 1);
    SDL_DestroyRenderer(renderer.renderer);
    SDL_DestroyWindow(renderer.window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void mainloop(void *arg)
{
    deltaTime = (SDL_GetTicks() - lastTime);
    lastTime = SDL_GetTicks();

    context *ctx = static_cast<context *>(arg);

    while (SDL_PollEvent(&ctx->event))
    {
        EventHandler(0, &ctx->event);
    }

    
    // if(rtcPlayers != playerCount) {
    //     printf("Player count changed\n");
    //     playerCount = rtcPlayers;
    // }

    JavascriptUpdateSelf();
    view.updatePlayerPosition();
    view.update();
    view.updateVisibleNPCs();
    JavascriptUpdatePeers();
    // store the player position in the DOM
    // EM_ASM_INT({
    //     updatePosition($0, $1, $2, $3);
    // }, view.playerPosition.globalX, view.playerPosition.globalY, view.playerPosition.tileGX, view.playerPosition.tileGY);


    if(peerID) {
        EM_ASM({
            setPlayerX($0);
        }, view.playerPosition.globalX);
        EM_ASM({
            setPlayerY($0);
        }, view.playerPosition.globalY);
        EM_ASM({
            setPlayerTX($0);
        }, view.playerPosition.tileGX);
        EM_ASM({
            setPlayerTY($0);
        }, view.playerPosition.tileGY);

        EM_ASM({
            updatePlayerPeer();
        });
    }
    // //Update RTC Player positions
    // // if (view.RTCTick > 2000 == 0) {
    // view.RTCTick = 0;

    // int playerX = EM_ASM_INT({
    //     return getPlayerX($0);
    // }, playerID);

    // int playerY = EM_ASM_INT({
    //     return getPlayerY($0);
    // }, playerID);

    // int playerTX = EM_ASM_INT({
    //     return getPlayerTX($0);
    // }, playerID);

    // int playerTY = EM_ASM_INT({
    //     return getPlayerTY($0);
    // }, playerID);
    // 
    // for (int p = 0; p < rtcPlayers; p++)
    // {
    //     int playerID = EM_ASM_INT({
    //         return getPlayerID($0);
    //     }, p);

    //     if(playerID) {
    //         int playerX = EM_ASM_INT({
    //             return getPlayerX($0);
    //         }, playerID);

    //         int playerY = EM_ASM_INT({
    //             return getPlayerY($0);
    //         }, playerID);

    //         int playerTX = EM_ASM_INT({
    //             return getPlayerTX($0);
    //         }, playerID);

    //         int playerTY = EM_ASM_INT({
    //             return getPlayerTY($0);
    //         }, playerID);

    //         printf("Player %d: %d, %d, %d, %d\n", playerID, playerX, playerY, playerTX, playerTY);
    //         view.updateRTCPlayer(playerID, playerX, playerY, playerTX, playerTY);
    //     }
    // }
    // }

    // else {
    //     view.RTCTick += deltaTime;
    // }
    
    renderer.render();
    
    ctx->iteration++;
}

void SDLCALL EventHandler(void *userdata, SDL_Event *event)
{
    value = EM_ASM_INT({
        return document.querySelector('#test').value;
    });
    switch (event->type)
    {
    case SDL_MOUSEBUTTONDOWN:
        // renderer.recalculateTiles(value);
        // renderer.setSeed(time(NULL)+rand()%100);
        if (view.playerPosition.colliding)
        {
            printf("Colliding\n");
        }
        break;

    case SDL_MOUSEBUTTONUP:
        break;

    case SDL_MOUSEMOTION:
        break;

    case SDL_KEYUP:
        keyStates[event->key.keysym.scancode] = false;
        break;

    case SDL_KEYDOWN:
        keyStates[event->key.keysym.scancode] = true;
        break;

    case SDL_MOUSEWHEEL:
        if (event->wheel.y < 0 && renderer.getTilesize() > 2)
        {
            renderer.recalculateTiles(renderer.getTilesize() / 2);
        }
        if (event->wheel.y > 0 && renderer.getTilesize() < 128)
        {
            renderer.recalculateTiles(renderer.getTilesize() * 2);
        }
        break;
    default:
        break;
    }
}

void JavascriptUpdateSelf() {

    //Check if peer ID needs to be set
    peerIDNeedsSet = EM_ASM_INT({
        return peerIDNeedsSet($0);
    });

    if (peerIDNeedsSet && peerIDNeedsSet != peerID) {
        peerID = peerIDNeedsSet;
        printf("peerIDNeedsSet: %d\n", peerIDNeedsSet);
    }

    //Check if seeds needs updating
    seedNeedsUpdate = EM_ASM_INT({
        return seedNeedsUpdate();
    });

    if(seedNeedsUpdate) {
        printf("Seed needs updating\n");
        
        seed = EM_ASM_INT({
            return getSeed();
        });

        srand(seed);
        renderer.setSeed(seed);
        WorldGenerator.setTerrainSeed(seed);
        
        EM_ASM_INT({
            setSeed($0);
        }, seed);
    }
}

void JavascriptUpdatePeers() {

    int removedPlayerCount = EM_ASM_INT({
        return getRemovedPlayersCount();
    });

    if(removedPlayerCount > 0) {
        printf("Removed player count: %d\n", removedPlayerCount);
        for(int i = 0; i < removedPlayerCount; i++) {
            int playerID = EM_ASM_INT({
                return getRemovedPlayerID($0);
            }, i);
            printf("Removed player: %d\n", playerID);
            view.removeRTCPlayer(playerID);
        }
    }

    int rtcPlayers = EM_ASM_INT({
        return getPlayerCount();
    });

    // printf("RTC Players: %d\n", rtcPlayers);
    view.visibleRTCindices.clear();
    for(int p = 0; p < rtcPlayers + 1; p++) {
        int playerID = EM_ASM_INT({
            return getPlayerID($0);
        }, p);

        // printf("Player %d - %d\n", playerID, peerID);
        if(playerID != peerID) {
            int playerX = EM_ASM_INT({
                return getPlayerX($0);
            }, playerID);
            int playerY = EM_ASM_INT({
                return getPlayerY($0);
            }, playerID);
            int playerTX = EM_ASM_INT({
                return getPlayerTX($0);
            }, playerID);
            int playerTY = EM_ASM_INT({
                return getPlayerTY($0);
            }, playerID);
            // printf("Player %d: %d, %d, %d, %d\n", playerID, playerX, playerY, playerTX, playerTY);
            
            view.updateRTCPlayer(playerID, playerX, playerY, playerTX, playerTY);
        }
    }
}
