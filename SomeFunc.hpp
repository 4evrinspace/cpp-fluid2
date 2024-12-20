
#pragma once
#include <type_traits>
#include <random>
using namespace std;
mt19937 rnd(1337);


template< typename T>
double to_double(T x)
{
    if constexpr (is_floating_point<T>::value)
    {
        return x;
    }
    else
    {
        return x.v;;
    }
    
}


template< typename T>
T random01()
{
    return T(rnd()/double(rnd.max())); 
}