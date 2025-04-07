#include "Client.hpp"

Client::Client(Server& server, int fd) : _socket(fd), _server(server), _request(this) {}

Client::~Client() {}

void  Client::init()
{
  _setters["ressource"] = &Client::setRessource;
  
  _getters["ressource"] = &Client::getRessource;

}

void Client::set(const std::string key, std::string value) 
{
  if (this->_setters.find(key) == this->_setters.end()) return ;
  (this->*_setters[key])(value);
}

bool Client::has(const std::string key) const 
{
  return !(this->_setters.find(key) == this->_setters.end());
}

bool Client::isset(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return false;
  return (!(this->*_getters.at(key))().empty());
}

std::string Client::get(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return "";
  return (this->*_getters.at(key))();
}

bool  Client::receive()
{
  ssize_t bread = recv(this->_socket, this->_buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);

  if (bread == -1)
    throw Parser::WebservParseException("");
  
  if (bread == 0)
    return this->_request.isset("ready");

  this->_buffer[bread] = '\0';
  this->_request.parse(this->_buffer, bread);
  ::bzero(this->_buffer, BUFFER_SIZE);
  return this->_request.isset("ready");
}

bool  Client::send()
{
  if (!this->isset("ressource"))
    this->set("ressource", this->_request.get("path"));
  
  std::string content;
  
  if (!this->_response.isset("status"))
  {
    content.append(this->_response.get("header"));
    this->_response.set(Response::E_RESPONSE_HEADER);
  }

  if (!this->_response.isset("type"))
    this->_response.set(Response::E_RESPONSE_COMPLETE);
  else
  {
    if (!this->_request.get("type").compare("PIPE"))
      this->_server.execute(this->_ressource, this->_request, this->_response);
    content.append(this->read());
  }

  ssize_t bwrite = ::send(this->_socket, content.c_str(), content.length(), MSG_NOSIGNAL);

  if (bwrite == -1)
    throw Parser::WebservParseException("");

  ::bzero(this->_buffer, BUFFER_SIZE);
  return this->_response.isset("done");
}

int   Client::write(char* buffer, size_t bytes)
{
  if (!this->isset("ressource"))
    this->set("ressource", this->_request.get("path"));

  if (!this->_request.get("type").compare("DOC"))
  {
    std::ofstream target(this->_ressource.c_str(), std::ios::app | std::ios::binary);

    target.write(buffer, bytes);

    target.close();

    return bytes;
  }
  
  int* fd = this->_request.pipe("open");

  return ::write(fd[1], buffer, bytes);
}

std::string   Client::read()
{
  std::string body;

  if (!this->_response.get("type").compare("DOC"))
  {
    std::ifstream target(this->_ressource.c_str(), std::ios::binary);
    while (true) 
    {	
      int bytes = target.readsome(this->_buffer, BUFFER_SIZE);
      if (bytes <= 0)
        break;
      
      std::string content(this->_buffer, bytes);
      body.append(content);
    }
    target.close();
  }
  else
  {
    int* fd = this->_response.pipe("open");
    while (::read(fd[0], this->_buffer, BUFFER_SIZE) > 0)
      body.append(this->_buffer);
    this->_response.pipe("close");
  }
  return body;
}

bool  Client::remove()
{
  if (!this->isset("ressource"))
    this->set("ressource", this->_request.get("path"));

  if (::remove(this->_ressource.c_str()) == -1)
    return false;

  return true;
}

void  Client::setRessource(const std::string& value)
{
  this->_ressource = this->_server.handle(value, this->_request, this->_response);
}

std::string Client::getRessource() const
{
  return this->_ressource;
}
