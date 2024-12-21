#include <bits/stdc++.h>
#include "SomeFunc.hpp"
using namespace std;



template <int n, int k> class Fixed
{
  public:
  static_assert(n == 8|| n == 16 || n == 32 || n == 64, "n must be 8, 16, 32 or 64");
    using type = conditional_t<(n == 8), int8_t, conditional_t<(n == 16), int16_t, conditional_t<(n == 32), int32_t, conditional_t<(n == 64), int64_t, void>>>>;


    operator double() const
    {
        return v / (double)(1 << k);
    }
    operator float() const
    {
        return v / (float)(1 << k);
    }
    constexpr Fixed(int v) : v(v << k)
    {
    }
    constexpr Fixed(float f) : v(f * (1 << k))
    {
    }
    constexpr Fixed(double f) : v(f * (1 << k))
    {
    }
    constexpr Fixed() : v(0)
    {
    }
    template <typename T>
    constexpr Fixed(T x) : v(to_double<T>(x) * (1 << k))
    {
    }
    static constexpr Fixed from_raw(type x)
    {
        Fixed ret;
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

    static constexpr Fixed get_epsilon()
    {
        return Fixed::from_raw(4);
    }

    static constexpr Fixed get_infinity()
    {
        return Fixed::from_raw(std::numeric_limits<type>::max());
    }

    auto operator<=>(const Fixed &) const = default;
    bool operator==(const Fixed &) const = default;

    template <typename T>
    Fixed &operator=(T x)
    {
        v = to_double<T>(x) * (1 << k);
        return *this;
    }

    static constexpr int Left = n;
    static constexpr int Right = k;
    type v;
};


template<int n, int k>
Fixed<n, k> operator+(Fixed<n, k> a, Fixed<n, k> b) {
    return Fixed<n, k>::from_raw(a.v + b.v);
}
template<typename T, int n, int k>
Fixed<n, k> operator+(Fixed<n, k> a, T b) {
    return Fixed<n, k>::from_raw(a.v + Fixed<n, k>(to_double<T>(b)).v);
}

template<typename T, int n, int k>
Fixed<n, k> operator+(T a, Fixed<n, k> b) {
    return Fixed<n, k>::from_raw(b.v + Fixed<n, k>(to_double<T>(a)).v);
}

template<int n, int k>
Fixed<n, k> operator-(Fixed<n, k> a, Fixed<n, k> b) {
    return Fixed<n, k>::from_raw(a.v - b.v);
}

template<typename T, int n, int k>
Fixed<n, k> operator-(Fixed<n, k> a, T b) {
    return Fixed<n, k>::from_raw(a.v - Fixed<n, k>(to_double<T>(b)).v);
}
template<typename T, int n, int k>
Fixed<n, k> operator-(T a, Fixed<n, k> b) {
    return Fixed<n, k>::from_raw(Fixed<n, k>(to_double<T>(a)).v - b.v);
}

template<int n, int k>
Fixed<n, k> operator*(Fixed<n, k> a, Fixed<n, k> b) {
    return Fixed<n, k>::from_raw(((int64_t) a.v * b.v) >> k);
}
template<typename T, int n, int k>
Fixed<n, k> operator*(Fixed<n, k> a, T b) {
    return Fixed<n, k>::from_raw(((int64_t) a.v * Fixed<n, k>(to_double<T>(b)).v) >> k);
}
template<typename T, int n, int k>
Fixed<n, k> operator*(T a, Fixed<n, k> b) {
    return Fixed<n, k>::from_raw(((int64_t) b.v * Fixed<n, k>(to_double<T>(a)).v) >> k);
}

template<int n, int k>
Fixed<n, k> operator/(Fixed<n, k> a, Fixed<n, k> b) {
    return Fixed<n, k>::from_raw(((int64_t) a.v << k) / b.v);
}
template<typename T, int n, int k>
Fixed<n, k> operator/(Fixed<n, k> a, T b) {
    return Fixed<n, k>::from_raw(((int64_t) a.v << k) / Fixed<n, k>(to_double<T>(b)).v);
}

template<typename T, int n, int k>
Fixed<n, k> operator/(T a, Fixed<n, k> b) {
    return Fixed<n, k>(to_double<T>(a)) / b;
}

template<int n, int k>
Fixed<n, k> &operator+=(Fixed<n, k> &a, Fixed<n, k> b) {
    a = a + b;
    return a;
}
template<typename T, int n, int k>
Fixed<n, k> &operator+=(Fixed<n, k> &a, T b) {
    a = a + Fixed<n, k>(to_double<T>(b));
    return a;
}

template<typename T, int n, int k>
T &operator+=(T &a, Fixed<n, k> b) {
    a = a + static_cast<T>(b);
    return a;
}
template<int n, int k>
Fixed<n, k> &operator*=(Fixed<n, k> &a, Fixed<n, k> b) {
    a = a * b;
    return a;
}
template<typename T, int n, int k>
Fixed<n, k> &operator*=(Fixed<n, k> &a, T b) {
    a = a * Fixed<n, k>(to_double<T>(b));
    return a;
}

template<typename T, int n, int k>
T &operator*=(T &a, Fixed<n, k> b) {
    a = a * static_cast<T>(b);
    return a;
}
template<int n, int k>
Fixed<n, k> &operator-=(Fixed<n, k> &a, Fixed<n, k> b) {
    a = a - b;
    return a;
}
template<typename T, int n, int k>
Fixed<n, k> &operator-=(Fixed<n, k> &a, T b) {
    a = a - Fixed<n, k>(to_double<T>(b));
    return a;
}

template<typename T, int n, int k>
T &operator-=(T &a, Fixed<n, k> b) {
    a = a - static_cast<T>(b);
    return a;
}
template<int n, int k>
Fixed<n, k> &operator/=(Fixed<n, k> &a, Fixed<n, k> b) {
    a = a / b;
    return a;
}
template<typename T, int n, int k>
Fixed<n, k> &operator/=(Fixed<n, k> &a, T b) {
    a = a / Fixed<n, k>(to_double<T>(b));
    return a;
}

template<typename T, int n, int k>
T &operator/=(T &a, Fixed<n, k> b) {
    a = a / static_cast<T>(b);
    return a;
}
template<int n, int k>
Fixed<n, k> abs(Fixed<n, k> x) {
    if (x.v < 0) {
        x.v = -x.v;
    }
    return x;
}

template<int n, int k>
ostream &operator<<(ostream &out, Fixed<n, k> x) {
    return out << x.v / (double) (1 << 16);
}