#pragma once

#include "App.h" // uwebsockets
#include <net/connection.h>

namespace net
{
class websocket_server
{
public:
    explicit websocket_server(int port) : port_(port)
    {
    }

    websocket_server& on_open(std::move_only_function<void(connection*)>&& handler)
    {
        on_open_ = std::move(handler);
        return *this;
    }

    websocket_server& on_message(std::move_only_function<void(connection*, std::string_view, uWS::OpCode)>&& handler)
    {
        on_message_ = std::move(handler);
        return *this;
    }

    websocket_server& on_close(std::move_only_function<void(connection*, std::string_view)>&& handler)
    {
        on_close_ = std::move(handler);
        return *this;
    }

    void run()
    {
        app_
          .ws<client_socket_data>(
            "/*",
            {.compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR | uWS::DEDICATED_DECOMPRESSOR),
             .maxPayloadLength = 100 * 1024 * 1024,
             .idleTimeout = 15, // time in seconds until connection is closed
             .maxBackpressure = 100 * 1024 * 1024,
             .closeOnBackpressureLimit = false,
             .resetIdleTimeoutOnSend = true,
             .sendPingsAutomatically = true,
             .upgrade = nullptr,
             .open = [this](connection* ws) { on_open_(ws); },
             .message = [this](connection* ws, std::string_view message, uWS::OpCode opCode)
             { on_message_(ws, message, opCode); },
             .dropped =
               [](auto* ws, std::string_view message, uWS::OpCode /*opCode*/)
             {
                 std::cout << "Message dropped. " << message << std::endl;
             },
             .drain =
               [](auto* /*ws*/)
             {
                 /* Check ws->getBufferedAmount() here */
             },
             .ping =
               [](connection* ws, std::string_view)
             {
                std::cout << "ping" << std::endl; // to: " << ws->getUserData()->id() << std::endl;
                 /* Not implemented yet */
             },
             .pong =
               [](connection* ws, std::string_view)
             {
                std::cout << "pong" << std::endl; // from: " << ws->getUserData()->id() << std::endl;
                 /* Not implemented yet */
             },
             .close =
               [this](connection* ws, int /*code*/, std::string_view message)
             {
                 on_close_(ws, message);
                 /* You may access ws->getUserData() here */
             }})
          .listen(port_,
                  [this](auto* listen_socket)
                  {
                      if (listen_socket)
                      {
                          std::cout << "Listening on port " << port_ << std::endl;
                      }
                  })
          .run();
    }

    uWS::App& app() 
    {
        return app_;
    }

private:
    int port_;

    uWS::App app_;

    std::move_only_function<void(connection*)> on_open_{nullptr};
    std::move_only_function<void(connection*, std::string_view, uWS::OpCode)> on_message_{nullptr};
    std::move_only_function<void(connection*, std::string_view)> on_close_{nullptr};
};
} // namespace net