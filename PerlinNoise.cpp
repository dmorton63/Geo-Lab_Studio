#include "PerlinNoise.h"
#include <cmath>
#include <algorithm>
#include <random>

PerlinNoise::PerlinNoise(unsigned int seed) {
    reseed(seed);
}

void PerlinNoise::reseed(unsigned int seed) {
    permutation.resize(512);
    
    // Fill with 0-255
    std::vector<int> p(256);
    for (int i = 0; i < 256; ++i) {
        p[i] = i;
    }
    
    // Shuffle using seed
    std::mt19937 gen(seed);
    std::shuffle(p.begin(), p.end(), gen);
    
    // Duplicate for wrapping
    for (int i = 0; i < 256; ++i) {
        permutation[i] = permutation[i + 256] = p[i];
    }
}

float PerlinNoise::fade(float t) const {
    // Improved fade: 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float PerlinNoise::lerp(float t, float a, float b) const {
    return a + t * (b - a);
}

float PerlinNoise::grad(int hash, float x, float y) const {
    // Convert hash to gradient direction
    int h = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

float PerlinNoise::noise(float x, float y) const {
    // Find unit grid cell containing point
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    
    // Get relative position within cell
    x -= std::floor(x);
    y -= std::floor(y);
    
    // Compute fade curves
    float u = fade(x);
    float v = fade(y);
    
    // Hash coordinates of 4 corners
    int aa = permutation[permutation[X] + Y];
    int ab = permutation[permutation[X] + Y + 1];
    int ba = permutation[permutation[X + 1] + Y];
    int bb = permutation[permutation[X + 1] + Y + 1];
    
    // Blend results from 4 corners
    float result = lerp(v,
        lerp(u, grad(aa, x, y), grad(ba, x - 1, y)),
        lerp(u, grad(ab, x, y - 1), grad(bb, x - 1, y - 1))
    );
    
    return result;
}

float PerlinNoise::fbm(float x, float y, int octaves, float persistence, float lacunarity) const {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; ++i) {
        total += noise(x * frequency, y * frequency) * amplitude;
        
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return total / maxValue;
}

float PerlinNoise::ridgedNoise(float x, float y, int octaves, float persistence, float lacunarity) const {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; ++i) {
        float n = noise(x * frequency, y * frequency);
        n = 1.0f - std::abs(n);
        n = n * n;
        
        total += n * amplitude;
        
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return total / maxValue;
}

float PerlinNoise::billowyNoise(float x, float y, int octaves, float persistence, float lacunarity) const {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; ++i) {
        float n = noise(x * frequency, y * frequency);
        n = std::abs(n);
        
        total += n * amplitude;
        
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    
    return total / maxValue;
}
