#pragma once

#include <net/client.h>
#include <net/publisher.h>

namespace game
{
class game
{
public:
    explicit game(net::publisher& publisher)
    : publisher_(publisher)
    {
    }

    void on_open(net::client& client)
    {
        publisher_.publish_to_client(client, "game::on_open");
    }

    void on_message(net::client& client, const std::string& message)
    {
        // publisher_.publish_to_topic("user", "publish via topic");
        publisher_.publish_to_client(client, message);
    }
    
    void on_close(net::client& client, const std::string& message)
    {
        // publisher_.publish_to_client(client, message); // EEEE?????
    }

private:
    net::publisher& publisher_;
};
} // namespace game