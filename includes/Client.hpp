#ifndef CLIENT_HPP
#define CLIENT_HPP

#define BUFFER_SIZE 1024

#include "Request.hpp"

class Client {
private:
  const int _serverFd;
  const int _clientFd;
  Request *_currentRequest;

public:
  Client();
  Client(int serverFd, int clientFd);
  Client(const Client &src);
  ~Client() {};

  Client &operator=(const Client &src);

  int getServerFd() const;
  Request *getCurrentRequest() const;

  bool receive();
};

#endif // !CLIENT_HPP
