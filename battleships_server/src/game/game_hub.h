#pragma once

#include <game/game.h>
#include <net/connection.h>
#include <net/publisher.h>
#include <net/websocket_server.h>
#include <util/thread_pool.h>

namespace game
{
class game_hub
{
public:
    game_hub(util::thread_pool& t_pool, net::websocket_server& ws_server, game& game)
    : t_pool_(t_pool),
      ws_server_(ws_server),
      game_(game)
    {
        ws_server_.on_open(
          [this](net::connection* ws)
          { t_pool_.post([this, c = net::client{ws, ws->getUserData()->id()}]() mutable { game_.on_open(c); }); });

        ws_server_.on_message(
          [this](net::connection* ws, std::string_view message, uWS::OpCode op_code)
          {
              if (op_code != uWS::OpCode::TEXT)
                  return; // log error?

              t_pool_.post(
                [this, c = net::client{ws, ws->getUserData()->id()}, message = std::string(message)]() mutable
                { game_.on_message(c, message); });
          });

        ws_server_.on_close(
          [this](net::connection* ws, std::string_view message)
          {
              t_pool_.post(
                [this, c = net::client{ws, ws->getUserData()->id()}, message = std::string{message}]() mutable
                { game_.on_close(c, message); });
          });
    }

    void run()
    {
        ws_server_.run();
    }

private:
    util::thread_pool& t_pool_;
    net::websocket_server& ws_server_;
    game& game_;
};
} // namespace game