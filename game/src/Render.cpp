#include "../includes/Render.hpp"

Render::Render() : rect1{0, 0, WIDTH, HEIGHT},
                   color{255, 0, 0, 255},
                   rows{static_cast<int>(WIDTH / TILESIZE)},
                   cols{static_cast<int>(HEIGHT / TILESIZE)}
{
    printf("%d\n", view.bounds[0][0]);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    // Allows opacity
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);

    tileRect.w = TILESIZE;
    tileRect.h = TILESIZE;
}
bool compareInterval(Position i1, Position i2)
{
    return i1.globalY < i2.globalY && i1.globalX < i2.globalX; // && i1.y < i2.y;
}

void Render::render()
{

    SDL_RenderClear(renderer);
    renderView();
    // sort the intervals in increasing order of
    // start time
    // sort(Factory.npc_pool.begin(), Factory.npc_pool.end(), compareInterval);
    renderPlayer();
    renderRTCPlayers();
    renderNPCs();
    renderCrosshair();

    // renderSuperFormula();
    // renderPolygonTest();
    // SDL_RenderCopy(renderer, screenTexture, NULL, &rect1);
    SDL_RenderPresent(renderer);
    // SDL_DestroyTexture(screenTexture);
}

void Render::renderView()
{
    // if (SDL_MUSTLOCK(surface))
    //     SDL_LockSurface(surface);
    // Uint8 *pixels = (Uint8 *)surface->pixels;

    int r = 0, g = 0, b = 0;
    int rgb = 0;
    std::array<int, 3> _rgb;
    std::array<int, 2> tile_point;

    float n;
    for (int x = view.bounds[0][0]; x < view.bounds[1][0] + 1; ++x)
    {
        for (int y = view.bounds[0][1]; y < view.bounds[3][1] + 1; ++y)
        {
            // ID = hasher(std::to_string(x) + " " + std::to_string(y));
            tile_point = {x, y};
            auto tile_noise = view.tileNoises.find(tile_point);
            if (tile_noise == view.tileNoises.end())
            {
                n = WorldGenerator.getTerrainCubic(x, y);

                rgb = 256 * n;

                if (n < 0.35)
                { // water
                    r = rgb * 0;
                    g = rgb * 0;
                    b = rgb * 2;
                }
                else if (n < 0.39)
                { // sand
                    r = rgb * 1.8;
                    g = rgb * 1.7;
                    b = rgb * 1.2;
                }
                else if (n < 0.62)
                { // grass
                    r = 139 * n * 1.2;
                    g = 214 * n * 1.8;
                    b = 74 * 1.5;
                }
                else
                { // stone
                    r = 139 * (n * 1.6);
                    g = 119 * (n * 1.4);
                    b = 109 * (n * 1.1);
                }

                _rgb = {r, g, b};

                view.tileNoises.insert(std::make_pair(tile_point, _rgb));
            }
            else
            {
                r = tile_noise->second[0];
                g = tile_noise->second[1];
                b = tile_noise->second[2];
            }

            tileRect.x = -view.xmod + (x - view.bounds[0][0]) * TILESIZE;
            tileRect.y = -view.ymod + (y - view.bounds[0][1]) * TILESIZE;
            tileRect.w = TILESIZE;
            tileRect.h = TILESIZE;
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderFillRect(renderer, &tileRect);
            // tx1 = tx * TILESIZE;
            // ty1 = ty * TILESIZE;
            // // r = rand() % 255;
            // // g = rand() % 255;
            // // b = rand() % 255;
            // for (int _tyn = 0; _tyn < TILESIZE; _tyn++)
            // {
            //     tyn = ty1 + _tyn;
            //     for (int _txn = 0; _txn < TILESIZE; _txn++)
            //     {
            //         txn = tx1 + _txn;
            //         int i = tyn * WIDTH + txn;
            //         pixels[i * 4 + 0] = b;
            //         pixels[i * 4 + 1] = g;
            //         pixels[i * 4 + 2] = r;
            //         pixels[i * 4 + 3] = 255;
            //     }
            // }
        }
    }

    // if (SDL_MUSTLOCK(surface))
    //     SDL_UnlockSurface(surface);
    // screenTexture = SDL_CreateTextureFromSurface(renderer, surface);
}

bool Render::comparePositions(std::pair<std::array<int, 2>, float> i1, std::pair<std::array<int, 2>, float> i2)
{
    return i1.first[0] < i2.first[0] && i1.first[1] < i2.first[1];
}

