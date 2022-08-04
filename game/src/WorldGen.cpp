#include "../includes/WorldGen.hpp"

WorldGen::WorldGen(){
    terrainNoise.SetSeed(time(NULL));
}

float WorldGen::getTerrainSimplex(int i, int j) {
    float n = (terrainNoise.GetSimplexFractal((i), (j)) - -1) / (1 - -1);
    return (terrainNoise.GetSimplexFractal((i)+pow(n, 1.1), (j)+pow(n, 1.1)) - -1) / (1 - -1);
}

float WorldGen::getTerrainCubic(int i, int j) {
    float n = (terrainNoise.GetSimplexFractal((i), (j)) - -1) / (1 - -1);
    return (terrainNoise.GetCubicFractal((i)+pow(n,2), (j)+pow(n,2)) - -1) / (1 - -1);
}


float WorldGen::superformula(float a, float b, float m, float n1, float n2, float n3) {
    float raux = pow(pow(abs(cos(m/4) / a), n2) + pow(abs(sin(m/4) / b), n2), -1/n2);

    float r = pow(abs(raux), (-1 / n1));

    return r / 10;
}

Vector2f WorldGen::superformulaStep(Superformula superformula, int step)
{
    int NP = superformula.NP;
    float m = superformula.m;
    float n1 = superformula.n1;
    float n2 = superformula.n2;
    float n3 = superformula.n3;
    float a = superformula.a;
    float b = superformula.b;

    float phi = M_PI * 2 / NP * step;

    float t1 = cos(m * phi / 4) / 1;
    t1 = abs(t1);
    t1 = pow(t1, n2);

    float t2 = sin(m * phi / 4) / b;
    t2 = abs(t2);
    t2 = pow(t2, n3);

    float r = pow(t1 + t2, 1 / n1);

    return Vector2f{r, phi};

}
