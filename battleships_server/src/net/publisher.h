#pragma once

#include "App.h" // uWebSocket

#include <net/client.h>
#include <net/connection.h>
#include <util/non_owning_ptr.h>

namespace net
{
class publisher
{
public:
    explicit publisher(uWS::App& app, uWS::Loop& loop) : app_(app), loop_(loop)
    {
    }

    void publish_to_topic(const std::string& topic, const std::string& payload)
    {
        loop_.defer([this, t = topic, p = payload]() { app_.publish(t, p, uWS::OpCode::TEXT); });
    }

    void publish_to_socket(connection& ws, const std::string& payload)
    {
        loop_.defer([&ws, p = payload]() { ws.send(p, uWS::OpCode::TEXT); });
    }

    void publish_to_client(client& client, const std::string& payload)
    {
        loop_.defer([ws = client.ws_, p = payload]() { ws->send(p, uWS::OpCode::TEXT); });
    }

    void client_subscribe(client& client, const std::string& topic)
    {
        loop_.defer([ws = client.ws_, t = topic]() { ws->subscribe(t); });
    }

    void client_unsubscribe(client& client, const std::string& topic)
    {
        loop_.defer([ws = client.ws_, t = topic]() { ws->unsubscribe(t); });
    }

private:
    uWS::App& app_;
    uWS::Loop& loop_;
};
} // namespace net