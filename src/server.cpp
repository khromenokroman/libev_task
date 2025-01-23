#include "server.hpp"

server::echo::Server::Server() : m_loop{ev_default_loop(0), ev_loop_destroy} {
    openlog("EchoServer", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Server starting ver: %d.%d.%d-%d ...", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TWEAK);

    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd < 0) {
        syslog(LOG_ERR, "Socket creation failed: %s", strerror(errno));
        throw exceptions::Create_socket(std::string("Socket creation failed: ").append(strerror(errno)));
    }

    set_nonblocking(m_server_fd);

    int opt = 1;
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(M_SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_server_fd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0) {
        syslog(LOG_ERR, "Socket binding failed: %s", strerror(errno));
        throw exceptions::Bind_socket(std::string("Socket binding failed: ").append(strerror(errno)));
    }

    if (listen(m_server_fd, 10) < 0) {
        syslog(LOG_ERR, "Listen failed: %s", strerror(errno));
        throw exceptions::Bind_socket(std::string("Listen failed: ").append(strerror(errno)));
    }

    syslog(LOG_INFO, "Server started and listening on port %d", M_SERVER_PORT);
}
server::echo::Server::~Server() {
    close(m_server_fd);
    syslog(LOG_INFO, "Server shutting down.");
    closelog();
}

void ::server::echo::Server::set_nonblocking(int const fd) {
    int const flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void ::server::echo::Server::client_cb(struct ev_loop *loop, ev_io *watcher, int revents) {
    std::array<char, M_BUFFER_SIZE> buffer{};

    if (revents & EV_READ) {
        //@todo: может получить не все сообщение, не самое лучшее решение
        ssize_t read_bytes = recv(watcher->fd, buffer.data(), buffer.size() - 1, 0);
        if (read_bytes <= 0) {
            syslog(LOG_NOTICE, "Client disconnected.");
            ev_io_stop(loop, watcher);
            close(watcher->fd);
            delete watcher;
        } else {
            buffer[static_cast<::std::size_t>(read_bytes)] = '\0';
            syslog(LOG_INFO, "Received message from client: %s", buffer.data());

            // send message for client
            send(watcher->fd, buffer.data(), static_cast<::std::size_t>(read_bytes), 0);
        }
    }
}

void server::echo::Server::run() const {
    // Init watcher for new connection
    ev_io accept_watcher;
    ev_io_init(&accept_watcher, accept_cb, m_server_fd, EV_READ);
    ev_io_start(m_loop.get(), &accept_watcher);

    // Run loop
    ev_loop(m_loop.get(), 0);
}

void ::server::echo::Server::accept_cb(struct ev_loop *loop, ev_io *watcher, [[maybe_unused]] int revents) {
    struct sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(watcher->fd, reinterpret_cast<struct sockaddr *>(&client_addr), &client_len);
    if (client_fd < 0) {
        syslog(LOG_ERR, "Failed to accept new connection: %s", strerror(errno));
        throw exceptions::Accept_socket(std::string("Listen failed: ").append(strerror(errno)));
    }

    set_nonblocking(client_fd);

    syslog(LOG_INFO, "New connection from: %s", inet_ntoa(client_addr.sin_addr));
    auto client_watcher = std::make_unique<ev_io>();

    // Create new ev_io for connection client
    ev_io_init(client_watcher.get(), ::server::echo::Server::client_cb, client_fd, EV_READ);
    ev_io_start(loop, client_watcher.get());
    client_watcher.release();
}
