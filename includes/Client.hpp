#ifndef CLIENT_HPP
#define CLIENT_HPP

#define BUFFER_SIZE 4096

#include "Request.hpp"

class Client {
private:
  const int _serverFd;
  const int _clientFd;
  Request _request;
  bool _isClose;

  Client(const Client &src);
  Client &operator=(const Client &src);

public:
  Client(int serverFd, int clientFd);
  ~Client() {};

  int getServerFd() const;
  int getClientFd() const;
  Request &getRequest();

  void setIsClose(bool isClose);

  bool receive();
  bool isClose() const;
};

#endif // !CLIENT_HPP
