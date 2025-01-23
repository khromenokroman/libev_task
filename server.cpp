#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <ev.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <syslog.h>
#include <unistd.h>
#include <iostream>


static constexpr size_t BUFFER_SIZE = 1024;
static constexpr int SERVER_PORT = 5000;

// set nonblock
void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// process event
void client_cb(struct ev_loop *loop, ev_io *w, int revents) {
    char buffer[BUFFER_SIZE];
    ssize_t read_bytes;

    if (revents & EV_READ) {
        read_bytes = recv(w->fd, buffer, sizeof(buffer) - 1, 0);
        if (read_bytes <= 0) {
            syslog(LOG_NOTICE, "Client disconnected.");
            ev_io_stop(loop, w);
            close(w->fd);
            delete w;
        } else {
            buffer[read_bytes] = '\0'; // Завершаем строку
            syslog(LOG_INFO, "Received message from client: %s", buffer);

            // send message for client
            send(w->fd, buffer, read_bytes, 0);
        }
    }
}

// process new connection
void accept_cb(struct ev_loop *loop, ev_io *w, [[maybe_unused]] int revents) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(w->fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        syslog(LOG_ERR, "Failed to accept new connection: %m");
        return;
    }

    set_nonblocking(client_fd);

    syslog(LOG_INFO, "New connection from: %s", inet_ntoa(client_addr.sin_addr));

    // Create new ev_io for connection client
    auto *client_watcher = new ev_io;
    ev_io_init(client_watcher, client_cb, client_fd, EV_READ);
    ev_io_start(loop, client_watcher);
}

int main() {
    openlog("EchoServer", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Server starting...");

    struct ev_loop *loop = ev_default_loop(0);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        syslog(LOG_ERR, "Socket creation failed: %m");
        return -1;
    }

    set_nonblocking(server_fd);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        syslog(LOG_ERR, "Socket binding failed: %m");
        return -1;
    }

    if (listen(server_fd, 10) < 0) {
        syslog(LOG_ERR, "Listen failed: %m");
        return -1;
    }

    syslog(LOG_INFO, "Server started and listening on port %d", SERVER_PORT);

    // Init watcher for new connection
    ev_io accept_watcher;
    ev_io_init(&accept_watcher, accept_cb, server_fd, EV_READ);
    ev_io_start(loop, &accept_watcher);

    // Run loop
    ev_loop(loop, 0);

    close(server_fd);
    syslog(LOG_INFO, "Server shutting down.");
    closelog();

    return 0;
}