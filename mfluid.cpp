#include "FastFixed.hpp"
#include "Fixed.hpp"
#include "Simulator.hpp"
#include "SomeFunc.hpp"
#include <bits/stdc++.h>
using namespace std;

#define pp pair<int, int>
#ifndef TYPES
#define TYPES FLOAT,FIXED(31,17),FAST_FIXED(25, 11),FIXED(32, 16),DOUBLE,FAST_FIXED(32, 16)
#endif

#ifndef SIZES
#define SIZES S(36,84), S(100,100)
#endif



// Честно, нам не объясняли ничего про дефайны, метапрограммирование, поэтому прошу простить, пришлось быстро
// разбираться в этом всём Но могу ответить на любые вопросы по данному коду, я понял как он работает
// Вроде.........................................

string p_type;
string v_type;
string v_flow_type;
string field_file;
ifstream input;
pair<int, int> fsize;
int ticks_for_save = -1;


template<typename P, typename V, typename VF, int N, int M>
bool run_simulation()
{
    Simulator<P, V, VF, N, M> sim(input, fsize.first, fsize.second);
    sim.run(ticks_for_save);
    return true;
}


pair<int, int> parse_size(const string &size_str)
{
    if (size_str[0] == 'S' && size_str[1] == '(')
    {
        throw std::runtime_error("Size pair must start with S(");
    }

    size_t start = 2;
    size_t comma = size_str.find(',', start);
    if (comma == string::npos)
    {
        throw std::runtime_error("Invalid size format: missing comma");
    }

    size_t end = size_str.find(')', comma);
    if (end == string::npos)
    {
        throw std::runtime_error("Invalid size format: missing )");
    }

    int n = stoi(size_str.substr(start, comma - start));
    int m = stoi(size_str.substr(comma + 1, end - comma - 1));

    return {n, m};
}

template <pair<int, int>... Sizes> 
struct SizesList
{
    static constexpr size_t size = sizeof...(Sizes);
    template <size_t I> 
    static constexpr pair<int, int> get()
    {
        constexpr pair<int, int> arr[] = {Sizes...};
        return arr[I];
    }
};
#define S(N,M) pair<int, int>(N, M)
using MySizeList = SizesList<SIZES>;
#undef S

template <typename List, size_t I = 0> 
constexpr bool matches_size(const pair<int, int> &size)
{
    if (I > List::size)
        return false;
    return (List::template get<I>() == size);
}




template <typename T> 
struct is_fixed_helper : std::false_type
{
};

template <size_t N, size_t K> 
struct is_fixed_helper<Fixed<N, K>> : std::true_type
{
};

template <typename T> 
inline constexpr bool is_fixed_v = is_fixed_helper<T>::value;

template <typename T> 
struct is_fast_fixed_helper : std::false_type
{
};

template <size_t N, size_t K> 
struct is_fast_fixed_helper<FastFixed<N, K>> : std::true_type
{
};

template <typename T> 
inline constexpr bool is_fast_fixed_v = is_fast_fixed_helper<T>::value;



pair<int, int> parse_fixed_params(const string &type)
{
    size_t start = type.find('(') + 1;
    size_t comma = type.find(',', start);
    size_t end = type.find(')', comma);

    int N = stoi(type.substr(start, comma - start));
    int K = stoi(type.substr(comma + 1, end - comma - 1));
    return {N, K};
}

template <typename T> 
static bool matches_type(const string &type)
{
    if constexpr (std::is_same_v<T, float>)
    {
        return type == "FLOAT";
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        return type == "DOUBLE";
    }
    else if constexpr (is_fixed_v<T>)
    {
        if (!type.starts_with("FIXED("))
            return false;
        auto [bits, frac] = parse_fixed_params(type);
        return bits == T::Left && frac == T::Right;
    }
    else if constexpr (is_fast_fixed_v<T>)
    {
        if (!type.starts_with("FAST_FIXED("))
            return false;
        auto [bits, frac] = parse_fixed_params(type);
        return bits == T::Left && frac == T::Right;
    }
    return false;
}

template <typename... Types> 
struct TypesList
{
    static constexpr size_t size = sizeof...(Types);
    template <size_t I> 
    using type_at = typename std::tuple_element<I, std::tuple<Types...>>::type;
};

