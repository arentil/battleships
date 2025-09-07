#pragma once

#include <game/board.h>
#include <net/client.h>

namespace game
{
enum class match_state
{
    init,
    setup,
    waiting,
    playing,
    finished
};

struct player
{
    explicit player(const net::client& c) : client(c)
    {
    }

    net::client client;
    board board;
};

enum class player_turn
{
    none,
    p1,
    p2
};

struct match
{
    match(const std::string& room_name_str, const net::client& c1, const net::client& c2, player_turn new_match_player_turn = player_turn::none,
          match_state new_match_state = match_state::init)
    : room_name(room_name_str),
      player_1(c1),
      player_2(c2),
      current_player_turn(new_match_player_turn),
      state(new_match_state)
    {
    }

    std::string room_name;
    player player_1;
    player player_2;

    player_turn current_player_turn = player_turn::none;
    match_state state = match_state::init;
};
} // namespace game