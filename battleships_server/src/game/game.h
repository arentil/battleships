#pragma once

#include <game/lobby.h>
#include <game/match.h>
#include <net/client.h>
#include <net/publisher.h>
#include <util/string.h>

namespace game
{
using match_id = int;

class game
{
public:
    explicit game(net::publisher& publisher) : publisher_(publisher)
    {
    }

    void on_open(net::client& client)
    {
        std::unique_lock ul(m_);
        bool is_emplaced = clients_.try_emplace(client.id(), client).second;
        ul.unlock();

        if (!is_emplaced)
        {
            std::cout << "Failed to add client into the game. Possible id dupplication" << std::endl;
            return;
        }

        publisher_.publish_to_client(client, "HELLO_FROM_SERVER");
    }

    void on_message(net::client& client, const std::string& payload)
    {
        // publisher_.publish_to_topic("user", "publish via topic");
        // publisher_.publish_to_client(client, payload);

        const auto tokens = std::split(payload, "|");

        if (tokens.front() == "JOIN_LOBBY")
        {
            std::unique_lock ul(m_);
            if (lobby_.size() == 0)
            {
                lobby_.push_client_to_lobby(client.id());
                ul.unlock();

                publisher_.publish_to_client(client, "LOBBY_JOINED");
            }
            else if (lobby_.size() >= 1)
            {
                const auto other_client_id = lobby_.front();
                lobby_.pop();
                net::client& other_client = clients_.at(other_client_id);
                ul.unlock();

                publisher_.publish_to_client(client, "MATCH_FOUND");
                publisher_.publish_to_client(other_client, "MATCH_FOUND");

                create_new_match(client, other_client);
            }
        }
        else if (tokens.front() == "CHAT")
        {
            const std::string& message_from_client = tokens.back();
            std::unique_lock ul(m_);
            if (auto it = client_to_match_.find(client.id()); it != client_to_match_.end())
            {
                const match_id& match_id = it->second;
                net::client opponent = matches_.at(match_id).player_1.client.id() == client.id()
                                         ? matches_.at(match_id).player_2.client
                                         : matches_.at(match_id).player_1.client;
                ul.unlock(); // safe to already unlock, because opponent client is copied
                publisher_.publish_to_client(opponent, message_from_client);
            }
            else
            {
                ul.unlock();
                publisher_.publish_to_client(client, "CHAT_RESULT|NOT_IN_MATCH");
            }
        }
    }

    void on_close(net::client& client, const std::string& payload)
    {
        std::lock_guard l(m_);
        const auto client_id = client.id();
        lobby_.remove_client_from_lobby(client_id);        
        if (auto client_to_match_it = client_to_match_.find(client_id); client_to_match_it != client_to_match_.end())
        {
            terminate_match(client_to_match_it->second);
            client_to_match_.erase(client_to_match_it);
        }
        clients_.erase(client_id);
    }

private:
    void create_new_match(net::client& first_client, net::client& second_client)
    {
        static match_id global_match_id = 0;
        const match_id new_match_id = global_match_id++; // use some UUID
        const std::string room_str = "room:" + std::to_string(new_match_id);

        publisher_.client_subscribe(first_client, room_str);
        publisher_.client_subscribe(second_client, room_str);

        // with new match, first there is a need to set fleet on board, then players turn will be randomized
        match new_match(room_str, first_client, second_client);

        std::unique_lock ul(m_);
        matches_.emplace(new_match_id, std::move(new_match));
        client_to_match_.emplace(first_client.id(), new_match_id);
        client_to_match_.emplace(second_client.id(), new_match_id);
        new_match.state = match_state::setup;
        ul.unlock();

        publisher_.publish_to_topic(room_str, "GAME|SET_SHIPS");
    }

    void terminate_match(match_id id)
    {
        const std::string room_str = "room:" + std::to_string(id);

        std::unique_lock ul(m_);

        matches::iterator match_it;
        if (match_it = matches_.find(id); match_it == matches_.end())
            return;

        match& match_to_remove = match_it->second;
        match_to_remove.state = match_state::finished;
        net::client c1 = match_to_remove.player_1.client;
        net::client c2 = match_to_remove.player_2.client;
        matches_.erase(match_it);

        client_to_match_.erase(c1.id());
        client_to_match_.erase(c2.id());

        ul.unlock();

        publisher_.publish_to_topic(room_str, "GAME|TERMINATED");
    }

    net::publisher& publisher_;

    std::mutex m_; // potential bottleneck? Would it be faster with 3 mutexes: for clients, lobby and matches?
    std::unordered_map<net::client_id_type, net::client> clients_;

    lobby lobby_;
    std::unordered_map<net::client_id_type, match_id> client_to_match_; // map of clients and their started match id
    using matches = std::unordered_map<match_id, match>;
    matches matches_;                       // map of matches
};
} // namespace game