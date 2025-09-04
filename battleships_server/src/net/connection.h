#pragma once

#include "App.h"

namespace net
{
using client_id_type = int;

class client_socket_data
{
public:
    client_socket_data() : id_(global_id++)
    {
    }

    client_id_type id() const
    {
        return id_;
    }

private:
    static client_id_type global_id; // TODO: uuid?
    client_id_type id_;
};
client_id_type client_socket_data::global_id = 0;

using connection = uWS::WebSocket<false, true, client_socket_data>;
} // namespace net
