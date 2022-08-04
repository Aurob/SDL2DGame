#pragma once
#include <array>
#include <vector>
#include <SDL2/SDL2_gfxPrimitives.h>

struct Vector2 {
    int x, y;
};
struct Vector2f {
    float x, y;
};

//global positions are the main position values
//screen and tile positions are calculated from the global
struct Position {
    int globalX, globalY;
    int tileGX, tileGY; // global
    int screenX, screenY;
    int tileSX, tileSY; // screen
    int size; //.....
    int sizeS; // altered size for zooming
    int direction;
    bool colliding;
};

//All data relating to eh life of an entity
// Health, RegenerationRate, Damage is NegativeRegeneration
struct Life {
    int health;
    int maxHealth;
    int regenerationRate;
    int damage;
    int negativeRegeneration;
};

struct Movement {
    float speed;
    float timer;
    int dx, dy;
    int step;
};

//Defines a superformula
struct Superformula {
    float a, b, m, n1, n2, n3;
    int NP;
    
    std::vector<Vector2f> points;
};

struct Player {
    int id;
};

struct NPC {
    int id;
    Position position;
    Life life;
    Superformula superformula;
};


//Custom hash for int[2] as key in an unordered_map
//https://stackoverflow.com/a/42701911
struct IntTupleHasher {
    std::size_t operator()(const std::array<int, 2>& a) const {
        std::size_t h = 0;

        for (auto e : a) {
            h ^= std::hash<int>{}(e)  + 0x9e3779b9 + (h << 6) + (h >> 2); 
        }
        return h;
    }   
};