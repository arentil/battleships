#include <game/game_hub.h>
#include <net/connection.h>
#include <net/publisher.h>
#include <net/websocket_server.h>
#include <util/thread_pool.h>

#include <algorithm>
#include <thread>

int main()
{
    const size_t t_pool_size{std::min(4ull, std::thread::hardware_concurrency() - 1ull)};
    util::thread_pool t_pool(t_pool_size);
    net::websocket_server ws_server(9001);
    net::publisher publisher(ws_server.app(), *(uWS::Loop::get()));
    game::game game(publisher);
    game::game_hub game_hub(t_pool, ws_server, game);
    game_hub.run();

    return 0;
}