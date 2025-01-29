#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client {
private:
  const int _clientFd;
  const int _serverFd;

public:
  Client();
  Client(int serverFd, int clientFd);
  Client(const Client &src);
  ~Client() {};

  Client &operator=(const Client &src);

  int getServerFd() const;
};

#endif // !CLIENT_HPP
