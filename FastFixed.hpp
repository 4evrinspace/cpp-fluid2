


#include <bits/stdc++.h>
#include "SomeFunc.hpp"
using namespace std;



template <int n, int k> class FastFixed
{
  public:
    using type =
        conditional_t<(n <= 8), int_fast8_t,conditional_t<(n <= 16), int_fast16_t,conditional_t<(n <= 32), int_fast32_t, conditional_t<(n <= 64), int_fast64_t, void>>>>;

    operator double() const
    {
        return v / (double)(1 << k);
    }
    operator float() const
    {
        return v / (float)(1 << k);
    }
    constexpr FastFixed(int v) : v(v << k)
    {
    }
    constexpr FastFixed(float f) : v(f * (1 << k))
    {
    }
    constexpr FastFixed(double f) : v(f * (1 << k))
    {
    }
    constexpr FastFixed() : v(0)
    {
    }
    template <typename T>
    constexpr FastFixed(T x) : v(to_double<T>(x) * (1 << k))
    {
    }
    static constexpr FastFixed from_raw(type x)
    {
        FastFixed ret;
        ret.v = x;
        return ret;
    }


    template <typename T>
    auto operator<=>(T x) const
    {
        return v <=> to_double<T>(x) * (1 << k);
    }

    template <typename T> 
    bool operator==(T x) const
    {
        return v == to_double<T>(x) * (1 << k);
    }

    static constexpr FastFixed get_epsilon()
    {
        return FastFixed::from_raw(4);
    }

    static constexpr FastFixed get_infinity()
    {
        return FastFixed::from_raw(std::numeric_limits<type>::max());
    }

    auto operator<=>(const FastFixed &) const = default;
    bool operator==(const FastFixed &) const = default;

    template <typename T>
    FastFixed &operator=(T x)
    {
        v = to_double<T>(x) * (1 << k);
        return *this;
    }
    

    static constexpr int Left = n;
    static constexpr int Right = k;
    type v;
};


template<int n, int k>
FastFixed<n, k> operator+(FastFixed<n, k> a, FastFixed<n, k> b) {
    return FastFixed<n, k>::from_raw(a.v + b.v);
}
template<typename T, int n, int k>
FastFixed<n, k> operator+(FastFixed<n, k> a, T b) {
    return FastFixed<n, k>::from_raw(a.v + FastFixed<n, k>(to_double<T>(b)).v);
}



template<int n, int k>
FastFixed<n, k> operator-(FastFixed<n, k> a, FastFixed<n, k> b) {
    return FastFixed<n, k>::from_raw(a.v - b.v);
}

template<typename T, int n, int k>
FastFixed<n, k> operator-(FastFixed<n, k> a, T b) {
    return FastFixed<n, k>::from_raw(a.v - FastFixed<n, k>(to_double<T>(b)).v);
}


template<int n, int k>
FastFixed<n, k> operator*(FastFixed<n, k> a, FastFixed<n, k> b) {
    return FastFixed<n, k>::from_raw(((int64_t) a.v * b.v) >> k);
}
template<typename T, int n, int k>
FastFixed<n, k> operator*(FastFixed<n, k> a, T b) {
    return FastFixed<n, k>::from_raw(((int64_t) a.v * FastFixed<n, k>(to_double<T>(b)).v) >> k);
}


template<int n, int k>
FastFixed<n, k> operator/(FastFixed<n, k> a, FastFixed<n, k> b) {
    return FastFixed<n, k>::from_raw(((int64_t) a.v << k) / b.v);
}
template<typename T, int n, int k>
FastFixed<n, k> operator/(FastFixed<n, k> a, T b) {
    return FastFixed<n, k>::from_raw(((int64_t) a.v << k) / FastFixed<n, k>(to_double<T>(b)).v);
}




template<int n, int k>
FastFixed<n, k> &operator+=(FastFixed<n, k> &a, FastFixed<n, k> b) {
    a = a + b;
    return a;
}
template<typename T, int n, int k>
FastFixed<n, k> &operator+=(FastFixed<n, k> &a, T b) {
    a = a + FastFixed<n, k>(to_double<T>(b));
    return a;
}


template<int n, int k>
FastFixed<n, k> &operator*=(FastFixed<n, k> &a, FastFixed<n, k> b) {
    a = a * b;
    return a;
}
template<typename T, int n, int k>
FastFixed<n, k> &operator*=(FastFixed<n, k> &a, T b) {
    a = a * FastFixed<n, k>(to_double<T>(b));
    return a;
}


template<int n, int k>
FastFixed<n, k> &operator-=(FastFixed<n, k> &a, FastFixed<n, k> b) {
    a = a - b;
    return a;
}
template<typename T, int n, int k>
FastFixed<n, k> &operator-=(FastFixed<n, k> &a, T b) {
    a = a - FastFixed<n, k>(to_double<T>(b));
    return a;
}


template<int n, int k>
FastFixed<n, k> &operator/=(FastFixed<n, k> &a, FastFixed<n, k> b) {
    a = a / b;
    return a;
}
template<typename T, int n, int k>
FastFixed<n, k> &operator/=(FastFixed<n, k> &a, T b) {
    a = a / FastFixed<n, k>(to_double<T>(b));
    return a;
}


template<int n, int k>
FastFixed<n, k> abs(FastFixed<n, k> x) {
    if (x.v < 0) {
        x.v = -x.v;
    }
    return x;
}

template<int n, int k>
ostream &operator<<(ostream &out, FastFixed<n, k> x) {
    return out << x.v / (double) (1 << 16);
}