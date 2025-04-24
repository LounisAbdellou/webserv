#include "Client.hpp"
#include "Request.hpp"

Client::Client(Server& server, int fd, const std::string& max_body) : _socket(fd), _server(server), _request(*this) 
{
  this->_max_body_size = Parser::strtoll(max_body, 10);
  ::bzero(this->_buffer, BUFFER_SIZE + 1);
  this->init();
}

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

  if (bread < 1)
    throw Parser::WebservParseException("");
  
  this->_buffer[bread] = '\0';
  this->_request.parse(this->_buffer, bread);
  ::bzero(this->_buffer, BUFFER_SIZE + 1);
  return this->_request.isset("ready");
}

bool  Client::send()
{
  if (!this->isset("ressource"))
    this->set("ressource", this->_request.get("path"));

  if (!this->_request.get("method").compare("DELETE"))
    this->remove();
  
  if (!this->_request.get("type").compare("PIPE") && !this->_response.isset("cgi"))
    this->_server.execute(this->_ressource, this->_request, this->_response);
  
  std::string content;
  int offset = 0;
  
  if (!this->_response.isset("status"))
  {
    content.append(this->_response.get("header"));
    if (!this->_request.isset("cgi") || !Parser::getExtension(this->_ressource).compare(".py"))
      content.append("\r\n");
    offset = content.length();
    this->_response.set(Response::E_RESPONSE_HEADER);
  }

  if (!this->_response.isset("type"))
    this->_response.set(Response::E_RESPONSE_COMPLETE);
  else
    content.append(this->read());

  ssize_t bwrite = ::send(this->_socket, content.c_str(), content.length(), MSG_NOSIGNAL);

  if (bwrite == -1)
    throw Parser::WebservParseException("");

  this->_response.bsend((bwrite - offset) * -1);
  
  if (this->_response.bsend() == 0)
  {
    this->_response.pipe("close");
    this->_request.pipe("close");
  }

  ::bzero(this->_buffer, BUFFER_SIZE + 1);
  return this->_response.isset("done");
}

int   Client::write(char* buffer, size_t bytes)
{
  if (!this->isset("ressource"))
    this->set("ressource", this->_request.get("path"));

  if (!this->_request.get("type").compare("DOC"))
  {
    if (!this->_request.isset("file"))
    {
      this->remove();
      this->_request.set(Request::E_REQUEST_BODY);
    }
    
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
    target.seekg(this->_response.bsend() * -1, std::ios::end);
    int bytes = target.readsome(this->_buffer, BUFFER_SIZE);
    if (this->_response.bsend() - bytes == 0)
      this->_response.set(Response::E_RESPONSE_COMPLETE);
    std::string content(this->_buffer, bytes);
    body.append(content);
    target.close();
  }
  else
  {
    int* fd = this->_response.pipe("open");
    int bytes = ::read(fd[0], this->_buffer, BUFFER_SIZE);
    if (this->_response.bsend() - bytes == 0 || bytes == 0)
      this->_response.set(Response::E_RESPONSE_COMPLETE);
    body.append(this->_buffer);
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

int Client::socket() const
{
  return this->_socket;
}

int Client::maxBody() const
{
  return this->_max_body_size;
}

void  Client::clean()
{
  this->_request.clean();
  this->_response.clean();
  this->_ressource.clear();
}
