#include "Client.hpp"
#include "Request.hpp"
#include <sys/socket.h>
#include <unistd.h>

Client::Client(int serverFd, int clientFd)
    : _serverFd(serverFd), _clientFd(clientFd), _request(clientFd),
      _isClose(false) {}

Client::Client(const Client &src)
    : _serverFd(src._serverFd), _clientFd(src._clientFd),
      _request(src._clientFd) {
  *this = src;
}

Client &Client::operator=(const Client &src) {
  (void)src;
  return *this;
}

int Client::getServerFd() const { return _serverFd; }

int Client::getClientFd() const { return _clientFd; }

Request &Client::getRequest() { return this->_request; }

bool Client::isClose() const { return this->_isClose; }

void Client::setIsClose(bool value) { this->_isClose = value; }

bool Client::receive() {
  char buffer[BUFFER_SIZE];

  ssize_t bytesRead =
      recv(this->_clientFd, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);

  if (bytesRead == -1) {
    this->_request.setStatus(Request::E_REQUEST_BAD);
    this->_request.setResponseCode(AHttpMessage::INTERNAL_SERVER_ERROR);
    this->_isClose = true;
    return true;
    // } else if (bytesRead == 0) {
    //
  }

  if (bytesRead > 0) {
    std::string fragment(buffer, bytesRead);

    // std::cout << "///////////FRAGMENT/////////////" << std::endl;
    // std::string::const_iterator it;
    // for (it = fragment.begin(); it != fragment.end(); it++) {
    //   if (isprint(*it)) {
    //     std::cout << *it;
    //   }
    // }
    // std::cout << std::endl;
    // std::cout << "/////////////END////////////////" << std::endl;

    this->_request.appendRawData(fragment);
  }

  return this->_request.getStatus() <= Request::E_REQUEST_COMPLETE;
}
