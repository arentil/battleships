#pragma once

#include <net/connection.h>

namespace game
{
class lobby
{
public:
    void push_client_to_lobby(net::client_id_type client_id)
    {
        if (auto lobby_it = std::ranges::find(lobby_, client_id); lobby_it != lobby_.end())
        {
            return;
        }

        lobby_.push_back(client_id);
    }

    void remove_client_from_lobby(net::client_id_type client_id)
    {
        if (auto lobby_it = std::ranges::find(lobby_, client_id); lobby_it == lobby_.end())
        {
            return;
        }
        else
        {
            lobby_.erase(lobby_it);
        }
    }

    size_t size() const
    {
        return lobby_.size();
    }

    net::client_id_type front()
    {
        return lobby_.front();
    }

    void pop()
    {
        lobby_.pop_front();
    }

private:
    std::deque<net::client_id_type> lobby_;
};
} // namespace game