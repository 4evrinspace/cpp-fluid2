
#pragma once
#include <type_traits>
#include <random>
using namespace std;
mt19937 rnd(1337);

template<int n, int k>
class Fixed;

template<int n, int k>
class FastFixed;

template< typename T>
double to_double(T x)
{
    if constexpr (is_floating_point<T>::value || is_same<T, int>::value)
    {
        return x;
    }
    else
    {
        return x.v;;
    }
    
}
template< typename T>
float to_float(T x)
{
    return (float)to_double<T>(x);
    
}


template< typename T>
T random01()
{
    return T(rnd()/double(rnd.max())); 
}

class Float
{
    public:
    template<typename T>
    constexpr Float (T x) : v(to_double<T>(x))
    {
    }

    constexpr Float() : v(0)
    {
    }

    auto operator<=>(const Float &) const = default;
    bool operator==(const Float &) const = default;

    template <typename T> 
    Float &operator=(T x)
    {
        v = to_double(x);
        return *this;
    }

    template <typename T> auto operator<=>(OTHER x) const
    {
        return v <=> to_double(x);
    }

    template <typename OTHER> bool operator==(OTHER x) const
    {
        return v == to_double(x);
    }

    static constexpr Float get_epsilon()
    {
        return Float(4);
    }

    static constexpr Float get_infinity()
    {
        return Float(std::numeric_limits<float>::max());
    }


    float v;

}




template<typename T>
double operator+(double a, T b) {
    return a + to_double<T>(b);
}

template<typename T>
double operator-(double a, T b) {
    return a - to_double<T>(b);
}

template<typename T>
double operator*(double a, T b) {
    return a * to_double<T>(b);
}
template<typename T>
double operator/(double a, T b) {
    return a / to_double<T>(b);
}

template<typename T>
double operator+=(double& a, T b) {
    a = a + to_double<T>(b);
    return a;
}
template<typename T>
double operator*=(double& a, T b) {
    a = a * to_double<T>(b);
    return a;
}
template<typename T>
double operator-=(double& a, T b) {
    a = a - to_double<T>(b);
    return a;
}
template<typename T>
double operator/=(double& a, T b) {
    a = a / to_double<T>(b);
    return a;
}


/*
template<typename T>
float operator+(float a, T b) {
    return a + to_float<T>(b);
}

template<typename T>
float operator-(float a, T b) {
    return a - to_float<T>(b);
}

template<typename T>
float operator*(float a, T b) {
    return a * to_float<T>(b);
}
template<typename T>
float operator/(float a, T b) {
    return a / to_float<T>(b);
}

template<typename T>
float operator+=(float& a, T b) {
    a = a + to_float<T>(b);
    return a;
}
template<typename T>
float operator*=(float& a, T b) {
    a = a * to_float<T>(b);
    return a;
}
template<typename T>
float operator-=(float& a, T b) {
    a = a - to_float<T>(b);
    return a;
}
template<typename T>
float operator/=(float& a, T b) {
    a = a / to_float<T>(b);
    return a;
}
*/