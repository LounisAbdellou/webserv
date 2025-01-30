#include "Client.hpp"
#include <sys/socket.h>

Client::Client() : _serverFd(-1), _clientFd(-1) {}

Client::Client(int serverFd, int clientFd)
    : _serverFd(serverFd), _clientFd(clientFd) {}

Client::Client(const Client &src)
    : _serverFd(src._serverFd), _clientFd(src._clientFd) {
  *this = src;
}

Client &Client::operator=(const Client &src) {
  if (this != &src) {
    this->_currentRequest = src._currentRequest;
  }

  return *this;
}

int Client::getServerFd() const { return _serverFd; }

Request *Client::getCurrentRequest() const { return _currentRequest; }

bool Client::receive() {
  bool hasEof = true;
  char buffer[BUFFER_SIZE];

  if (!this->_currentRequest) {
    this->_currentRequest = new Request();
  }

  while (true) {
    ssize_t bytesRead = recv(this->_clientFd, buffer, BUFFER_SIZE - 1, 0);

    if (bytesRead < 1) {
      break;
    }

    this->_currentRequest->appendRawData(buffer);
  }

  return hasEof;
}
