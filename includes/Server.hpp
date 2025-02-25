#ifndef __SERVER__
#define __SERVER__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <dirent.h>
#include <sys/types.h>
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
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>


#define BUFFER_SIZE 4096

struct Entry {
	std::string name;
	bool isDir;
};

class Server {
  public:
    Server();
    ~Server();

    int	                                      send(int fd);
    void                                      handle(Request& request);
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

    std::string                                             _cgi_env[13];

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
    std::string                                             getRoot() const;
    std::string                                             getAllowedMethod() const;
    std::string                                             getAllowListing() const;
    std::string                                             getRedirect() const;
    std::string                                             getErrorPage() const;
    std::string                                             getResponse() const;
    Location*                                               getLocation(const std::string& ressource);

    std::string                                             handleGet(const std::string& path, Location* location);
    std::string                                             handleDelete(const std::string& path, Location* location);
    std::string                                             handlePost(const std::string& path, const Request& request, Location* location);
    std::string                                             handleAction(const std::string& ressource, Request& request, Location* location);
    void                                                    handleFile(std::string& ressource, Location* location, Request& request);
    std::string                                             handleListing(const std::string& path);
    std::string                                             handleRedirect(Location* location);
    void                                                    handleError(std::string code, std::string& ressource, Location* location);
    
    std::string                                             handleCgi(std::string& ressource, Request& request, Location* location, const std::string& ext);
    void                                                    setEnvCgi(const std::string& ressource, Request& request, bool is_php);
    std::string                                             executeCgi(int fds[2][2], const std::string& cgi_path, const std::string& ressource);
    std::string                                             extractResponseCgi(int fd);

    void                                                    handlePath(std::string& ressource, Request& request, Location* location);
    bool                                                    checkPathAccess(std::string method, std::string& ressource);
    bool                                                    checkPathType(std::string method, std::string& ressource, Location* location);
    void                                                    addPathRoot(std::string& ressource, Location* location);
    void                                                    addPathIndex(std::string& ressource, Location* location);
};

std::ostream&  operator<<(std::ostream& cout, const Server& server);

#endif
