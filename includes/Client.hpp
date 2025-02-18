#ifndef __CLIENT__
#define __CLIENT__

#include <string>
#include "Request.hpp"

class Client {
  public:
    Client(int server_fd, int client_fd);
    ~Client();
    
    bool  receive() const;
    bool  isClose();

    int                 getServerFd() const;
    int                 getClientFd() const;
    std::string         getRessource() const;
    Request&            getRequest();

  
  private:
    Client();
    Client(const Client& cpy);
    Client& operator=(Client& cpy);

    int     _server_fd;
    int     _client_fd;
    Request _request;
};

#endif
