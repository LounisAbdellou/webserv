#include "Server.hpp"

Server::Server()
{
  _setters["listen"] = &Server::setListen;
  _setters["server_name"] = &Server::setServerName;
  _setters["root"] = &Server::setRoot;
  _setters["index"] = &Server::setIndex;
  _setters["error_page"] = &Server::setErrorPage;
  _setters["allowed_method"] = &Server::setAllowedMethod;
  _setters["allow_listing"] = &Server::setAllowListing; 
  _setters["redirect"] = &Server::setRedirect;

  _getters["server_name"] = &Server::getServerName;
  _getters["listen"] = &Server::getListen;
  _getters["index"] = &Server::getIndex;
}

Server::Server(const Server& cpy)
{
  (void)cpy;
}

Server& Server::operator=(Server& cpy)
{
  return cpy;
}

Server::~Server()
{
  for (std::map<std::string, Location*>::iterator it = this->_locations.begin(); it != this->_locations.end(); ++it) 
  {
    delete it->second;
    it->second = NULL;
  }
}


std::ostream& operator<<(std::ostream& cout, const Server& server)
{
  (void)server;
  cout << "Server up and listenning at : " << std::endl;
  for (std::vector<std::string>::const_iterator it = server.iterator(0); it != server.iterator(-1); ++it)
  {
    std::cout << "=> " << *it << std::endl;
  }
  return cout;
}


bool  Server::has(const std::string key) const
{
  return !(this->_setters.find(key) == this->_setters.end());
}

void  Server::set(const std::string key, std::string value)
{
  (this->*_setters[key])(value);
}

bool  Server::set(const std::string key, Location* location)
{
  if (this->_locations.find(key) != this->_locations.end())
    return false;
  this->_locations[key] = location;
  return true;
}

std::string  Server::get(const std::string key) const 
{
  return (this->*_getters.at(key))();
}

std::vector<std::string>::iterator  Server::iterator(int pos)
{
  if (pos < 0 || pos > (int)this->_listen.size())
    return this->_listen.end();
  std::vector<std::string>::iterator it = this->_listen.begin();
  for (int i = 0; i < pos; i++)
  {
    it++;
  }
  return it;
}

void  Server::handle(Request& request)
{
  // Gestion des erreurs
  // Construction de la response a partir de la request 
  // Gestion des CGI OU Recuperation du contenu du fichier demande
  // ...
  // Destruction de la request
  request.clean();
}

void  Server::send(int fd)
{
  std::string response = this->_response.get();
  /*std::size_t bwrite = ::send(fd, response.c_str(), response.length(), MSG_NOSIGNAL);*/
  ::send(fd, response.c_str(), response.length(), MSG_NOSIGNAL);
  /*return bwrite == response.length();*/
}

std::vector<std::string>::const_iterator  Server::iterator(int pos) const
{
  if (pos < 0 || pos > (int)this->_listen.size())
    return this->_listen.end();
  std::vector<std::string>::const_iterator it = this->_listen.begin();
  for (int i = 0; i < pos; i++)
  {
    it++;
  }
  return it;
}

bool  Server::isset(const std::string key) const
{
  return (!(this->*_getters.at(key))().empty());
}

void  Server::setListen(std::string& value)
{
  if (!Validator::validate("listen", value))
    Validator::throwError("Invalid couple 'ip:port'.");
  if (std::find(this->_listen.begin(), this->_listen.end(), value) != this->_listen.end())
    Validator::throwError("This couple 'ip:port' already exist");
  this->_listen.push_back(value);
}

void  Server::setServerName(std::string& value)
{
  this->_server_name = value;
}

void  Server::setRoot(std::string& value)
{
  if (!this->_root.empty())
    Validator::throwError("Try to rewrite root directory.");
  this->_root = value;
}

void  Server::setIndex(std::string& value)
{
  if (!Validator::validate("file", value))
    Validator::throwError("Invalid file for index.");
  this->_index = value;
}

void  Server::setErrorPage(std::string& value)
{
  if (!Validator::validate("file", value))
    Validator::throwError("Error in error_page instruction.");
  this->_error_page = value;
}

void  Server::setAllowedMethod(std::string& value)
{
  if (!Validator::validate("method", value))
    Validator::throwError("Error in allowed_method instruction.");
  this->_allowed_method = value;
}

void  Server::setAllowListing(std::string& value)
{
  this->_allow_listing = value;
}

void  Server::setRedirect(std::string& value)
{
  this->_redirect = value;
}

std::string  Server::getServerName() const
{
  return this->_server_name;
}

std::string  Server::getListen() const
{
  return this->_listen[0];
}

std::string  Server::getIndex() const
{
  return this->_index;
}
