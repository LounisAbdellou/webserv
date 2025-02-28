#include "Server.hpp"

Server::Server() : _ctx("./var/www"), _ctx_err("./var/error")
{
  Location* default_location = new Location;
  default_location->set("key", "/");
  _locations["/"] = default_location;
  
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
  if (this->_getters.find(key) == this->_getters.end())
    return "";
  return (this->*_getters.at(key))();
}

void  Server::setDefault()
{
  if (this->_listen.empty())
    this->set("listen", "80");
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
  
  this->handlePath(ressource, request, location);

  std::string ext = Parser::getExtension(ressource);
  std::string body;

  if (!ext.compare(".php") || !ext.compare(".py"))
    body = this->handleCgi();
  else 
    body = this->handleAction(ressource, request, location);

  this->_response.generate(body, request);
  request.clean();
}

void  Server::handlePath(std::string& ressource,  Request& request, Location* location)
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
  bool  allowed = location->isset("allow_listing");
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
  std::string pattern = location->get("key");
  if (pattern.compare("/"))
  {
    if (location->isset("root"))
      ressource.replace(0, pattern.length(), location->get("root"));
    else
      ressource.replace(0, pattern.length(), this->get("root"));
  }
  else
    ressource.insert(0, location->get("root"));
  
  if (ressource[0] != '/')
    ressource.insert(0, "/");  
  
  ressource.insert(0, this->_ctx);
}

void  Server::addPathIndex(std::string& ressource, Location* location)
{
  if (ressource[ressource.length() - 1] != '/')
    ressource.append("/");
  if (location->isset("index")) 
    ressource.append(location->get("index"));
  else
    ressource.append(this->get("index"));
}

Location* Server::getLocation(const std::string& ressource)
{
  for (std::map<std::string, Location*>::iterator it = this->_locations.begin(); it != this->_locations.end(); ++it)
  {
    if (ressource.find(it->first) == 0 && it->first.compare("/") && (ressource.length() == it->first.length() || ressource[it->first.length() + 1] == '/'))
      return it->second;
  }
  return this->_locations.at("/");
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
  if (location->isset("redirect"))
    return this->handleRedirect(location);
  if (!location->isset("allowed_method", method))
  {
    std::string err = ressource;
    this->handleError(AHttpMessage::NOT_ALLOWED, err, location);
    return this->handleGet(err, location);
  }
  if (!method.compare("GET"))
    return this->handleGet(ressource, location);
  if (!method.compare("DELETE"))
    return this->handleDelete(ressource, location);
  if (!method.compare("POST"))
    return this->handlePost(ressource, request, location);
  
  return "";
}

std::string Server::handleGet(const std::string& path, Location* location)
{
  if (location->isset("allow_listing") && this->_response.getIsListing())
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
    return this->handleGet(ressource, location);
  }
  return "";
}

std::string Server::handlePost(const std::string& path, const Request& request, Location* location)
{
  std::ofstream file;
  if (request.getIsBinary())
    file.open(path.c_str(), std::ios::binary);
  else
    file.open(path.c_str());

  if (!file || this->_response.getResponseCode().compare("200 OK"))
  {
    std::string ressource = path;
    this->handleError(this->_response.getResponseCode(), ressource, location);
    return this->handleGet(ressource, location);
  }  
  file << request.getBody();
  file.close();
  return "";
}

std::string  Server::handleRedirect(Location* location)
{
  std::string code = this->_response.get(location->get("redirect", "code"));

  this->_response.setResponseCode(code);

  this->_response.setIsRedirect(true);

  this->_response.setAttribute("Location", location->get("redirect", "url"));

  return "";
}

std::string  Server::handleListing(const std::string& path)
{
	Entry entry;
	std::vector<struct Entry> entries;
	DIR *dir = opendir(path.c_str());
	struct dirent *dir_entry = readdir(dir);;

	while(dir_entry != NULL) {
		dir_entry = readdir(dir);

		if (!dir_entry || dir_entry->d_name[0] == '.')
			continue ;

		entry.name = dir_entry->d_name;
		entry.isDir = dir_entry->d_type == DT_DIR;

		entries.push_back(entry);
	}

	closedir(dir);

  return Parser::getListingHtml(entries, path);
}

void  Server::handleError(std::string code, std::string& ressource, Location* location)
{
  this->_response.setResponseCode(code);

  if (location->isset("error_page"))
    ressource = location->get("error_page", code);

  if (ressource[0] != '/')
    ressource.insert(0, "/");  

  ressource.insert(0, this->_ctx_err);
  
  struct stat sb;
  if (stat(ressource.c_str(), &sb) == -1 || S_ISDIR(sb.st_mode) || access(ressource.c_str(), R_OK) == -1) 
  {
    ressource = this->_ctx_err.substr(0);
    ressource.append("/error.html");
  }
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

bool  Server::isset(const std::string key) const
{
  if (this->_getters.find(key) == this->_getters.end())
    return false;
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
  this->_locations.at("/")->set("root", value);
}

void  Server::setIndex(std::string& value)
{
  if (!Validator::validate("file", value))
    Validator::throwError("Invalid file for index.");
  this->_locations.at("/")->set("index", value);
}

void  Server::setErrorPage(std::string& value)
{
  if (!Validator::validate("file", value))
    Validator::throwError("Error in error_page instruction.");
  this->_locations.at("/")->set("error_page", value);
}

void  Server::setAllowedMethod(std::string& value)
{
  if (!Validator::validate("method", value))
    Validator::throwError("Error in allowed_method instruction.");
  this->_locations.at("/")->set("allowed_method", value);
}

void  Server::setAllowListing(std::string& value)
{
  this->_locations.at("/")->set("allow_listing", value);
}

void  Server::setRedirect(std::string& value)
{
  this->_locations.at("/")->set("redirect", value);
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
  return this->_locations.at("/")->get("index");
}

std::string  Server::getRoot() const
{
  return this->_locations.at("/")->get("root");
}

std::string  Server::getAllowedMethod() const
{
  return this->_locations.at("/")->get("allowed_method");
}

std::string  Server::getAllowListing() const
{
  return this->_locations.at("/")->get("allow_listing");
}

std::string  Server::getRedirect() const
{
  return this->_locations.at("/")->get("redirect");
}

std::string  Server::getErrorPage() const
{
  return this->_locations.at("/")->get("error_page");
}
