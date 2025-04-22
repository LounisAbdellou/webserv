#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Header.hpp"

#include "Server.hpp"
#include "Response.hpp"

class Server;
class Request;

class Client {
public:
  Client(Server& server, int fd);
  ~Client();

  void          init();
  void          set(const std::string key, const std::string value);
  bool          has(const std::string key) const;
  bool          isset(const std::string key) const;
  int           socket() const;
  std::string   get(const std::string key) const;
  void          clean();


  bool          receive();
  bool          send();
  std::string   read();
  int           write(char* buffer, size_t bytes);
  bool          remove();
  
private:
  std::map<std::string, void (Client::*)(const std::string &)>   _setters;
  std::map<std::string, std::string(Client::*)() const >         _getters;
  
  const int     _socket;
  Server&       _server;
  Request       _request;                  
  Response      _response;                  
  char          _buffer[BUFFER_SIZE + 1];
  
  std::string   _ressource;
  
  void          setRessource(const std::string& value);

  std::string   getRessource() const;

};

#endif // !CLIENT_HPP
