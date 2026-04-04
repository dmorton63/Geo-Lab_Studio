#pragma once
#include <vector>
#include <cstdint>

class PerlinNoise {
public:
    PerlinNoise(unsigned int seed = 0);
    
    // Basic 2D Perlin noise (-1 to 1)
    float noise(float x, float y) const;
    
    // Fractional Brownian Motion (multiple octaves)
    float fbm(float x, float y, int octaves, float persistence, float lacunarity) const;
    
    // Different noise variations
    float ridgedNoise(float x, float y, int octaves, float persistence, float lacunarity) const;
    float billowyNoise(float x, float y, int octaves, float persistence, float lacunarity) const;
    
    void reseed(unsigned int seed);

private:
    std::vector<int> permutation;
    
    float fade(float t) const;
    float lerp(float t, float a, float b) const;
    float grad(int hash, float x, float y) const;
};
