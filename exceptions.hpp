#pragma once

#include <exception>
#include <string>

namespace server::echo::exceptions {

struct Exceptions_server : std::exception {
    explicit Exceptions_server(std::string message_) : message{std::move(message_)} {};

    [[nodiscard]] const char *what() const noexcept override { return message.c_str(); }

private:
    std::string message{};
};
struct Create_socket final : Exceptions_server {
    using Exceptions_server::Exceptions_server;
};
struct Bind_socket final : Exceptions_server {
    using Exceptions_server::Exceptions_server;
};
struct Listen_socket final : Exceptions_server {
    using Exceptions_server::Exceptions_server;
};
struct Accept_socket final : Exceptions_server {
    using Exceptions_server::Exceptions_server;
};
} // namespace exception_plugins::os