# Echo TCP Server с использованием libev

## Описание
Этот проект реализует легковесный **echo-сервер** на основе **TCP** с использованием библиотеки `libev` для событийно-ориентированного ввода-вывода. Сервер предназначен для обработки нескольких подключений одновременно и возвращает клиенту любые полученные от него сообщения.

Ключевые характеристики:
- Асинхронная обработка нескольких подключений с использованием **неблокирующего ввода-вывода**.
- Логирование событий сервера с помощью **syslog** для упрощения мониторинга и отладки.

## Возможности
- **Echo-сервер**: Отправляет клиенту обратно любое полученное сообщение.
- **Событийно-ориентированное управление**: Использует библиотеку `libev` для обработки нескольких клиентов.
- **Интеграция с Syslog**: Логирует такие события как подключения, отключения клиентов и ошибки.
- **Неблокирующие сокеты**: Обеспечивает производительность и асинхронность.

## Как это работает
![work](video/libev.gif)

## Как собрать
````bash
apt install libev-dev build-essential
mkdir -p build && cd build
cmake ..
cmake --build . 
cpack -G DEB
````
Соберется пакет `libev_task-0.0.1-Linux.deb` который можно установить используя пакетный менеджер
````bash
apt install ./libev_task-0.0.1-Linux.deb 
#или
dpkg -i ./libev_task-0.0.1-Linux.deb
````
## Как запустить
````bash
./server
#или если установка была через пакетный менеджер
server
````
- Принимает входящие подключения.
- Эхо-сообщения (отправляет клиенту то же самое сообщение, что и получает).
- Записывает события в системный журнал через `syslog`.

## Как протестировать
### Через `netcat`:
Откройте терминал и подключитесь к серверу:
````bash
nc localhost 5000
````
Отправьте сообщение серверу и получите ответ:
````bash
Hello!
Hello!
````
