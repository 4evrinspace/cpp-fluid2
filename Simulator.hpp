#include <bits/stdc++.h>
#include "SomeFunc.hpp"
using namespace std;

template <typename P_type, typename V_type, typename V_flow_type, int n, int m> 
class Simulator
{
  public:
    static constexpr size_t some_T = 1'000'000;
    static constexpr std::array<pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    static constexpr P_type inf = P_type::get_infinity();
    static constexpr P_type eps = P_type::get_epsilon();
    P_type rho[256];
    P_type p[n][m];
    P_type old_p[n][m];
    int dirs[n][m];

    template <typename T> 
    struct VectorField
    {
        array<T, deltas.size()> v[n][m];

        VectorField() : v()
        {
        }

        T &add(int x, int y, int dx, int dy, T dv)
        {
            return get(x, y, dx, dy) += dv;
        }

        T &get(int x, int y, int dx, int dy)
        {
            size_t i = find(deltas.begin(), deltas.end(), pair(dx, dy)) - deltas.begin();
            // assert(i < deltas.size());
            return v[x][y][i];
        }
    };

    VectorField<V_type> velocity;
    VectorField<V_flow_type> velocity_flow;

    int last_use[n][m];
    int UT;
    char field[n][m + 1];

    Simulator(ifstream& input, int _n, int _m) : velocity(), velocity_flow()
    {
        UT = 0;
         
        
        for (size_t x = 0; x < n; ++x)
        {
            for (size_t y = 0; y < m; ++y)
            {
                input >> field[x][y];
            }
            field[x][m] = '\0';
        }
        
    }
    tuple<P_type, bool, pair<int, int>> propagate_flow(int x, int y, P_type lim)
    {
        last_use[x][y] = UT - 1;
        P_type ret = 0;
        for (auto [dx, dy] : deltas)
        {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT)
            {
                auto cap = velocity.get(x, y, dx, dy);
                auto flow = velocity_flow.get(x, y, dx, dy);
                if (flow == cap)
                {
                    continue;
                }
                auto vp = min(lim, P_type(cap - flow));
                if (last_use[nx][ny] == UT - 1)
                {
                    velocity_flow.add(x, y, dx, dy, vp);
                    last_use[x][y] = UT;
                    return {P_type(vp), 1, {nx, ny}};
                }
                auto [t, prop, end] = propagate_flow(nx, ny, vp);
                ret += t;
                if (prop)
                {
                    velocity_flow.add(x, y, dx, dy, t);
                    last_use[x][y] = UT;
                    return {P_type(t), prop && end != pair(x, y), end};
                }
            }
        }
        last_use[x][y] = UT;
        return {ret, 0, {0, 0}};
    }

    void propagate_stop(int x, int y, bool force = false)
    {
        if (!force)
        {
            bool stop = true;
            for (auto [dx, dy] : deltas)
            {
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) > 0)
                {
                    stop = false;
                    break;
                }
            }
            if (!stop)
            {
                return;
            }
        }
        last_use[x][y] = UT;
        for (auto [dx, dy] : deltas)
        {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT || velocity.get(x, y, dx, dy) > 0)
            {
                continue;
            }
            propagate_stop(nx, ny);
        }
    }

    P_type move_prob(int x, int y)
    {
        P_type sum = 0;
        for (size_t i = 0; i < deltas.size(); ++i)
        {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT)
            {
                continue;
            }
            auto v = velocity.get(x, y, dx, dy);
            if (v < 0)
            {
                continue;
            }
            sum += v;
        }
        return sum;
    }

    struct ParticleParams
    {
        char type;
        P_type cur_p;
        array<V_type, deltas.size()> v;

        void swap_with(Simulator &sim, int x, int y)
        {
            swap(sim.field[x][y], type);
            swap(sim.p[x][y], cur_p);
            swap(sim.velocity.v[x][y], v);
        }
    };

    bool propagate_move(int x, int y, bool is_first)
    {
        last_use[x][y] = UT - is_first;
        bool ret = false;
        int nx = -1, ny = -1;
        do
        {
            array<P_type, deltas.size()> tres;
            P_type sum = 0;
            for (size_t i = 0; i < deltas.size(); ++i)
            {
                auto [dx, dy] = deltas[i];
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] == '#' || last_use[nx][ny] == UT)
                {
                    tres[i] = sum;
                    continue;
                }
                auto v = velocity.get(x, y, dx, dy);
                if (v < 0)
                {
                    tres[i] = sum;
                    continue;
                }
                sum += v;
                tres[i] = sum;
            }

            if (sum == 0)
            {
                break;
            }

            P_type p = random01<P_type>() * sum;
            size_t d = std::ranges::upper_bound(tres, p) - tres.begin();

            auto [dx, dy] = deltas[d];
            nx = x + dx;
            ny = y + dy;
            assert(velocity.get(x, y, dx, dy) > 0 && field[nx][ny] != '#' && last_use[nx][ny] < UT);

            ret = (last_use[nx][ny] == UT - 1 || propagate_move(nx, ny, false));
        } while (!ret);
        last_use[x][y] = UT;
        for (size_t i = 0; i < deltas.size(); ++i)
        {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) < 0)
            {
                propagate_stop(nx, ny);
            }
        }
        if (ret)
        {
            if (!is_first)
            {
                ParticleParams pp{};
                pp.swap_with(*this, x, y);
                pp.swap_with(*this, nx, ny);
                pp.swap_with(*this, x, y);
            }
        }
        return ret;
    }

    void run(int ticks_for_save = -1)
    {
        rho[' '] = 0.01;
        rho['.'] = 1000;
        P_type g = 0.1;
        

        for (size_t x = 0; x < n; ++x)
        {
            for (size_t y = 0; y < m; ++y)
            {
                if (field[x][y] == '#')
                    continue;
                for (auto [dx, dy] : deltas)
                {
                    dirs[x][y] += (field[x + dx][y + dy] != '#');
                }
            }
        }

        for (size_t i = 0; i < some_T; ++i)
        {
            P_type total_delta_p = 0;
            for (size_t x = 0; x < n; ++x)
            {
                for (size_t y = 0; y < m; ++y)
                {
                    if (field[x][y] == '#')
                        continue;
                    if (field[x + 1][y] != '#')
                        velocity.add(x, y, 1, 0, V_type(g));
                }
            }

            for (int x = 0; x < n; ++x)
            {
                for (int y = 0; y < m; ++y)
                {
                    old_p[x][y] = p[x][y];
                }
            }

            for (size_t x = 0; x < n; ++x)
            {
                for (size_t y = 0; y <m; ++y)
                {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy] : deltas)
                    {
                        int nx = x + dx, ny = y + dy;
                        if (field[nx][ny] != '#' && old_p[nx][ny] < old_p[x][y])
                        {
                            auto delta_p = old_p[x][y] - old_p[nx][ny];
                            auto force = delta_p;
                            auto &contr = velocity.get(nx, ny, -dx, -dy);
                            if (contr * rho[(int)field[nx][ny]] >= force)
                            {
                                contr -= force / rho[(int)field[nx][ny]];
                                continue;
                            }
                            force -= contr * rho[(int)field[nx][ny]];
                            contr = 0;
                            velocity.add(x, y, dx, dy, force / rho[(int)field[x][y]]);
                            p[x][y] -= force / P_type(dirs[x][y]);
                            total_delta_p -= force / P_type(dirs[x][y]);
                        }
                    }
                }
            }

            velocity_flow = {};
            bool prop = false;
            do
            {
                UT += 2;
                prop = 0;
                for (size_t x = 0; x < n; ++x)
                {
                    for (size_t y = 0; y < m; ++y)
                    {
                        if (field[x][y] != '#' && last_use[x][y] != UT)
                        {
                            auto [t, local_prop, _] = propagate_flow(x, y, 1);
                            if (t > 0)
                            {
                                prop = 1;
                            }
                        }
                    }
                }
            } while (prop);

            for (size_t x = 0; x < n; ++x)
            {
                for (size_t y = 0; y < m; ++y)
                {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy] : deltas)
                    {
                        auto old_v = velocity.get(x, y, dx, dy);
                        auto new_v = velocity_flow.get(x, y, dx, dy);
                        if (old_v > 0)
                        {
                            assert(new_v <= old_v);
                            velocity.get(x, y, dx, dy) = new_v;
                            auto force = (old_v - new_v) * rho[(int)field[x][y]];
                            if (field[x][y] == '.')
                                force *= P_type(0.8);
                            if (field[x + dx][y + dy] == '#')
                            {
                                p[x][y] += force / P_type(dirs[x][y]);
                                total_delta_p += force / P_type(dirs[x][y]);
                            }
                            else
                            {
                                p[x + dx][y + dy] += force / P_type(dirs[x + dx][y + dy]);
                                total_delta_p += force / P_type(dirs[x + dx][y + dy]);
                            }
                        }
                    }
                }
            }

            UT += 2;
            prop = false;
            for (size_t x = 0; x < n; ++x)
            {
                for (size_t y = 0; y < m; ++y)
                {
                    if (field[x][y] != '#' && last_use[x][y] != UT)
                    {
                        if (random01<P_type>() < move_prob(x, y))
                        {
                            prop = true;
                            propagate_move(x, y, true);
                        }
                        else
                        {
                            propagate_stop(x, y, true);
                        }
                    }
                }
            }

            if (prop)
            {
                cout << "Tick " << i << ":\n";
                for (size_t x = 0; x < n; ++x)
                {
                    cout << field[x] << "\n";
                }
                if (ticks_for_save != -1 && i % ticks_for_save == 0)
                {
                    //save();
                }
            }
        }
    }
};