void Render::renderNPCs()
{

    for (int npcID : view.visibleNPCindices)
    {
        // Position npcPosition = Factory.npc_pool[npcIndex];
        auto npc_map = Factory.npc_pool.find(npcID);
        NPC npc = npc_map->second;
        Position npcPosition = npc.position;
        Life npcLife = npc.life;
        Superformula npcSuperformula = npc.superformula;

        rect1.x = npcPosition.screenX;
        rect1.y = npcPosition.screenY;
        rect1.w = TILESIZE * npcPosition.size / 10;
        rect1.h = TILESIZE * npcPosition.size / 10;

        // Vector2f stepPoint;
        // int lastx, lasty;
        Sint16 vx[npcSuperformula.NP];
        Sint16 vy[npcSuperformula.NP];
        int i = 0;
        for (auto point : npcSuperformula.points)
        {
            float r = point.x;
            float phi = point.y;
            if (abs(r) != 0) {
                r = 1 / r;
                float x = r * cos(phi) * TILESIZE * 3;
                float y = r * sin(phi) * TILESIZE * 3;
                    
                vx[i] = x + npcPosition.screenX;
                vy[i] = y + npcPosition.screenY;
                i++;
            }
        }
       

        filledPolygonColor(renderer, vx, vy, npcSuperformula.NP, 0xA8603200);
        // SDL_SetRenderDrawColor(renderer, 255 - npcLife.health, 0, 0, 255);
        // SDL_RenderFillRect(renderer, &rect1);
    }
}

void Render::renderPlayer()
{
    // Player avatar
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    rect1.w = TILESIZE * view.playerPosition.size / 25;
    rect1.h = TILESIZE * view.playerPosition.size / 25;
    rect1.x = WIDTH / 2 - rect1.w / 2;
    rect1.y = HEIGHT / 2 - rect1.h / 2;
    SDL_RenderFillRect(renderer, &rect1);
}

void Render::renderRTCPlayers()
{
    for (auto playerID : view.visibleRTCindices)
    {
        auto player = view.RTCPlayers.find(playerID);
        if (player != view.RTCPlayers.end())
        {
            rect1.w = TILESIZE;
            rect1.h = TILESIZE;
            rect1.x = player->second.screenX;
            rect1.y = player->second.screenY;
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &rect1);
        }
    }
}

void Render::renderCrosshair()
{
    // Cross hair
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, WIDTH / 2 - rect1.w / 2, HEIGHT / 2, WIDTH / 2 + rect1.w / 2, HEIGHT / 2);
    SDL_RenderDrawLine(renderer, WIDTH / 2, HEIGHT / 2 - rect1.h / 2, WIDTH / 2, HEIGHT / 2 + rect1.h / 2);
}

void Render::recalculateTiles(int tilesize)
{
    TILESIZE = tilesize;
    rows = static_cast<int>(WIDTH / TILESIZE);
    cols = static_cast<int>(HEIGHT / TILESIZE);
}

void Render::renderPolygonTest()
{
    Sint16 vx[] = {100, 300, 200};
    Sint16 vy[] = {100, 100, 300};
    filledPolygonColor(renderer, vx, vy, 3, 0xFFFF0000);
}

void Render::renderSuperFormula()
{

    // int NP = 1000;
    // float phi;
    // float r, t1, t2;

    // float lastX = 0;
    // float lastY = 0;
    // for (int i = 0; i <= NP; i++) {

    // Vector2 stepPoint = WorldGenerator.superformulaStep(i, NP, 1, 1, 39.6, 6.612, 7.138, -6.250);
    // float r = stepPoint.x;
    // float phi = stepPoint.y;
    //     if (abs(r) != 0) {
    //         r = 1 / r;
    //         float x = r * cos(phi) * 500;
    //         float y = r * sin(phi) * 500;

    //         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //         // SDL_RenderDrawPointF(renderer, WIDTH/2 + stepPoint.x, HEIGHT/2 + stepPoint.y);
    //         SDL_RenderDrawLineF(renderer, WIDTH / 2 + lastX, HEIGHT / 2 + lastY, WIDTH / 2 + x, HEIGHT / 2 + y);
    //         // SDL_RenderDrawPointF(renderer, WIDTH / 2 + x, HEIGHT / 2 + y);
    //         // SDL_RenderDrawLineF(renderer, WIDTH / 2 + lastX, HEIGHT / 2 + lastY, WIDTH / 2 + x, HEIGHT / 2 + y);
    //         lastX = x;
    //         lastY = y;
    //     }
    //     // }
    // }
    float a = 1, b = 1, m = 39.6, n1 = 6.612, n2 = 7.138, n3 = -6.250;
    int NP = 1000;
    Superformula superformula{a, b, m, n1, n2, n3, NP};
    float phi;
    float r, t1, t2;

    float lastX = 0;
    float lastY = 0;
    for (int i = 0; i <= NP; i++)
    {
        Vector2f stepPoint = WorldGenerator.superformulaStep(superformula, i);
        float r = stepPoint.x;
        float phi = stepPoint.y;
        if (abs(r) != 0)
        {
            r = 1 / r;
            float x = r * cos(phi) * 500;
            float y = r * sin(phi) * 500;

            // SDL_RenderDrawPointF(renderer, WIDTH / 2 + x, HEIGHT / 2 + y);
            SDL_RenderDrawLineF(renderer, WIDTH / 2 + lastX, HEIGHT / 2 + lastY, WIDTH / 2 + x, HEIGHT / 2 + y);
            lastX = x;
            lastY = y;
        }
    }
}