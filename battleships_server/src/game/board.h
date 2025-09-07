#pragma once

namespace game
{
static constexpr int N = 10;
static constexpr int CELLS = N * N;
inline int idx(int r, int c)
{
    return r * N + c;
}
inline int rowFromIdx(int i)
{
    return i / N;
}
inline int colFromIdx(int i)
{
    return i % N;
}

enum class orientation : uint8_t
{
    horizontal,
    vertical
};

enum class ship_type : uint8_t
{
    carrier,
    battleship,
    cruiser,
    submarine,
    destroyer
};

inline size_t ship_size(ship_type s)
{
    switch (s)
    {
    case ship_type::carrier:
        return 5;
    case ship_type::battleship:
        return 4;
    case ship_type::cruiser:
        return 3;
    case ship_type::submarine:
        return 3;
    case ship_type::destroyer:
        return 2;
    default:
        std::unreachable();
    }
}

enum class shot_result : uint8_t
{
    hit,
    miss,
    invalid
};

struct pos
{
    int r, c; // row, column
};

struct ship_info
{
    ship_info(ship_type t, pos a, orientation o) : ship(t), anchor(a), orient(o)
    {
    }

    ship_type ship;
    pos anchor;
    orientation orient;

    std::bitset<CELLS> mask() const
    {
        std::bitset<CELLS> m;
        for (size_t i = 0; i < ship_size(ship); i++)
        {
            int rr = anchor.r + (orient == orientation::vertical ? i : 0);
            int cc = anchor.c + (orient == orientation::horizontal ? i : 0);
            m.set(idx(rr, cc));
        }

        return m;
    }
};

struct board
{
    board()
    {
        fleet.reserve(5);
    }

    std::bitset<100> ships;
    std::bitset<100> shots_fired;
    std::bitset<100> hits_taken;

    std::vector<ship_info> fleet;

    // Is pos in board
    bool in_bounds(const pos p) const
    {
        return p.r >= 0 && p.r < N && p.c >= 0 && p.r < N;
    }

    // Can place ship - will ship \s overlap?
    bool can_place(const ship_info& s) const
    {
        for (size_t i = 0; i < ship_size(s.ship); i++)
        {
            int r = s.anchor.r + (s.orient == orientation::vertical ? i : 0);
            int c = s.anchor.c + (s.orient == orientation::horizontal ? i : 0);
            if (!in_bounds({r, c}))
                return false;
            if (ships.test(idx(r, c)))
                return false;
        }
    }

    bool place_ship(ship_type ship_type, const pos anchor, orientation orient)
    {
        if (fleet.size() >= 5) [[unlikely]] // should not happen, but just in case
            return false;

        ship_info s{ship_type, anchor, orient};
        if (!can_place(s))
            return false;

        fleet.emplace_back(std::move(s));
        auto m = s.mask();
        ships |= m;

        return true;
    }
};

shot_result fire_on_pos(board& attacker, board& defender, pos p)
{
    if (!attacker.in_bounds(p))
        return shot_result::invalid;

    const int i = idx(p.r, p.c);
    if (!attacker.shots_fired.test(i))
        return shot_result::invalid;

    attacker.shots_fired.set(i);
    const bool hit = defender.ships.test(i);
    if (hit)
        defender.hits_taken.set(i); // not sure if this if is valid
    return hit ? shot_result::hit : shot_result::miss;
}

// My shots that his opponent's ships
inline std::bitset<CELLS> my_hits(const board& me, const board& opponent)
{
    return opponent.ships & me.shots_fired;
}

// My shots that missed opponent's shis
inline std::bitset<CELLS> my_misses(const board& me, const board& opponent)
{
    const std::bitset<CELLS> my_shots_that_hit = my_hits(me, opponent);
    return me.shots_fired & ~my_shots_that_hit;
}

inline std::bitset<CELLS> opponent_misses(const board& me, const board& opponent)
{
    return opponent.shots_fired & ~me.ships;
}

// Zrzuca bitset (100 bitów) do małego wektora bajtów (13 bajtów).
inline std::vector<uint8_t> toBytes(const std::bitset<CELLS>& b) {
    std::vector<uint8_t> out((CELLS + 7) / 8);
    for (int i = 0; i < CELLS; ++i)
        if (b.test(i)) out[i >> 3] |= uint8_t(1u << (i & 7));
    return out;
}

// Wczytuje z buforu (rozmiar co najmniej 13 bajtów); nadmiar ignorowany.
inline std::bitset<CELLS> fromBytes(const uint8_t* data, size_t len) {
    std::bitset<CELLS> b;
    const size_t L = std::min(len, (size_t)((CELLS + 7) / 8));
    for (size_t byte = 0; byte < L; ++byte) {
        uint8_t v = data[byte];
        for (int bit = 0; bit < 8; ++bit) {
            size_t i = (byte << 3) + bit;
            if (i >= (size_t)CELLS) break;
            if (v & (1u << bit)) b.set((int)i);
        }
    }
    return b;
}
}; // namespace game