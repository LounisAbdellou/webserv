#include "Client.hpp"
#include "Request.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

Client::Client(int serverFd, int clientFd)
    : _serverFd(serverFd), _clientFd(clientFd) {}

Client::Client(const Client &src)
    : _serverFd(src._serverFd), _clientFd(src._clientFd) {
  *this = src;
}

Client &Client::operator=(const Client &src) {
  (void)src;
  return *this;
}

int Client::getServerFd() const { return _serverFd; }

Request &Client::getRequest() { return this->_request; }

bool Client::receive() {
  char *buffer;
  struct stat statBuffer;

  fstat(this->_clientFd, &statBuffer);
  buffer = new char[statBuffer.st_size + 1];

  ssize_t bytesRead =
      recv(this->_clientFd, buffer, statBuffer.st_size, MSG_DONTWAIT);

  buffer[bytesRead] = '\0';

  if (bytesRead > 0) {
    std::string fragment = buffer;

    std::cout << fragment << "FRAGMENT" << std::endl;

    this->_request.appendRawData(fragment);
  }

  delete buffer;

  return this->_request.getStatus() <= Request::E_REQUEST_COMPLETE;
}

bool Client::isClose() const {
  char buffer[42];
  ssize_t bytesRead = recv(this->_clientFd, buffer, 42, MSG_PEEK);

  return bytesRead < 1;
}
