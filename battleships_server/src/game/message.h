#pragma once



namespace game
{
enum class message_type
{
    unsupported,
    session, // eg.: LOGIN, LOGOUT, ERROR
    matchmaking, // eg.: SEARCH_GAME, MATCH_FOUND, LEAVE_ROOM, READY
    game, // eg.: FIRE, FIRE_RESULT, YOUR_TURN, OPPONENT_TURN, GAME_OVER
    chat // eg.: CHAT
};

message_type from_string(const std::string& message_type_str)
{
    if (message_type_str == "CHAT")
        return message_type::chat;
    return message_type::unsupported;
}

class message
{
public:
    explicit message(const std::string& message_str)
    {
        
    }

private:
    message_type type;
    std::string payload;
};
}