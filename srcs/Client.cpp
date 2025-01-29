#include "Client.hpp"
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
  bool hasEof = true;
  char buffer[BUFFER_SIZE];

  while (true) {
    ssize_t bytesRead = recv(this->_clientFd, buffer, BUFFER_SIZE - 1, 0);
    buffer[bytesRead] = '\0';

    if (bytesRead > 0) {
      std::string fragment = buffer;

      // std::cout << fragment << "FRAGMENT" << std::endl;

      // if (!this->_request) {
      //   this->_request = new Request(this->_clientFd);
      // }

      this->_request.appendRawData(fragment);
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

  std::cout << this->_request.getRawData() << std::endl;

  // delete this->_request;

  send(this->_clientFd, response.c_str(), response.size(), 0);
}

Client::~Client() { close(this->_clientFd); }
