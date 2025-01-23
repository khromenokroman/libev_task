#include "server.hpp"

int main(){
    server::echo::Server server;
    server.run();
    return 0;
}