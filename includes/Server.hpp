#ifndef __SERVER__
#define __SERVER__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Location.hpp"
#include "Validator.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <sys/socket.h>   // socket(), bind(), listen(), accept(), send(), recv()

class Server {
  public:
    Server();
    ~Server();

    void                                      handle(Request& request);
    void                                      send(int fd);
    bool                                      has(const std::string key) const;
    void                                      set(const std::string key, std::string value);
    bool                                      set(const std::string key, Location* location);
    bool                                      isset(const std::string key) const;
    std::string                               get(const std::string key) const;
    std::vector<std::string>::iterator        iterator(int pos);
    std::vector<std::string>::const_iterator  iterator(int pos) const;
  
  private:
    Server(const Server& cpy);
    Server& operator=(Server& cpy);

    std::map<std::string, void(Server::*)(std::string&)>    _setters;
    std::map<std::string, std::string(Server::*)() const >  _getters;
    std::map<std::string, Location*>                        _locations;
    std::vector<std::string>                                _listen;
    Response                                                _response;
    
    std::string                                             _server_name;
    std::string                                             _root;
    std::string                                             _index;
    std::string                                             _error_page;
    std::string                                             _allowed_method;
    std::string                                             _allow_listing;
    std::string                                             _redirect;

    void          setListen(std::string& value);
    void          setServerName(std::string& value);
    void          setRoot(std::string& value);
    void          setIndex(std::string& value);
    void          setErrorPage(std::string& value);
    void          setAllowedMethod(std::string& value);
    void          setAllowListing(std::string& value);
    void          setRedirect(std::string& value);

    std::string  getServerName() const;
    std::string  getListen() const;
    std::string  getIndex() const;
};

std::ostream&  operator<<(std::ostream& cout, const Server& server);

#endif
