#pragma once
#include <random>

struct Randomizer
{
    static float Real(float min, float max)
    {
        std::random_device rd; //makes different outputs from same input when looped
        std::mt19937 mt(rd()); //random number gen
        std::uniform_real_distribution<float> value(min, max); // makes random floating point values
        return value(mt);
    }

    static int Integer(int min, int max)
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> value(min, max);
        return value(mt);
    }
};