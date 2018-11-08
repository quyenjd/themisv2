/** themisv2 Project (compiled with MinGW - GNU C++11)
    ---
    Copyright (C) 2017 @quyenjd

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.ved.
    ---
    This header is a part of themisv2 Project.
    It contains everything you need to create random tests with useful random functions.
**/
#ifndef __THEMISV2_RANDOMER__
#define __THEMISV2_RANDOMER__

#include "themisv2.h"

#define rank __rank__

/* A struct for saving edges. */
struct edge {
    int u, v;
    edge() {}
    edge (int _u, int _v): u(_u), v(_v) {}
};

typedef vector<edge> ved;

/* Mersenne Twister Engine using seeds from chrono clock! */
mt19937_64 gen(chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1));

/* Some global variables. */
int id, test, subtask;

/* I preferably use uniform_int_distribution for probablities random! */

/* Random int in [a, b]. */
template<typename T>
inline T random_int (T a, T b) {
    if (a < 0 || a > b)
        halt(crash, "Randomer: Wrong arguments for calling random!");
    uniform_int_distribution<T> dis(a, b);
    return dis(gen);
}

/* Random real in [a, b]. */
inline double random_real (double a, double b) {
    if (a < 0 || a > b)
        halt(crash, "Randomer: Wrong arguments for calling random!");
    uniform_real_distribution<double> dis(a, b);
    return dis(gen);
}

/* Reverse some edges. */
void finalize (ved& r) {
    shuffle(ALL(r), gen);
    for (int i = 0; i < (int)r.size(); ++i)
        if (random_int(0, 1))
            swap(r[i].u, r[i].v);
}

/* I'm using C++11's shuffle and easy random distribution for shuffling numbers from 1 to n. */
vi random_permutation (int n) {
    if (n < 1)
        halt(crash, "Randomer: Wrong parameters for permutations!");
    vi r;
    for (int i = 1; i <= n; ++i)
        r.pb(i);
    shuffle(ALL(r), gen);
    return r;
}

/* I'm using a basic dsu-based tree random algorithm by dividing two groups of
   vertices following tree rules.
*/
ved random_tree (int n) {
    if (n < 1)
        halt(crash, "Randomer: Wrong parameters for trees!");
    vi dst = random_permutation(n), src;
    ved r;
    src.pb(dst.back());
    dst.pop_back();
    while (!dst.empty()) {
        int a = src[random_int(0, (int)src.size() - 1)],
            b = dst.back();
        dst.pop_back();
        r.pb(edge(a, b));
        src.pb(b);
    }
    finalize(r);
    return r;
}

/* Just random edges. */
ved random_graph (int n, int m, bool loop = 1) {
    if (n < 1)
        halt(crash, "Randomer: Wrong parameters for graphs!");
    ved r;
    while (m--) {
        int x = random_int(1, n),
            y = random_int(1, n);
        while (x == y && !loop)
            x = random_int(1, n), y = random_int(1, n);
        r.pb(edge(x, y));
    }
    finalize(r);
    return r;
}

/* First, random a tree.
   Second, random some other edges.
*/
ved random_connected_graph (int n, int m, bool loop = 1) {
    if (m < n - 1)
        halt(crash, "Randomer: Connected graphs cannot have less than vertices-1 edges!");
    ved r = random_tree(n);
    for (edge p: random_graph(n, m - n + 1, loop))
        r.pb(p);
    finalize(r);
    return r;
}

/* Just some basic ideas.
   First, I divide vertices by random ranking.
   Second, I draw edges from a random vertex to another random vertex having lower ranking.
*/
ved random_dag (int n, int m) {
    if (n < 1)
        halt(crash, "Randomer: Wrong parameters for DAGs!");
    vector<vi> rank(1);
    ved r;
    int mrank = 1;
    rank[0].pb(1);
    for (int i = 2; i <= n; ++i) {
        int t = random_int(0, mrank);
        if (t == mrank) {
            rank.pb(vi(1, i));
            ++mrank;
        } else
            rank[t].pb(i);
    }
    while (m--) {
        int x = 1, y = 1;
        while (x == y) {
            x = random_int(0, mrank - 1);
            y = random_int(0, mrank - 1);
        }
        if (x > y)
            swap(x, y);
        int u = rank[x][random_int(0, (int)rank[x].size() - 1)],
            v = rank[y][random_int(0, (int)rank[y].size() - 1)];
        r.pb(edge(u, v));
    }
    finalize(r);
    return r;
}

/* Divide vertices into two groups and draw random edges. */
ved random_bipartite_graph (int n, int m) {
    if (n < 1 || (n == 1 && m > 0))
        halt(crash, "Randomer: Wrong parameters for bipartite graphs!");
    if (n == 1)
        return ved(0);
    vi side[2];
    ved r;
    side[0].pb(1);
    side[1].pb(2);
    for (int i = 3; i <= n; ++i)
        side[random_int(0, 1)].pb(i);
    while (m--)
        r.pb(edge(side[0][random_int(0, (int)side[0].size() - 1)],
                  side[1][random_int(0, (int)side[1].size() - 1)]));
    finalize(r);
    return r;
}

/* Just random chars and join to one string. Later, I use C++11's shuffle to shuffle it.
   You give me a vector containing pair of numbers [l, r] that spectify your ranges of chars.
   I only accept ASCII chars.
*/
string random_string (int length, vp t) {
    bool unused[256];
    memset(unused, 1, sizeof unused);
    vi src;
    string r;
    for (int i = 0; i < (int)t.size(); ++i) {
        if (t[i].fi > t[i].se || t[i].fi < 0 || t[i].se < 0 || t[i].fi > 255 || t[i].se > 255)
            halt(crash, "Randomer: Wrong parameters for strings!");
        for (int j = t[i].fi; j <= t[i].se; ++j)
            unused[j] = 0;
    }
    for (int i = 0; i < 256; ++i)
        if (!unused[i])
            src.pb(i);
    while (length--) {
        char x = src[random_int(0, (int)src.size() - 1)];
        r += x;
    }
    shuffle(ALL(r), gen);
    return r;
}

/* Just random ints then convert to chars and join to one string. */
string random_bignum (int length, int leading_zeroes = 0) {
    if (leading_zeroes > length)
        halt(crash, "Randomer: Wrong parameters for big integers!");
    string r;
    for (int i = 0; i < leading_zeroes; ++i)
        r += '0';
    r += random_int(48, 57);
    r += random_string(length - leading_zeroes - 1, {{'0', '9'}});
    return r;
}

/* Just random an arbitrary sequence then sort it increasing. */
template<typename T>
vector<T> random_increasing_seq (int n, T min_value, T max_value, bool rev = 0) {
    vector<T> r;
    for (int i = 0; i < n; ++i)
        r.pb(random_int(min_value, max_value) * (1 - 2 * rev));
    sort(ALL(r));
    for (int i = 0; i < n; ++i)
        r[i] = llabs(r[i]);
    return r;
}

/* Usage: randomer.exe [file input] [id] [test] [subtask] */
void regis_randomer (int argc, char* argv[]) {
    if (argc < 5)
        halt(crash, "Randomer: Wrong randomer usage!");

    id      = atoi(argv[2]);
    test    = atoi(argv[3]);
    subtask = atoi(argv[4]);

    if (id < 0 || id >= test)
        halt(crash, "Randomer: Conflict id and test!");
    if (subtask < 0)
        halt(crash, "Randomer: Subtask invalid!");
    freopen(argv[1], "w", stdout);
}

#endif // __THEMISV2_RANDOMER__
