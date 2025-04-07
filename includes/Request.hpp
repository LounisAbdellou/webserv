#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string.h>

class Client;

class Request {
public:
  Request(Client* client);
  ~Request() {};
  
  enum Status {
    E_REQUEST_CREATED = 0,
    E_REQUEST_HEADER = 1,
    E_REQUEST_ARGS = 2,
    E_REQUEST_COMPLETE = 3,
    E_REQUEST_BAD = 4,
  };
  
  enum Type {
    E_REQUEST_OTHER = 0,
    E_REQUEST_POST = 1,
    E_REQUEST_CHUNK = 2,
    E_REQUEST_CGI = 3,
    E_REQUEST_CGI_CHUNK = 4,
  };

  void                                            init();
  void                                            parse(const char* buffer, size_t bytes);
  
  void                                            set(const std::string key, const std::string value);
  void                                            set(Request::Status status);
  void                                            set(Request::Type type);
  bool                                            has(const std::string key) const;
  bool                                            isset(const std::string key) const;
  std::string                                     get(const std::string key) const;
  std::string                                     get(const std::string container, const std::string key) const;

  int*                                            pipe(const std::string value);
  int                                             socket();
  void                                            clean();
  
private:
  Request(const Request &src);
  Request &operator=(const Request &src);
  
  std::map<std::string, void (Request::*)(const std::string &)>   _setters;
  std::map<std::string, std::string(Request::*)() const >         _getters;
  
  Client*                                         _client;
  Status                                          _status;
  Type                                            _type;
  int                                             _pipe[2];
  int                                             _size;
  
  std::string                                     _method;
  std::string                                     _path;
  std::string                                     _query;
  std::string                                     _protocol;
  std::map<std::string, std::string>              _args;

  bool                                            update();
  void                                            parseHeader(const char* buffer);
  void                                            parseArgs(const char* buffer);
  void                                            parseBody(const char* buffer);
  
  void                                            setMethod(std::string& value);
  void                                            setPath(std::string& value);
  void                                            setQuery(std::string& value);
  void                                            setProtocol(std::string& value);
  void                                            setArgs(std::string& value);
  void                                            setSize(std::string& value);

  std::string                                     getMethod() const;
  std::string                                     getPath() const;
  std::string                                     getQuery() const;
  std::string                                     getProtocol() const;
  std::string                                     getArgs(const std::string& key) const;
  std::string                                     getReady() const;
  std::string                                     getError() const;
  std::string                                     getType() const;
  std::string                                     getChunk() const;

};

#endif // !REQUEST_HPP
