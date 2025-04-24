#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Header.hpp"

#include <string.h>

class Client;

class Request {
public:
  Request(Client& client);
  ~Request();
  
  enum Status {
    E_REQUEST_CREATED = 0,
    E_REQUEST_HEADER = 1,
    E_REQUEST_ARGS = 2,
    E_REQUEST_BODY = 3,
    E_REQUEST_COMPLETE = 4,
    E_REQUEST_BAD = 5,
  };
  
  enum Type {
    E_REQUEST_OTHER = 0,
    E_REQUEST_LIST = 1,
    E_REQUEST_POST = 2,
    E_REQUEST_CGI = 3,
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
  int                                             socket() const;
  void                                            clean();
  
private:
  Request(const Request &src);
  Request& operator=(Request &src);
  
  std::map<std::string, void (Request::*)(std::string &)>   _setters;
  std::map<std::string, std::string(Request::*)() const >   _getters;
  
  Client&                                         _client;
  Status                                          _status;
  Type                                            _type;
  int                                             _pipe[2];
  int                                             _size;
	size_t																					_chunkSize;
  
	std::string																			_chunk;
	std::string																			_chunkContent;

  std::string                                     _method;
  std::string                                     _path;
  std::string                                     _query;
  std::string                                     _protocol;
  std::map<std::string, std::string>              _args;

  bool                                            update();
  void                                            parseHeader(const char* buffer, size_t bread);
  void                                            parseArgs(const char* buffer, size_t bread);
  void                                            parseBody(const char* buffer, size_t bread);
	void																						parseChunk(std::string buffer);
  
  void                                            setMethod(std::string& value);
  void                                            setPath(std::string& value);
  void                                            setQuery(std::string& value);
  void                                            setProtocol(std::string& value);
  void                                            setArgs(std::string& value);
  void                                            setSize(std::string& value);
  void                                            setPipe(std::string& value);
  void                                            setChunk(std::string& value);

  std::string                                     getMethod() const;
  std::string                                     getPath() const;
  std::string                                     getQuery() const;
  std::string                                     getProtocol() const;
  std::string                                     getArgs(const std::string& key) const;
  std::string                                     getReady() const;
  std::string                                     getError() const;
  std::string                                     getType() const;
  std::string                                     getChunk() const;
  std::string                                     getFile() const;
  std::string                                     getList() const;
  std::string                                     getCgi() const;

};

#endif // !REQUEST_HPP
