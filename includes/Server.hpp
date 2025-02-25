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
#include "AHttpMessage.hpp"
#include "Parser.hpp"
#include <sys/socket.h>   // socket(), bind(), listen(), accept(), send(), recv()
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>

class Server {
  public:
    Server();
    ~Server();

    void                                      handle(Request& request);
    void                                      setDefault();
    bool                                      send(int fd);
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
    
    std::string                                             _ctx;
    std::string                                             _ctx_err;
    std::string                                             _server_name;
    std::string                                             _root;
    std::string                                             _index;
    std::string                                             _error_page;
    std::string                                             _allowed_method;
    std::string                                             _allow_listing;
    std::string                                             _redirect;

    void                                                    setListen(std::string& value);
    void                                                    setServerName(std::string& value);
    void                                                    setRoot(std::string& value);
    void                                                    setIndex(std::string& value);
    void                                                    setErrorPage(std::string& value);
    void                                                    setAllowedMethod(std::string& value);
    void                                                    setAllowListing(std::string& value);
    void                                                    setRedirect(std::string& value);

    std::string                                             getServerName() const;
    std::string                                             getListen() const;
    std::string                                             getIndex() const;
    Location*                                               getLocation(const std::string& ressource);

    std::string                                             handleCgi();
    std::string                                             handleGet(const std::string& path);
    std::string                                             handleDelete(const std::string& path, Location* location);
    std::string                                             handlePost(const std::string& path, const std::string body, Location* location);
    std::string                                             handleAction(const std::string& ressource, Request& request, Location* location);
    void                                                    handleFile(std::string& ressource, Location* location, Request& request);
    std::string                                             handleListing(const std::string& path);
    void                                                    handleError(std::string code, std::string& ressource, Location* location);

    void                                                    handlePath(std::string& ressource, Location* location, Request& request);
    bool                                                    checkPathAccess(std::string method, std::string& ressource);
    bool                                                    checkPathType(std::string method, std::string& ressource, Location* location = NULL);
    void                                                    addPathRoot(std::string& ressource, Location* location);
    void                                                    addPathIndex(std::string& ressource, Location* location);

    bool                                                    isListingAllowed() const;
};

std::ostream&  operator<<(std::ostream& cout, const Server& server);

#endif
