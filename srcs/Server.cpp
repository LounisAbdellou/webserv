#include "Server.hpp"

Server::Server() : _ctx("./var/www"), _ctx_err("./var/error")
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

void  Server::setDefault()
{
  if (this->_listen.empty())
    this->set("listen", "80");
  if (this->_index.empty())
    this->_index = "index.html";
  if (this->_error_page.empty())
    this->_error_page = "error.html";
  if (this->_allow_listing.empty())
    this->_allow_listing = "false";
  if (this->_allowed_method.empty())
    this->_allowed_method = "GET/POST/DELETE";
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
  std::string ressource = request.getPath();
  
  Location* location = this->getLocation(ressource);
  
  this->handlePath(ressource, location, request);

  std::string ext = Parser::getExtension(ressource);
  std::string body;

  if (!ext.compare(".php") || !ext.compare(".py"))
    body = this->handleCgi();
  else 
    body = this->handleAction(ressource, request, location);

  std::cout << "body : '" <<  body << "'" << std::endl;

  // 3. Construire la response a partir du chemin + body 
  this->_response.generate(body, request);
  request.clean();
}

void  Server::handlePath(std::string& ressource, Location* location, Request& request)
{
  if (request.getStatus() == Request::E_REQUEST_BAD)
    return this->handleError(request.getResponseCode(), ressource, location);
  
  this->addPathRoot(ressource, location);

  if (this->checkPathType(request.getMethod(), ressource, location))
    this->addPathIndex(ressource, location);
  
  if (access(Parser::getFolder(ressource).c_str(), F_OK) == -1 || (access(ressource.c_str(), F_OK) == -1 && request.getMethod().compare("POST")))
    return this->handleError(AHttpMessage::NOT_FOUND, ressource, location);
  
  if (!this->checkPathAccess(request.getMethod(), ressource))
    return this->handleError(AHttpMessage::FORBIDDEN, ressource, location);
}

bool  Server::checkPathAccess(std::string method, std::string& ressource)
{
  std::cout << this->_response.getIsListing() << std::endl;
  if (!method.compare("GET") && access(ressource.c_str(), R_OK) == -1)
    return false;

  if (!method.compare("POST") && access(ressource.c_str(), W_OK) == -1 && access(Parser::getFolder(ressource).c_str(), F_OK) == -1)
    return false;
  
  if (!method.compare("DELETE") && access(ressource.c_str(), X_OK) == -1)
    return false;

  return true;
}

bool  Server::checkPathType(std::string method, std::string& ressource, Location* location)
{
  bool  allowed = this->isListingAllowed();
  if (location && location->isset("allow_listing"))
    allowed = location->isListingAllowed();
  struct stat sb;
  if (stat(ressource.c_str(), &sb) == -1) 
    return false;
  if (!S_ISDIR(sb.st_mode))
    return false;
  if (!method.compare("DELETE"))
    return false;
  if (!method.compare("GET") && allowed)
  {
    this->_response.setIsListing(true);
    return false;
  }
  return true;
}

void  Server::addPathRoot(std::string& ressource, Location* location)
{
  if (location)
  {
    std::string pattern = location->get("key");
    if (location->isset("root"))
      ressource.replace(0, pattern.length(), location->get("root"));
    else
      ressource.replace(0, pattern.length(), this->_root);
  }
  else
    ressource.insert(0, this->_root);
  
  if (ressource[0] != '/')
    ressource.insert(0, "/");  
  
  ressource.insert(0, this->_ctx);
}

void  Server::addPathIndex(std::string& ressource, Location* location)
{
  if (ressource[ressource.length() - 1] != '/')
    ressource.append("/");
  if (location && location->isset("index")) 
    ressource.append(location->get("index"));
  else
    ressource.append(this->_index);
}

Location* Server::getLocation(const std::string& ressource)
{
  for (std::map<std::string, Location*>::iterator it = this->_locations.begin(); it != this->_locations.end(); ++it)
  {
    if (ressource.find(it->first) == 0)
      return it->second;
  }
  return NULL;
}

std::string Server::handleCgi()
{
  std::cout << "Handle Cgi !" << std::endl;
  // Fork
  // Execve
  // Waitpid
  return "";
}

std::string Server::handleAction(const std::string& ressource, Request& request, Location* location)
{
  std::string method = request.getMethod();
  // Verifier si la method est allow 
  if (!method.compare("GET"))
    return this->handleGet(ressource);
  if (!method.compare("DELETE"))
    return this->handleDelete(ressource, location);
  if (!method.compare("POST"))
    return this->handlePost(ressource, request.getBody(), location);
  std::string err = ressource;
  this->handleError(AHttpMessage::NOT_ALLOWED, err, location);
  return this->handleGet(err);
}

std::string Server::handleGet(const std::string& path)
{
  // Verifier si le listing est allow 
  if (this->isListingAllowed() && this->_response.getIsListing())
    return this->handleListing(path);
  std::ifstream ressource(path.c_str());
  if (!ressource)
    return "";
  std::string line;
  std::string content;
  while (std::getline(ressource, line))
  {
    content.append(line);
  }
  ressource.close();
  return content;
}

std::string Server::handleDelete(const std::string& path, Location* location)
{
  if (this->_response.getResponseCode().compare("200 OK") || std::remove(path.c_str()) == -1)
  {
    std::string ressource = path;
    this->handleError(this->_response.getResponseCode(), ressource, location);
    return this->handleGet(ressource);
  }
  return "";
}

std::string Server::handlePost(const std::string& path, const std::string body, Location* location)
{
  std::ofstream file(path.c_str());
  if (!file || this->_response.getResponseCode().compare("200 OK"))
  {
    std::string ressource = path;
    this->handleError(this->_response.getResponseCode(), ressource, location);
    return this->handleGet(ressource);
  }  
  file << body;
  file.close();
  return "";
}

std::string  Server::handleListing(const std::string& path)
{
  std::cout << path << std::endl;
  return "OKAYY le listing est good";
}

void  Server::handleError(std::string code, std::string& ressource, Location* location)
{
  std::cout << "Handle error : " << code << std::endl;
  
  this->_response.setResponseCode(code);

  std::string error_default = "error.html";

  if (location && location->isset("error_page"))
  {
    std::string error_page = location->get("error_page");
    if (access(error_page.c_str(), R_OK) == 0)
    {
      ressource = error_page;
      return ;
    }
  }

  if (access(this->_error_page.c_str(), R_OK) == 0)
    ressource = this->_error_page;
  else
    ressource = error_default;

  if (ressource[0] != '/')
    ressource.insert(0, "/");  
  
  ressource.insert(0, this->_ctx_err);
  return ;
}

bool  Server::send(int fd)
{
  std::string response = this->_response.get();
  std::size_t bsend = ::send(fd, response.c_str(), response.length(), MSG_NOSIGNAL);
  this->_response.clean();
  return bsend > 0;
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

bool  Server::isListingAllowed() const 
{
  return !this->_allow_listing.compare("true") || !this->_allow_listing.compare("TRUE");
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
