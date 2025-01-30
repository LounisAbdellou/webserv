#ifndef CLIENT_HPP
#define CLIENT_HPP

#define BUFFER_SIZE 1024

#include "Request.hpp"
#include <queue>

class Client {
private:
  const int _serverFd;
  const int _clientFd;
  std::queue<Request *> _requestQueue;

public:
  Client();
  Client(int serverFd, int clientFd);
  Client(const Client &src);
  ~Client();

  Client &operator=(const Client &src);

  int getServerFd() const;
  // Request *getCurrentRequest() const;

  bool receive();
  void sendResponse() const;
};

#endif // !CLIENT_HPP
