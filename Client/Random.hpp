/*****************************************************************//**
 * \file   Random.hpp
 * \brief  
 * 
 * \author 2018t
 * \date   May 2024
 *********************************************************************/

#pragma once

#include <random>
#include <type_traits>

// Templated function to generate a random number in a range
template<typename T>
T GetRandom(T min, T max) 
{
    static_assert(std::is_arithmetic<T>::value, "GetRandom can only be used with arithmetic types");

    // sets up the random generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // works with ints
    if constexpr (std::is_integral<T>::value) 
    {
        std::uniform_int_distribution<T> dis(min, max);
        return dis(gen);
    }

    // works with floats
    else if constexpr (std::is_floating_point<T>::value)
    {
        std::uniform_real_distribution<T> dis(min, max);
        return dis(gen);
    }
}