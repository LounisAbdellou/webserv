#include "Server.hpp"

Server::Server() : _ctx("./var/www"), _ctx_err("./var/error") 
{
  Location *location = new Location;
  location->set("key", "/");
  _locations["/"] = location;

  _cgi_env[0] = "REQUEST_METHOD";
  _cgi_env[1] = "QUERY_STRING";
  _cgi_env[2] = "CONTENT_TYPE";
  _cgi_env[3] = "CONTENT_LENGTH";
  _cgi_env[4] = "SCRIPT_FILENAME";
  _cgi_env[5] = "SERVER_NAME";
  _cgi_env[6] = "SERVER_PORT";
  _cgi_env[7] = "REMOTE_ADDR";
  _cgi_env[8] = "HTTP_COOKIE";
  _cgi_env[9] = "PATH_INFO";
  _cgi_env[10] = "HTTP_USER_AGENT";
  _cgi_env[11] = "PHP_SELF";
  _cgi_env[12] = "PYTHONPATH";

  this->init();
}

Server::Server(const Server &cpy) { (void)cpy; }

Server &Server::operator=(Server &cpy) { return cpy; }

Server::~Server() 
{
  for (std::map<std::string, Location *>::iterator it =
           this->_locations.begin();
       it != this->_locations.end(); ++it) {
    delete it->second;
    it->second = NULL;
  }
}

std::ostream &operator<<(std::ostream &cout, const Server &server) 
{
  cout << "Server up and listenning at : " << std::endl;
  for (std::vector<std::string>::const_iterator it = server.iterator(0);
       it != server.iterator(-1); ++it) {
    std::cout << "=> " << *it << std::endl;
  }
  return cout;
}

void  Server::init()
{
  _setters["listen"] = &Server::setListen;
  _setters["server_name"] = &Server::setServerName;
  _setters["root"] = &Server::setRoot;
  _setters["index"] = &Server::setIndex;
  _setters["error_page"] = &Server::setErrorPage;
  _setters["allowed_method"] = &Server::setAllowedMethod;
  _setters["allow_listing"] = &Server::setAllowListing;
  _setters["redirect"] = &Server::setRedirect;
  _setters["server_name"] = &Server::setServerName;

  _getters["server_name"] = &Server::getServerName;
  _getters["listen"] = &Server::getListen;
  _getters["index"] = &Server::getIndex;
}

void Server::set(const std::string key, const std::string value) 
{
  if (this->_setters.find(key) == this->_setters.end()) return ;
  (this->*_setters[key])(value);
}

bool Server::has(const std::string key) const 
{
  return !(this->_setters.find(key) == this->_setters.end());
}

bool Server::isset(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return false;
  return (!(this->*_getters.at(key))().empty());
}

std::string Server::get(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return "";
  return (this->*_getters.at(key))();
}

bool Server::set(const std::string key, Location *location) 
{
  if (this->_locations.find(key) != this->_locations.end())
    return false;
  location->set("key", key);
  this->_locations[key] = location;
  return true;
}

std::vector<std::string>::iterator  Server::iterator(int pos)
{
  if (pos < 0 || pos > (int)this->_listen.size())
    return this->_listen.end();
  std::vector<std::string>::iterator it = this->_listen.begin();
  for (int i = 0; i < pos; i++) {
    it++;
  }
  return it;
}

std::vector<std::string>::const_iterator Server::iterator(int pos) const 
{
  if (pos < 0 || pos > (int)this->_listen.size())
    return this->_listen.end();
  std::vector<std::string>::const_iterator it = this->_listen.begin();
  for (int i = 0; i < pos; i++) {
    it++;
  }
  return it;
}

std::string Server::handle(const std::string& path, Request& request, Response& response) 
{
  std::string ressource = path;
  std::string code = this->handleRessource(ressource, request, response);
  
  if (request.get("type").compare("PIPE"))
    this->setContentLength(ressource, request, response);
  response.set("header", code);

  return ressource;
}

std::string Server::handleRessource(std::string& ressource, Request& request, Response& response)
{
  Location *location = this->getLocation(ressource);

  if (request.isset("error"))
    return this->handleError("400", ressource, location);

  if (!location->isset("allowed_method", request.get("method")))
    return this->handleError("405", ressource, location);

  if (location->isset("redirect"))
  {
    response.add("Location", location->get("redirect", "url"));
    return location->get("redirect", "code");
  }
  
  this->addPathRoot(ressource, location);

  if (this->checkPathType(request, ressource, location))
    this->addPathIndex(ressource, location);

  if (access(Parser::getFolder(ressource).c_str(), F_OK) == -1 || 
    (access(ressource.c_str(), F_OK) == -1 && request.get("method").compare("POST")))
    return this->handleError("404", ressource, location);

  if (!this->checkPathAccess(request.get("method"), ressource))
    return this->handleError("403", ressource, location);

  return "200";
}

