#include "Client.hpp"
#include "Request.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

Client::Client(int serverFd, int clientFd)
    : _serverFd(serverFd), _clientFd(clientFd) {}

Client::Client(const Client &src)
    : _serverFd(src._serverFd), _clientFd(src._clientFd) {
  *this = src;
}

Client &Client::operator=(const Client &src) {
  if (this != &src) {
    this->_request = src._request;
  }

  return *this;
}

int Client::getServerFd() const { return _serverFd; }

Request &Client::getRequest() { return this->_request; }

bool Client::receive() {
  char buffer[BUFFER_SIZE];

  while (true) {
    ssize_t bytesRead =
        recv(this->_clientFd, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);

    buffer[bytesRead] = '\0';

    if (bytesRead > 0) {
      std::string fragment = buffer;

      std::cout << fragment << "FRAGMENT" << std::endl;

      this->_request.appendRawData(fragment);
    }

    if (bytesRead < BUFFER_SIZE - 1 ||
        this->_request.getStatus() == REQUEST_BAD) {
      break;
    }
  }

  return this->_request.getStatus() <= REQUEST_COMPLETE;
}

bool Client::isClose() const {
  char buffer[42];
  ssize_t bytesRead = recv(this->_clientFd, buffer, 42, MSG_PEEK);

  return bytesRead < 1;
}
