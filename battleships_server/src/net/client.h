#pragma once

#include "App.h"

#include <net/connection.h>
#include <util/non_owning_ptr.h>

namespace net
{
class publisher; // forward declaration

///  
/// clients is trivially copyable including websocket connection raw pointer.
///
class client
{
    ///
    /// Only publisher has possibility to access websocket and its methods
    /// since publisher has thread-safe publish methods that utilizes websocket.
    ///
    friend class publisher;

public:
    client(util::non_owning_ptr<net::connection> ws, client_id_type id) : ws_(ws), id_(id)
    {
    }

    client_id_type id() const
    {
        return id_;
    }

private:
    util::non_owning_ptr<net::connection> ws_;
    client_id_type id_;
};
} // namespace net