bool Server::checkPathAccess(std::string method, std::string &ressource) 
{
  if (!method.compare("GET") && access(ressource.c_str(), R_OK) == -1)
    return false;

  if (!method.compare("POST") && access(ressource.c_str(), W_OK) == -1 &&
      access(Parser::getFolder(ressource).c_str(), F_OK) == -1)
    return false;

  if (!method.compare("DELETE") && access(ressource.c_str(), X_OK) == -1)
    return false;

  return true;
}

bool Server::checkPathType(Request& request, std::string &ressource, Location *location) 
{
  std::string method = request.get("method");
  struct stat sb;
  if (stat(ressource.c_str(), &sb) == -1)
    return false;
  if (!S_ISDIR(sb.st_mode))
    return false;
  if (!method.compare("DELETE"))
    return false;
  if (!method.compare("GET") && location->isset("allow_listing"))
  {
    request.set(Request::E_REQUEST_LIST);
    return false;
  }
  return true;
}

void Server::addPathRoot(std::string &ressource, Location *location) 
{
  std::string pattern = location->get("key");
  if (pattern.compare("/")) {
    if (location->isset("root"))
      ressource.replace(0, pattern.length(), location->get("root"));
    else
      ressource.replace(0, pattern.length(), this->get("root"));
  } else
    ressource.insert(0, location->get("root"));

  if (ressource[0] != '/')
    ressource.insert(0, "/");

  ressource.insert(0, this->_ctx);
}

void Server::addPathIndex(std::string &ressource, Location *location) 
{
  if (ressource[ressource.length() - 1] != '/')
    ressource.append("/");
  if (location->isset("index"))
    ressource.append(location->get("index"));
  else
    ressource.append(this->get("index"));
}

void Server::setContentLength(const std::string& ressource, Request& request, Response& response) const
{
  if (request.get("method").compare("GET"))
  {
    response.add("Content-Length", "0");
    return;
  }
  response.set(Response::E_RESPONSE_DOC);
  std::ifstream target(ressource.c_str(), std::ios::binary);
  target.seekg(0, std::ios::end);
  int content_length = target.tellg();
  target.close();
  response.bsend(content_length);
  response.add("Content-Length", Parser::to_string(content_length));
}

std::string Server::handleError(std::string code, std::string& ressource, Location *location) 
{
  if (location->isset("error_page"))
    ressource = location->get("error_page", code);

  if (ressource[0] != '/')
    ressource.insert(0, "/");

  ressource.insert(0, this->_ctx_err);

  struct stat sb;
  if (stat(ressource.c_str(), &sb) == -1 || S_ISDIR(sb.st_mode) ||
      access(ressource.c_str(), R_OK) == -1) {
    ressource = this->_ctx_err.substr(0);
    ressource.append("/error.html");
  }
  return code;
}

void  Server::execute(const std::string& ressource, Request& request, Response& response)
{
  response.set(Response::E_RESPONSE_PIPE);
  if (request.isset("list"))
    return this->listing(ressource, response);
  return this->cgi(ressource, request, response);
}

void  Server::listing(const std::string& ressource, Response& response)
{
  Entry entry;
  std::vector<struct Entry> entries;
  DIR *dir = opendir(ressource.c_str());
  struct dirent *dir_entry = readdir(dir);
  ;

  while (dir_entry != NULL) {
    dir_entry = readdir(dir);

    if (!dir_entry || dir_entry->d_name[0] == '.')
      continue;

    entry.name = dir_entry->d_name;
    entry.isDir = dir_entry->d_type == DT_DIR;

    entries.push_back(entry);
  }

  closedir(dir);

  std::string listing = Parser::getListingHtml(entries, ressource);

  int* res = response.pipe("open");
  
  ::write(res[1], listing.c_str(), listing.length());
  
  response.bsend(listing.length());

  response.add("Content-Length", Parser::to_string(response.bsend()));

}

std::string Server::setCgi(const std::string& ressource, Request& request, bool is_php)
{
  std::vector<std::string> host = Parser::getSocketInfo(request.socket());
  
  setenv(this->_cgi_env[0].c_str(), request.get("method").c_str(), 1); // PB
  setenv(this->_cgi_env[1].c_str(), request.get("query").c_str(), 1);
  setenv(this->_cgi_env[2].c_str(), request.get("args", "Content-Type").c_str(), 1);
  setenv(this->_cgi_env[3].c_str(), request.get("args", "Content-Length").c_str(), 1);
  setenv(this->_cgi_env[4].c_str(), ressource.c_str(), 1); // PB
  setenv(this->_cgi_env[5].c_str(), this->get("server_name").c_str(), 1);
  setenv(this->_cgi_env[6].c_str(), host[1].c_str(), 1);
  setenv(this->_cgi_env[7].c_str(), host[0].c_str(), 1);
  setenv(this->_cgi_env[8].c_str(), request.get("args", "Cookie").c_str(), 1);
  setenv(this->_cgi_env[9].c_str(), ressource.c_str(), 1); // PB
  setenv(this->_cgi_env[10].c_str(), request.get("args", "User-Agent").c_str(), 1); // PB
  if (is_php)
  {
    setenv("REDIRECT_STATUS", "200", 1);
    setenv("PHP_SELF", ressource.c_str(), 1);
    return "/bin/php-cgi";
  }
  
  setenv("PYTHONPATH", "/bin/python3.10", 1);
  return "/bin/python3";
}

