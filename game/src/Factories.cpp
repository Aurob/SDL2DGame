#include "../includes/Factories.hpp"

Factories::Factories()
{
    // std::unordered_map<Position, Life> npc_pool{};

    // Temporary NPCs
    //  assign a global position to each NPC
    int maxGlobalX = 10000;
    int maxGlobalY = 10000;
    // for(int i = 0; i < 413; i++) {
    //     int x = rand() % maxGlobalX - maxGlobalX;
    //     int y = rand() % maxGlobalY - maxGlobalY;
    //     makeNPC(x, y);
    // }

    int NP = 100;
    float phi;
    float r, t1, t2;
    Superformula superformula{1, 1, 39.6, 6.612, 7.138, -6.250, NP};
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

            int tX = static_cast<int>(x) * 5;
            int tY = static_cast<int>(y) * 5;
            makeNPC(tX, tY);
        }
    }
}

// Creates an NPC entity at a specified global position
void Factories::makeNPC(int globalX, int globalY)
{
    Position npcPosition{};
    Life npcLife{};

    npcPosition.globalX = globalX;
    npcPosition.globalY = globalY;
    npcPosition.tileGX = floor(globalX / TILESIZE);
    npcPosition.tileGY = floor(globalY / TILESIZE);
    npcPosition.size = (rand() % 10) + 1;

    npcLife.health = 100;

    float low = 0;
    float high = 10;
    float m = -10 + static_cast<float>(rand()) * static_cast<float>(20 - -10) / static_cast<float>(RAND_MAX);
    float n1 = -10 + static_cast<float>(rand()) * static_cast<float>(-10 - -10) / static_cast<float>(RAND_MAX);
    float n2 = -0.5 + static_cast<float>(rand()) * static_cast<float>(17 - -0.5) / static_cast<float>(RAND_MAX);
    float n3 = 2 + static_cast<float>(rand()) * static_cast<float>(20 - 2) / static_cast<float>(RAND_MAX);

    int NP = npcPosition.size + rand() % 150;

    Superformula npcSuperformula{
        1, 1, m, n1, n2, n3, NP
    };
    Vector2f stepPoint;
    for (int i = 0; i < npcSuperformula.NP; i++)
    {
        stepPoint = WorldGenerator.superformulaStep(npcSuperformula, i);
        float r = stepPoint.x;
        float phi = stepPoint.y;
        npcSuperformula.points.push_back(Vector2f{r, phi});
    }
    NPC npc{++npcIndex, npcPosition, npcLife, npcSuperformula};

    npc_pool.insert(std::make_pair(npc.id, npc));
}