template <typename AllowedTypes, typename SelectedTypes> 
struct TypeSelector
{
    template <typename... Selected> 
    static bool try_combinations()
    {
        return try_all_p_types<0>();
    }

  private:
    template <size_t I> 
    static bool try_all_p_types()
    {
        if constexpr (I >= AllowedTypes::size)
        {
            return false;
        }
        else
        {
            using P = typename AllowedTypes::template type_at<I>;
            return try_with_p_type<P>() || try_all_p_types<I + 1>();
        }
    }

    template <typename P> 
    static bool try_with_p_type()
    {
        if (!matches_type<P>(p_type))
            return false;
        return try_all_v_types<P, 0>();
    }

    template <typename P, size_t I> 
    static bool try_all_v_types()
    {
        if constexpr (I >= AllowedTypes::size)
        {
            return false;
        }
        else
        {
            using V = typename AllowedTypes::template type_at<I>;
            return try_with_v_type<P, V>() || try_all_v_types<P, I + 1>();
        }
    }

    template <typename P, typename V> 
    static bool try_with_v_type()
    {
        if (!matches_type<V>(v_type))
            return false;
        return try_all_vf_types<P, V, 0>();
    }

    template <typename P, typename V, size_t I> 
    static bool try_all_vf_types()
    {
        if constexpr (I >= AllowedTypes::size)
        {
            return false;
        }
        else
        {
            using VF = typename AllowedTypes::template type_at<I>;
            return try_with_vf_type<P, V, VF>() || try_all_vf_types<P, V, I + 1>();
        }
    }

    template <typename P, typename V, typename VF> 
    static bool try_with_vf_type()
    {
        if (!matches_type<VF>(v_flow_type))
            return false;
        return try_all_size_find<P, V, VF, 0>();
    }

    template <typename P, typename V, typename VF, size_t I> 
    static bool try_all_size_find()
    {
        if constexpr (I >= MySizeList::size)
        {
            return run_simulation<P, V, VF, 0, 0>();
        }
        else
        {
            return try_with_size_find<P, V, VF, I>() || try_all_size_find<P, V, VF, I + 1>();
        }
    }

    template <typename P, typename V, typename VF, size_t I> 
    static bool try_with_size_find()
    {
        if (matches_size<MySizeList, I>(fsize))
        {
            return run_simulation<P, V, VF, MySizeList::get<I>().first, MySizeList::get<I>().second>();
        }
        else
        {
            return false;
        }
    }
};





template <typename... Types> bool try_all_type_combinations()
{
    return TypeSelector<TypesList<Types...>, TypesList<>>::try_combinations();
}

bool start_simulation()
{
    try
    {

#define FLOAT float
#define DOUBLE double
#define FIXED(N, K) Fixed<N, K>
#define FAST_FIXED(N, K) FastFixed<N, K>
        if (!try_all_type_combinations<TYPES>())
        {
            throw std::runtime_error("No type combination was found ");
        }
        return true;
    }
    catch (const std::exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return false;
    }
}

string get_arg(string to_find, int argc, char **argv)
{
    for (int i = 1; i < argc - 1; ++i)
    {
        if (argv[i] == to_find)
        {
            return argv[i + 1];
        }
    }
    throw std::runtime_error("Argument " + to_find + " not found");
}

int main(int argc, char **argv)
{
#ifndef TYPES
    throw std::runtime_error("No types defined");
#endif
#ifndef SIZES
    throw std::runtime_error("No sizes defined");
#endif

    p_type = get_arg(string("--p-type"), argc, argv);
    v_type = get_arg(string("--v-type"), argc, argv);
    v_flow_type = get_arg(string("--v-flow-type"), argc, argv);
    string field_file = get_arg(string("--input-file"), argc, argv);
    input.open(field_file);

    
    input >> fsize.first >> fsize.second;
    for (int i = 1; i < argc - 1; ++i)
    {
        if (argv[i] == "--ticks-for-save")
        {
            ticks_for_save = stoi(argv[i + 1]);
        }
    }


    start_simulation();
    input.close();
    return 0;
}