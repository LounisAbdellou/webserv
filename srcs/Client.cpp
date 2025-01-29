#include "Client.hpp"

Client::Client() : _serverFd(-1), _clientFd(-1) {}

Client::Client(int serverFd, int clientFd)
    : _serverFd(serverFd), _clientFd(clientFd) {}

Client::Client(const Client &src)
    : _serverFd(src._serverFd), _clientFd(src._clientFd) {
  *this = src;
}

Client &Client::operator=(const Client &src) {
  if (this != &src) {
  }

  return *this;
}
