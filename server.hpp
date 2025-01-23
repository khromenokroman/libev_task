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

static_assert(sizeof(::std::unique_ptr<struct ev_loop, void (*)(struct ev_loop *)>) == 16);
static_assert(sizeof(size_t) == 8);
static_assert(sizeof(int) == 4);

namespace server::echo {
class Server final {
   public:
    /**
     * @brief Конструктор Echo-сервера.
     *
     * Инициализирует цикл обработки событий, создает серверный сокет,
     * привязывает его к указанному порту, переводит сокет в неблокирующий режим
     * и начинает прослушивание входящих соединений.
     *
     * @throw exceptions::Create_socket если не удалось создать сокет.
     * @throw exceptions::Bind_socket если возникла ошибка при привязке
     * или прослушивании сокета.
     */
    Server();
    /**
     * @brief Деструктор Echo-сервера.
     *
     * Закрывает серверный сокет и завершает работу с syslog.
     */
    ~Server();

    Server(Server const &) = delete;
    Server(Server &&) = default;
    Server &operator=(Server const &) = delete;
    Server &operator=(Server &&) = delete;
    /**
     * @brief Запуск Echo-сервера.
     *
     * Создает наблюдатель для обработки новых соединений и запускает основной цикл обработчика событий.
     */
    void run() const;

   private:
    /**
     * @brief Callback-функция для обработки нового соединения.
     *
     * Вызывается, когда сервер обнаруживает новый входящий запрос на подключение.
     * Принимает соединение, переводит его в неблокирующий режим, создает наблюдатель
     * для обработки событий от клиента.
     *
     * @param loop Указатель на цикл обработки событий.
     * @param watcher Наблюдатель серверного сокета.
     * @param revents Тип события (например, EV_READ).
     * @throw exceptions::Accept_socket если произошла ошибка при приеме нового соединения.
     */
    static void accept_cb(struct ev_loop *loop, ev_io *watcher, [[maybe_unused]] int revents);
    /**
     * @brief Callback-функция для обработки событий от клиента.
     *
     * Вызывается в случае, если на клиентском сокете имеются данные для чтения.
     * Обрабатывает входящие данные, отправляет их обратно клиенту.
     * Завершает соединение, если клиент отключился или произошла ошибка.
     *
     * @param loop Указатель на цикл обработки событий.
     * @param watcher Наблюдатель (watcher) клиентского сокета.
     * @param revents Тип события (например, EV_READ).
     */
    static void client_cb(struct ev_loop *loop, ev_io *watcher, int revents);
    /**
     * @brief Устанавливает неблокирующий режим для сокета.
     *
     * @param fd Дескриптор сокета.
     */
    static void set_nonblocking(int fd);
    ::std::unique_ptr<struct ev_loop, void (*)(struct ev_loop *)> m_loop; // 16
    static constexpr size_t M_BUFFER_SIZE = 1024;                         // 8
    int m_server_fd = -1;                                                 // 4
    static constexpr int M_SERVER_PORT = 5000;                            // 4
};

} // namespace server::echo