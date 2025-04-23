#ifndef __WEBSERV__
#define __WEBSERV__

#include "Header.hpp"

#include <queue>
#include "Parser.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <fcntl.h>
#include "Validator.hpp"
#include <sys/types.h>    // Définitions de types (socklen_t, etc.)
#include <sys/socket.h>   // socket(), bind(), listen(), accept(), send(), recv()
#include <netinet/in.h>   // struct sockaddr_in (adresses IP)
#include <arpa/inet.h> 
#include <sys/epoll.h>    // epoll_create(), epoll_ctl(), epoll_wait()
#include <stdlib.h>       // exit()
#include <unistd.h>
#include <cerrno>

class Webserv {
  public:
    Webserv();
    Webserv(const std::string& configFile);
    ~Webserv();
    
    static bool is_running;

    bool  configure(const std::string& configFile);
    void  init();
    void  run();
    
    class		WebservException : public std::invalid_argument
    {
      public:
        WebservException(std::string what) : std::invalid_argument(what) {}
    };

  private:
    Webserv(const Webserv& cpy);
    Webserv& operator=(Webserv& cpy);

    std::vector<Server*>                                  _servers;
    std::map<std::string, int>                            _hosts;
    std::map<int, std::queue<Server*>* >                  _sockets;
    std::map<std::string, Server*>                        _names;
    std::map<int, Client*>                                _clients;
    std::map<std::string, void(Webserv::*)(std::string)>  _setters;
    
    int                                                   _epoll_fd;
    std::string                                           _max_body_size;
    
    void  setMaxBodySize(const std::string value);
    
    void  configureServer(std::ifstream& file);
    void  configureLocation(std::ifstream& file, Server* server, const std::string path);
    void  initServer(Server* server);
    void  initSocket(std::string& host, int socket_fd) const;
    void  close(int fd);

    void  throwError(std::string message) const;
};

#endif
