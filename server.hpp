#pragma once

#include <arpa/inet.h>
#include <ev.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <syslog.h>
#include <unistd.h>

#include <array>
#include <cstring>
#include <ctime>
#include <memory>

#include "exceptions.hpp"

static_assert(sizeof(size_t) == 8);
static_assert(sizeof(int) == 4);

namespace server::echo {
class Server final {
   public:
    Server();
    ~Server();
    Server(Server const &) = delete;
    Server(Server &&) = default;
    Server &operator=(Server const &) = delete;
    Server &operator=(Server &&) = delete;
    void run() const;

   private:
    static void accept_cb(struct ev_loop *loop, ev_io *w, [[maybe_unused]] int revents);
    static void client_cb(struct ev_loop *loop, ev_io *w, int revents);
    static void set_nonblocking(int fd);
    ::std::unique_ptr<struct ev_loop, void (*)(struct ev_loop *)> m_loop; // 8
    static constexpr size_t M_BUFFER_SIZE = 1024;                         // 8
    int m_server_fd = -1;                                                 // 4
    static constexpr int M_SERVER_PORT = 5000;                            // 4
};

} // namespace server::echo