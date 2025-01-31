#include "Client.hpp"

Client::Client()
{
}

Client::Client(int server_fd) : _server_fd(server_fd)
{
}

Client::Client(const Client& cpy)
{
  (void)cpy;
}

Client& Client::operator=(Client& cpy)
{
  return cpy;
}

Client::~Client()
{
}

bool  Client::receive() const
{
  return true;
}

bool  Client::isClose()
{
  // Test avec un recv sans bouger la tete de lecture
  return true;
}

int       Client::getServerFd() const
{
  return this->_server_fd;
}

Request&  Client::getRequest()
{
  return this->_request;
}