template <typename P_type, typename V_type, typename V_flow_type> 
class Simulator <P_type, V_type, V_flow_type, 0, 0>
{
  public:
  int n, m;
    static constexpr size_t some_T = 1'000'000;
    static constexpr std::array<pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    static constexpr P_type inf = P_type::get_infinity();
    static constexpr P_type eps = P_type::get_epsilon();
    P_type rho[256];
    P_type** p;
    P_type** old_p;
    int** dirs;

    template <typename T> 
    struct VectorField
    {
        array<T, deltas.size()>** v;

        VectorField() : v()
        {
            v = new array<T, deltas.size()> *[n];
            for (int i = 0; i < n; i++)
            {
                v[i] = new array<T, deltas.size()>[m];
            }
        }

        T &add(int x, int y, int dx, int dy, T dv)
        {
            return get(x, y, dx, dy) += dv;
        }

        T &get(int x, int y, int dx, int dy)
        {
            size_t i = find(deltas.begin(), deltas.end(), pair(dx, dy)) - deltas.begin();
            // assert(i < deltas.size());
            return v[x][y][i];
        }
    };

    VectorField<V_type> velocity;
    VectorField<V_flow_type> velocity_flow;

    int** last_use;
    int UT;
    char** field;

    Simulator(ifstream& input, int n, int m) : velocity(), velocity_flow(), n(n), m(m)
    {
        dirs = new int *[n];
        p = new P_type *[n];
        old_p = new P_type *[n];
        last_use = new int *[n];
        UT = 0;
        field = new char *[n];
        for (int i = 0; i < n; i++)
        {
            dirs[i] = new int[m];
            p[i] = new P_type[m];
            old_p[i] = new P_type[m];
            last_use[i] = new int[m];
            field[i] = new char[m + 1];
        }

        for (size_t x = 0; x < n; ++x)
        {
            for (size_t y = 0; y < m; ++y)
            {
                input >> field[x][y];
            }
            field[x][m] = '\0';
        }
        UT = 0;
    }
    ~Simulator()
    {
        for (int i = 0; i < n; i++)
        {
            delete[] dirs[i];
            delete[] p[i];
            delete[] old_p[i];
            delete[] last_use[i];
            delete[] field[i];
            delete[] velocity.v[i];
            delete[] velocity_flow.v[i];
        }
        delete[] dirs;
        delete[] p;
        delete[] old_p;
        delete[] last_use;
        delete[] field;
        delete[] velocity.v;
        delete[] velocity_flow.v;
    }
    
