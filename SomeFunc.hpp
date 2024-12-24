
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
double to_double(const T& x)
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

    template <typename T> 
    auto operator<=>(T x) const
    {
        return v <=> to_double(x);
    }

    template <typename T> 
    bool operator==(T x) const
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

};
class Double
{
    public:
    
    template<typename T>
    constexpr Double (T x) : v(to_double<T>(x))
    {
    }

    constexpr Double() : v(0)
    {
    }

    auto operator<=>(const Double &) const = default;
    bool operator==(const Double &) const = default;

    template <typename T> 
    Double &operator=(T x)
    {
        v = to_double(x);
        return *this;
    }

    template <typename T> 
    auto operator<=>(T x) const
    {
        return v <=> to_double(x);
    }

    template <typename T> 
    bool operator==(T x) const
    {
        return v == to_double(x);
    }

    static constexpr Double get_epsilon()
    {
        return Double(4);
    }

    static constexpr Double get_infinity()
    {
        return Double(std::numeric_limits<double>::max());
    }

    double v;

};




template<typename T>
Double operator+(Double a, T b) {
    return a + to_double<T>(b);
}

template<typename T>
Double operator-(Double a, T b) {
    return a - to_double<T>(b);
}

template<typename T>
Double operator*(Double a, T b) {
    return a * to_double<T>(b);
}
template<typename T>
Double operator/(Double a, T b) {
    return a / to_double<T>(b);
}

template<typename T>
Double& operator+=(Double& a, T b) {
    a = a + to_double<T>(b);
    return a;
}
template<typename T>
Double& operator*=(Double& a, T b) {
    a = a * to_double<T>(b);
    return a;
}
template<typename T>
Double& operator-=(Double& a, T b) {
    a = a - to_double<T>(b);
    return a;
}
template<typename T>
Double& operator/=(Double& a, T b) {
    a = a / to_double<T>(b);
    return a;
}
Double abs(Double x) {
    if (x.v < 0) {
        x.v = -x.v;
    }
    return x;
}


template<typename T>
Float operator+(Float a, T b) {
    return a + to_float<T>(b);
}

template<typename T>
Float operator-(Float a, T b) {
    return a - to_float<T>(b);
}

template<typename T>
Float operator*(Float a, T b) {
    return a * to_float<T>(b);
}
template<typename T>
Float operator/(Float a, T b) {
    return a / to_float<T>(b);
}

template<typename T>
Float& operator+=(Float& a, T b) {
    a = a + to_float<T>(b);
    return a;
}
template<typename T>
Float& operator*=(Float& a, T b) {
    a = a * to_float<T>(b);
    return a;
}
template<typename T>
Float& operator-=(Float& a, T b) {
    a = a - to_float<T>(b);
    return a;
}
template<typename T>
Float& operator/=(Float& a, T b) {
    a = a / to_float<T>(b);
    return a;
}


Float abs(Float x) {
    if (x.v < 0) {
        x.v = -x.v;
    }
    return x;
}