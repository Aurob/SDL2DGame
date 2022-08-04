#include "../includes/View.hpp"
View::View() : playerMovement{.speed = 100}, // static_cast<float>(default_TILESIZE/32)},
               playerPosition{.globalX = 0, .globalY = 0, .size = 50}
{
}

void View::update()
{
    // Simple update when the tile size is the default size
    if (TILESIZE == default_TILESIZE)
    {
        xOffset = (fmod(playerPosition.globalX, static_cast<float>(TILESIZE)));
        yOffset = (fmod(playerPosition.globalY, static_cast<float>(TILESIZE)));
    }
    // If the tile size is not default, add "zoom offsets" to ensure the view remains centered
    else
    {
        xOffset = (fmod(playerPosition.globalX, static_cast<float>(default_TILESIZE)) / static_cast<float>(default_TILESIZE)) * static_cast<float>(TILESIZE);
        yOffset = (fmod(playerPosition.globalY, static_cast<float>(default_TILESIZE)) / static_cast<float>(default_TILESIZE)) * static_cast<float>(TILESIZE);
    }

    // Fix for stuttering effect in negative coordinates
    if (playerPosition.tileGX < 1 && playerPosition.globalX < 0)
        playerPosition.tileGX -= 1;
    if (playerPosition.tileGY < 1 && playerPosition.globalY < 0)
        playerPosition.tileGY -= 1;

    // Set view bounds based on the user's global position
    // These values are used for determining which tiles to render
    bounds[0][0] = (playerPosition.tileGX - ((WIDTH / TILESIZE) / 2));
    bounds[0][1] = (playerPosition.tileGY - ((HEIGHT / TILESIZE) / 2));
    bounds[1][0] = bounds[0][0] + (WIDTH / TILESIZE);
    bounds[1][1] = bounds[0][1];
    bounds[2][0] = bounds[0][0];
    bounds[2][1] = bounds[0][1] + (HEIGHT / TILESIZE);
    bounds[3][0] = bounds[1][0];
    bounds[3][1] = bounds[2][1];

    // offset directions based on negative/positive positions
    xmod = (playerPosition.tileGX < 0) ? xOffset + TILESIZE : xOffset;
    ymod = (playerPosition.tileGY < 0) ? yOffset + TILESIZE : yOffset;

    // Zoom scaled size
    // playerPosition.size = TILESIZE / 10;
}

bool View::updatePlayerPosition()
{

    float tempSpeed = ceil((static_cast<float>(deltaTime) / 1000) * playerMovement.speed);

    Position temp = playerPosition;

    if (keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_UP])
        temp.globalY -= tempSpeed;
    if (keyStates[SDL_SCANCODE_D] || keyStates[SDL_SCANCODE_RIGHT])
        temp.globalX += tempSpeed;
    if (keyStates[SDL_SCANCODE_S] || keyStates[SDL_SCANCODE_DOWN])
        temp.globalY += tempSpeed;
    if (keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_LEFT])
        temp.globalX -= tempSpeed;

    temp.tileGX = floor(temp.globalX / default_TILESIZE);
    temp.tileGY = floor(temp.globalY / default_TILESIZE);
    playerPosition = temp;

    // if (WorldGenerator.getTerrainNoise(temp.tileGX, temp.tileGY) >= .45)
    // {
    //     playerPosition = temp;
    //     return true;
    // }

    return true; // false;
}