    tuple<P_type, bool, pair<int, int>> propagate_flow(int x, int y, P_type lim)
    {
        last_use[x][y] = UT - 1;
        P_type ret = 0;
        for (auto [dx, dy] : deltas)
        {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT)
            {
                auto cap = velocity.get(x, y, dx, dy);
                auto flow = velocity_flow.get(x, y, dx, dy);
                if (flow == cap)
                {
                    continue;
                }
                auto vp = min(lim, P_type(cap - flow));
                if (last_use[nx][ny] == UT - 1)
                {
                    velocity_flow.add(x, y, dx, dy, vp);
                    last_use[x][y] = UT;
                    return {P_type(vp), 1, {nx, ny}};
                }
                auto [t, prop, end] = propagate_flow(nx, ny, vp);
                ret += t;
                if (prop)
                {
                    velocity_flow.add(x, y, dx, dy, t);
                    last_use[x][y] = UT;
                    return {P_type(t), prop && end != pair(x, y), end};
                }
            }
        }
        last_use[x][y] = UT;
        return {ret, 0, {0, 0}};
    }

    void propagate_stop(int x, int y, bool force = false)
    {
        if (!force)
        {
            bool stop = true;
            for (auto [dx, dy] : deltas)
            {
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) > 0)
                {
                    stop = false;
                    break;
                }
            }
            if (!stop)
            {
                return;
            }
        }
        last_use[x][y] = UT;
        for (auto [dx, dy] : deltas)
        {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT || velocity.get(x, y, dx, dy) > 0)
            {
                continue;
            }
            propagate_stop(nx, ny);
        }
    }

    P_type move_prob(int x, int y)
    {
        P_type sum = 0;
        for (size_t i = 0; i < deltas.size(); ++i)
        {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT)
            {
                continue;
            }
            auto v = velocity.get(x, y, dx, dy);
            if (v < 0)
            {
                continue;
            }
            sum += v;
        }
        return sum;
    }

    struct ParticleParams
    {
        char type;
        P_type cur_p;
        array<V_type, deltas.size()> v;

        void swap_with(Simulator &sim, int x, int y)
        {
            swap(sim.field[x][y], type);
            swap(sim.p[x][y], cur_p);
            swap(sim.velocity.v[x][y], v);
        }
    };

    bool propagate_move(int x, int y, bool is_first)
    {
        last_use[x][y] = UT - is_first;
        bool ret = false;
        int nx = -1, ny = -1;
        do
        {
            array<P_type, deltas.size()> tres;
            P_type sum = 0;
            for (size_t i = 0; i < deltas.size(); ++i)
            {
                auto [dx, dy] = deltas[i];
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] == '#' || last_use[nx][ny] == UT)
                {
                    tres[i] = sum;
                    continue;
                }
                auto v = velocity.get(x, y, dx, dy);
                if (v < 0)
                {
                    tres[i] = sum;
                    continue;
                }
                sum += v;
                tres[i] = sum;
            }

            if (sum == 0)
            {
                break;
            }

            P_type p = random01<P_type>() * sum;
            size_t d = std::ranges::upper_bound(tres, p) - tres.begin();

            auto [dx, dy] = deltas[d];
            nx = x + dx;
            ny = y + dy;
            assert(velocity.get(x, y, dx, dy) > 0 && field[nx][ny] != '#' && last_use[nx][ny] < UT);

            ret = (last_use[nx][ny] == UT - 1 || propagate_move(nx, ny, false));
        } while (!ret);
        last_use[x][y] = UT;
        for (size_t i = 0; i < deltas.size(); ++i)
        {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) < 0)
            {
                propagate_stop(nx, ny);
            }
        }
        if (ret)
        {
            if (!is_first)
            {
                ParticleParams pp{};
                pp.swap_with(*this, x, y);
                pp.swap_with(*this, nx, ny);
                pp.swap_with(*this, x, y);
            }
        }
        return ret;
    }

    void run(int ticks_for_save = -1)
    {
        rho[' '] = 0.01;
        rho['.'] = 1000;
        P_type g = 0.1;
       

        for (size_t x = 0; x < n; ++x)
        {
            for (size_t y = 0; y < m; ++y)
            {
                if (field[x][y] == '#')
                    continue;
                for (auto [dx, dy] : deltas)
                {
                    dirs[x][y] += (field[x + dx][y + dy] != '#');
                }
            }
        }

        for (size_t i = 0; i < some_T; ++i)
        {
            P_type total_delta_p = 0;
            for (size_t x = 0; x < n; ++x)
            {
                for (size_t y = 0; y < m; ++y)
                {
                    if (field[x][y] == '#')
                        continue;
                    if (field[x + 1][y] != '#')
                        velocity.add(x, y, 1, 0, V_type(g));
                }
            }

            for (int x = 0; x < n; ++x)
            {
                for (int y = 0; y < m; ++y)
                {
                    old_p[x][y] = p[x][y];
                }
            }

            for (size_t x = 0; x < n; ++x)
            {
                for (size_t y = 0; y <m; ++y)
                {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy] : deltas)
                    {
                        int nx = x + dx, ny = y + dy;
                        if (field[nx][ny] != '#' && old_p[nx][ny] < old_p[x][y])
                        {
                            auto delta_p = old_p[x][y] - old_p[nx][ny];
                            auto force = delta_p;
                            auto &contr = velocity.get(nx, ny, -dx, -dy);
                            if (contr * rho[(int)field[nx][ny]] >= force)
                            {
                                contr -= force / rho[(int)field[nx][ny]];
                                continue;
                            }
                            force -= contr * rho[(int)field[nx][ny]];
                            contr = 0;
                            velocity.add(x, y, dx, dy, force / rho[(int)field[x][y]]);
                            p[x][y] -= force / P_type(dirs[x][y]);
                            total_delta_p -= force / P_type(dirs[x][y]);
                        }
                    }
                }
            }

            velocity_flow = {};
            bool prop = false;
            do
            {
                UT += 2;
                prop = 0;
                for (size_t x = 0; x < n; ++x)
                {
                    for (size_t y = 0; y < m; ++y)
                    {
                        if (field[x][y] != '#' && last_use[x][y] != UT)
                        {
                            auto [t, local_prop, _] = propagate_flow(x, y, 1);
                            if (t > 0)
                            {
                                prop = 1;
                            }
                        }
                    }
                }
            } while (prop);

            for (size_t x = 0; x < n; ++x)
            {
                for (size_t y = 0; y < m; ++y)
                {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy] : deltas)
                    {
                        auto old_v = velocity.get(x, y, dx, dy);
                        auto new_v = velocity_flow.get(x, y, dx, dy);
                        if (old_v > 0)
                        {
                            assert(new_v <= old_v);
                            velocity.get(x, y, dx, dy) = new_v;
                            auto force = (old_v - new_v) * rho[(int)field[x][y]];
                            if (field[x][y] == '.')
                                force *= P_type(0.8);
                            if (field[x + dx][y + dy] == '#')
                            {
                                p[x][y] += force / P_type(dirs[x][y]);
                                total_delta_p += force / P_type(dirs[x][y]);
                            }
                            else
                            {
                                p[x + dx][y + dy] += force / P_type(dirs[x + dx][y + dy]);
                                total_delta_p += force / P_type(dirs[x + dx][y + dy]);
                            }
                        }
                    }
                }
            }

            UT += 2;
            prop = false;
            for (size_t x = 0; x < n; ++x)
            {
                for (size_t y = 0; y < m; ++y)
                {
                    if (field[x][y] != '#' && last_use[x][y] != UT)
                    {
                        if (random01<P_type>() < move_prob(x, y))
                        {
                            prop = true;
                            propagate_move(x, y, true);
                        }
                        else
                        {
                            propagate_stop(x, y, true);
                        }
                    }
                }
            }

            if (prop)
            {
                cout << "Tick " << i << ":\n";
                for (size_t x = 0; x < n; ++x)
                {
                    cout << field[x] << "\n";
                }
                if (ticks_for_save != -1 && i % ticks_for_save == 0)
                {
                    //save();
                }
            }
        }
    }
};