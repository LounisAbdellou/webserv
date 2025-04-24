#ifndef __SERVER__
#define __SERVER__

#include "Header.hpp"

#include <dirent.h>
#include <sys/types.h>
#include "Location.hpp"
#include "Validator.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Parser.hpp"
#include <sys/socket.h>   // socket(), bind(), listen(), accept(), send(), recv()
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>


class Response;

struct Entry {
	std::string name;
	bool isDir;
};

class Server {
  public:
    Server();
    ~Server();
    
    void                                      init();
    std::string                               handle(const std::string& path, Request& request, Response& response);
    void                                      execute(std::string& ressource, Request& request, Response& response);
    
    void                                      set(const std::string key, const std::string value);
    bool                                      set(const std::string key, Location* location);
    bool                                      has(const std::string key) const;
    bool                                      isset(const std::string key) const;
    std::string                               get(const std::string key) const;

    std::vector<std::string>::iterator        iterator(int pos);
    std::vector<std::string>::const_iterator  iterator(int pos) const;

  private:
    Server(const Server& cpy);
    Server& operator=(Server& cpy);

    std::map<std::string, void (Server::*)(const std::string &)>    _setters;
    std::map<std::string, std::string(Server::*)() const >          _getters;
    
    std::map<std::string, Location*>                                _locations;
    std::vector<std::string>                                        _listen;
    
    std::string                                                     _ctx;
    std::string                                                     _ctx_err;
    std::string                                                     _server_name;

    std::string                                                     _cgi_env[13];
    
    std::string                                                     handleRessource(std::string& ressource, Request& request, Response& response);
    bool                                                            checkPathAccess(std::string method, std::string& ressource);
    bool                                                            checkPathType(Request& request, std::string& ressource, Location* location);
    void                                                            addPathRoot(std::string& ressource, Location* location);
    void                                                            addPathIndex(std::string& ressource, Location* location);
    std::string                                                     handleError(std::string code, std::string& ressource, Location *location);
    
    void                                                            cgi(std::string& ressource, Request& request, Response& response);
    void                                                            listing(const std::string& ressource, Response& response);
    std::string                                                     setCgi(const std::string& ressource, Request& request, bool is_php);
    void                                                            cgiResponse(const std::string& ressource, int req, int res, Response& response);
    void                                                            cgiError(const std::string& ressource, Response& response, int res);
    
    void                                                            setListen(const std::string& value);
    void                                                            setServerName(const std::string& value);
    void                                                            setRoot(const std::string& value);
    void                                                            setIndex(const std::string& value);
    void                                                            setErrorPage(const std::string& value);
    void                                                            setAllowedMethod(const std::string& value);
    void                                                            setAllowListing(const std::string& value);
    void                                                            setRedirect(const std::string& value);
    void                                                            setContentLength(const std::string& ressource, Request& request, Response& response) const;

    std::string                                                     getServerName() const;
    std::string                                                     getListen() const;
    std::string                                                     getIndex() const;
    std::string                                                     getRoot() const;
    std::string                                                     getAllowedMethod() const;
    std::string                                                     getAllowListing() const;
    std::string                                                     getRedirect() const;
    std::string                                                     getErrorPage() const;
    Location*                                                       getLocation(const std::string& ressource);

};

std::ostream&  operator<<(std::ostream& cout, const Server& server);

#endif