void View::updateVisibleNPCs()
{
    visibleNPCindices.clear();
    // for (auto npc_index : Factory.npc_destroy_queue)
    // {
    //     Factory.npc_pool.erase(Factory.npc_pool.begin() + npc_index);
    // }
    // Factory.npc_destroy_queue.clear();
    

    // for (Position &npcPosition : Factory.npc_pool)
    for (auto &npc : Factory.npc_pool)
    {
        int npcID = npc.first;
        Position &npcPosition = npc.second.position;

        if (npcPosition.tileGX >= bounds[0][0] - 1 && npcPosition.tileGX <= bounds[1][0] + 1)
        {
            if (npcPosition.tileGY >= bounds[0][1] - 1 && npcPosition.tileGY <= bounds[2][1] + 1)
            {

                // int index = &npcPosition - &Factory.npc_pool[0];
                
                npcPosition.screenX = (-xmod + ((npcPosition.tileGX - bounds[0][0]) * TILESIZE)) +
                                      (static_cast<float>(npcPosition.globalX % default_TILESIZE) /
                                       static_cast<float>(default_TILESIZE)) *
                                          TILESIZE;

                npcPosition.screenY = (-ymod + ((npcPosition.tileGY - bounds[0][1]) * TILESIZE)) +
                                      (static_cast<float>(npcPosition.globalY % default_TILESIZE) /
                                       static_cast<float>(default_TILESIZE)) *
                                          TILESIZE;

                if (positionsCollide(playerPosition, npcPosition))
                {
                    // playerPosition.colliding = true;
                    // // printf("NPC %d collided with player\n", index);
                    // Life &npcLife = Factory.npc_life_pool[index];
                    // if (npcLife.health > 0)
                    // {
                    //     npcLife.health -= 1;
                    // }
                    // else
                    // {
                    //     // printf("NPC %d died\n", index);
                    //     // Factory.npc_pool.erase(Factory.npc_pool.begin() + index);
                    //     Factory.npc_destroy_queue.push_back(index);
                    //     continue;
                    //     // npcPosition.tileGX = -1;
                    //     // npcPosition.tileGY = -1;
                    // }
                }
                else if (playerPosition.colliding)
                {
                    playerPosition.colliding = false;
                }

                visibleNPCindices.push_back(npcID);
                // npcPosition.globalX += WorldGenerator.getTerrainCubic(npcPosition.tileGX, deltaTime) * 1;
                // npcPosition.globalY += WorldGenerator.getTerrainCubic(npcPosition.tileGY, deltaTime) * 1;
            }
        }
    }
}

void View::updateRTCPlayer(int id, int x, int y, int tx, int ty)
{
    //check if id in player map
    if (RTCPlayers.find(id) == RTCPlayers.end())
    {
        //add id to player map
        RTCPlayers.insert(std::pair<int, Position>(id, Position{x, y, tx, ty}));
    }
    else
    {
        //update id in player map
        RTCPlayers[id].globalX = x;
        RTCPlayers[id].globalY = y;
        RTCPlayers[id].tileGX = tx;
        RTCPlayers[id].tileGY = ty;
        // printf("RTC player %d updated\n", id);
    }

    if (tx >= bounds[0][0] - 1 && tx <= bounds[1][0] + 1) {
        if (ty >= bounds[0][1] - 1 && ty <= bounds[2][1] + 1) {
            RTCPlayers[id].screenX = (-xmod + ((tx - bounds[0][0]) * TILESIZE)) +
                    (
                        static_cast<float>(x % default_TILESIZE) /
                        static_cast<float>(default_TILESIZE)
                    ) * TILESIZE;

            RTCPlayers[id].screenY = (-ymod + ((ty - bounds[0][1]) * TILESIZE)) +
                    (
                        static_cast<float>(y % default_TILESIZE) /
                        static_cast<float>(default_TILESIZE)
                    ) * TILESIZE;

            visibleRTCindices.push_back(id);
        }
    }
}

void View::removeRTCPlayer(int id) {
    RTCPlayers.erase(id);
}

bool View::positionsCollide(Position a, Position b)
{
    float asize = a.size;
    float bsize = b.size;
    // simple 2d box collisions
    if (a.globalX + asize > b.globalX && a.globalX < b.globalX + bsize)
    {
        if (a.globalY + asize > b.globalY && a.globalY < b.globalY + bsize)
        {
            return true;
        }
    }
    return false;
}
