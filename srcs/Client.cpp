#include "Client.hpp"
#include <sys/socket.h>
#include <unistd.h>

Client::Client() : _serverFd(-1), _clientFd(-1) {}

Client::Client(int serverFd, int clientFd)
    : _serverFd(serverFd), _clientFd(clientFd) {}

Client::Client(const Client &src)
    : _serverFd(src._serverFd), _clientFd(src._clientFd) {
  *this = src;
}

Client &Client::operator=(const Client &src) {
  if (this != &src) {
    this->_requestQueue = src._requestQueue;
  }

  return *this;
}

int Client::getServerFd() const { return _serverFd; }

// Request *Client::getCurrentRequest() const { return _currentRequest; }

#include <iostream>
bool Client::receive() {
  bool hasEof = true;
  char buffer[BUFFER_SIZE];

  this->_requestQueue.push(new Request());

  while (true) {
    ssize_t bytesRead = recv(this->_clientFd, buffer, BUFFER_SIZE - 1, 0);
    // std::cout << bytesRead << std::endl;
    if (bytesRead > 0) {
      this->_requestQueue.back()->appendRawData(buffer);
    }

    if (bytesRead < BUFFER_SIZE - 1) {
      break;
    }
  }

  return hasEof;
}

void Client::sendResponse() const {
  std::string response =
      "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";

  send(this->_clientFd, response.c_str(), response.size(), 0);
}

Client::~Client() { close(this->_clientFd); }