void  Server::cgi(const std::string& ressource, Request& request, Response& response)
{
  std::string cgi = this->setCgi(ressource, request, !Parser::getExtension(ressource).compare(".php"));

  int* req = request.pipe("open");
  int* res = response.pipe("open");
  int inter[] = { -1, -1 }; 

  const char *args[] = {cgi.c_str(), ressource.c_str(), NULL};
  
  pipe(inter);
  pid_t pid = fork();
  if (pid == 0)
  {
    dup2(req[0], 0);
    dup2(inter[1], 1);
    request.pipe("close");
    response.pipe("close");
    close(inter[0]);
    close(inter[1]);
    execvp(cgi.c_str(), (char**)args);
  }
  request.pipe("close");
  close(inter[1]);
  waitpid(pid, NULL, 0);
  
  char buffer[BUFFER_SIZE + 1];
  
  ::bzero(buffer, BUFFER_SIZE + 1);

  int offset = 0;
  
  if (!Parser::getExtension(ressource).compare(".php"))
  {
    ::read(inter[0], buffer, BUFFER_SIZE);

    std::string str = buffer;

    if (str.find("\r\n\r\n") != std::string::npos)
      offset = str.find("\r\n\r\n") + 4;
    
    response.bsend(::write(res[1], buffer, ::strlen(buffer)));
    
    ::bzero(buffer, BUFFER_SIZE + 1);
  }

  while (::read(inter[0], buffer, BUFFER_SIZE))
  {
    response.bsend(::write(res[1], buffer, ::strlen(buffer)));
    ::bzero(buffer, BUFFER_SIZE + 1);
  }
  response.add("Content-Length", Parser::to_string(response.bsend() - offset));
  response.set(Response::E_RESPONSE_CGI);
  close(inter[0]);
}

void Server::setListen(const std::string &value) 
{
  std::string listen = value;
  if (!Validator::validate("listen", listen))
    Validator::throwError("Invalid couple 'ip:port'.");
  if (std::find(this->_listen.begin(), this->_listen.end(), listen) !=
      this->_listen.end())
    Validator::throwError("This couple 'ip:port' already exist");
  this->_listen.push_back(listen);
}


void Server::setServerName(const std::string &value) 
{ 
  this->_server_name = value; 
}

void Server::setRoot(const std::string &value) 
{
  this->_locations.at("/")->set("root", value);
}

void Server::setIndex(const std::string &value) 
{
  std::string path = value;
  if (!Validator::validate("file", path))
    Validator::throwError("Invalid file for index.");
  this->_locations.at("/")->set("index", value);
}

void Server::setErrorPage(const std::string &value) 
{
  std::string path = value;
  if (!Validator::validate("file", path))
    Validator::throwError("Error in error_page instruction.");
  this->_locations.at("/")->set("error_page", value);
}

void  Server::setAllowedMethod(const std::string& value)
{
  this->_locations.at("/")->set("allowed_method", value);
}

void Server::setAllowListing(const std::string &value) 
{
  this->_locations.at("/")->set("allow_listing", value);
}

void Server::setRedirect(const std::string &value) 
{
  this->_locations.at("/")->set("redirect", value);
}

Location* Server::getLocation(const std::string& ressource)
{
  for (std::map<std::string, Location*>::iterator it = this->_locations.begin(); it != this->_locations.end(); ++it)
  {
    if (ressource.find(it->first) == 0 && it->first.compare("/") && (ressource.length() == it->first.length() || ressource[it->first.length()] == '/'))
      return it->second;
  }
  return this->_locations.at("/");
}

std::string Server::getServerName() const 
{ 
  return this->_server_name; 
}

std::string Server::getListen() const 
{ 
  return this->_listen[0]; 
}

std::string Server::getIndex() const 
{
  return this->_locations.at("/")->get("index");
}

std::string Server::getRoot() const 
{
  return this->_locations.at("/")->get("root");
}

std::string Server::getAllowedMethod() const 
{
  return this->_locations.at("/")->get("allowed_method");
}

std::string Server::getAllowListing() const 
{
  return this->_locations.at("/")->get("allow_listing");
}

std::string Server::getRedirect() const 
{
  return this->_locations.at("/")->get("redirect");
}

std::string Server::getErrorPage() const 
{
  return this->_locations.at("/")->get("error_page");
}
