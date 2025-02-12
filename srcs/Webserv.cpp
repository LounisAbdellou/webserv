#include "Webserv.hpp"

Webserv::Webserv()
{
  Validator::init();
  _setters["max_body_size"] = &Webserv::setMaxBodySize;
  if ((this->_epoll_fd = epoll_create1(0)) == -1)
    this->throwError("Fail to create epoll instance");
}

Webserv::Webserv(const std::string& configFile)
{
  (void)configFile;
}

Webserv::Webserv(const Webserv& cpy)
{
  (void)cpy;
}

Webserv& Webserv::operator=(Webserv& cpy)
{
  return cpy;
}

Webserv::~Webserv()
{
  for (std::vector<Server*>::iterator it = this->_servers.begin(); it != this->_servers.end(); ++it) 
  {
    delete *it;
    *it = NULL;
  }
  for (std::map<int, std::queue<Server*>*>::iterator it = this->_sockets.begin(); it != this->_sockets.end(); ++it)
  {
    delete it->second;
    it->second = NULL;
  }
  for (std::map<std::string, int>::iterator it = this->_hosts.begin(); it != this->_hosts.end(); ++it)
  {
    if (it->second > -1)
    {
      shutdown(it->second, SHUT_RDWR);
      close(it->second);
    }
  }
  for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
  {
    if (it->first > -1)
    {
      shutdown(it->first, SHUT_RDWR);
      close(it->first);
    }
    if (it->second)
    {
      delete it->second;
      it->second = NULL;
    }
  }
  close(this->_epoll_fd);
  std::cout << "Cleaning up and closing Webserv ! Bye !" << std::endl;
}

bool  Webserv::is_running = true; 

bool  Webserv::configure(const std::string& configFile)
{
  std::ifstream file(configFile.c_str());

  std::cout << "Configuration from file : '" << configFile << "'" << std::endl;

  if (!file)
    Parser::throwError("No valid file to open");

  std::string line;
  std::string key;

  Parser::handleFile(file);

  std::cout << "Parsing file and configure servers" << std::endl;

  while (std::getline(file, line))
  {
    if (std::cin.eof())
      Parser::throwError("Problem occurs while reading file");

    if (line.find_first_not_of(" \n\r\t") > line.length())
      continue;

    if (line.find("}") < line.length() && Parser::handleClosure(line))
      break;
    
    if (line.find("server") < line.length() && Parser::handleBlock(line, "server"))
    {
      this->configureServer(file);
      continue;
    }

    key = Parser::getKey(line);
    
    if (this->_setters.find(key) == this->_setters.end())
      Parser::throwError("Invalid webserv's instruction");
      
    (this->*_setters[key])(Parser::getValue(key, line));
  }

  while (std::getline(file, line))
  {
    if (line.find_first_not_of("\n\r\t ") < line.length())
      Parser::throwError("Definition outside webserv block...");
  }

  file.close();
  return true;
}

void  Webserv::configureServer(std::ifstream& file)
{
  Server*     server = new Server();
  std::string line;
  std::string key;
  
  this->_servers.push_back(server);

  while (std::getline(file, line))
  {
    if (std::cin.eof())
      Parser::throwError("Problem occurs while reading file");

    if (line.find_first_not_of(" \n\r\t") > line.length())
      continue;

    if (line.find("}") < line.length() && Parser::handleClosure(line))
      break;
    
    if (line.find("location") < line.length() && Parser::handleBlock(line, "location"))
    {
      this->configureLocation(file, server, Parser::getLocation(line));
      continue;
    }

    key = Parser::getKey(line);
    
    if (!server->has(key))
      Parser::throwError("Invalid server's instruction");
      
    server->set(key, Parser::getValue(key, line));
  }
}

void  Webserv::configureLocation(std::ifstream& file, Server* server, const std::string path)
{
  Location*   location = new Location();
  std::string line;
  std::string key;
  
  if (!server->set(path, location))
    Parser::throwError("Try to rewrite location.", location);
  
  while (std::getline(file, line))
  {
    if (std::cin.eof())
      Parser::throwError("Problem occurs while reading file.");

    if (line.find_first_not_of(" \n\r\t") > line.length())
      continue;

    if (line.find("}") < line.length() && Parser::handleClosure(line))
      break;
    
    key = Parser::getKey(line);
    
    if (!location->has(key))
      Parser::throwError("Invalid location's instruction.");
      
    location->set(key, Parser::getValue(key, line));
  }
}

void  Webserv::init()
{
  std::cout << "Init servers..." << std::endl;
  for (std::vector<Server*>::iterator server = this->_servers.begin(); server != this->_servers.end(); ++server)
  {
    if (!((*server)->isset("listen")))
      (*server)->set("listen", "80");
    this->initServer(*server);
    std::cout << **server << std::endl;
  }
}

void  Webserv::initServer(Server* server)
{
  for (std::vector<std::string>::iterator it = server->iterator(0); it != server->iterator(-1); ++it)
  {
    if (this->_hosts.find(*it) == this->_hosts.end())
    {
        this->_hosts[*it] = socket(AF_INET, SOCK_STREAM, 0);
        this->initSocket(*it, this->_hosts[*it]);
    }
    
    if (this->_hosts[*it] == -1)
      this->throwError("Fail to open socket.");

    if (this->_sockets.find(this->_hosts[*it]) == this->_sockets.end())
    {
      std::queue<Server*>* q = new std::queue<Server*>();
      this->_sockets[this->_hosts[*it]] = q;
    }
    
    this->_sockets[this->_hosts[*it]]->push(server);
    
    if (server->isset("server_name"))
    {
      if (this->_names.find(server->get("server_name")) != this->_names.end())
        this->throwError("Try to attribute server_name already given");
      this->_names[server->get("server_name")] = server;
    }
  }
}

void  Webserv::initSocket(std::string& host, int socket_fd) const
{
  if (socket_fd < 0)
    return;
  
  std::string ip = host.substr(0, host.find(":")); 
  std::string port = host.substr(host.find(":") + 1, host.length()); 

  int opt = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(std::atoi(port.c_str()));
  if (!ip.compare("0.0.0.0"))
    address.sin_addr.s_addr = INADDR_ANY;
  else
    inet_pton(AF_INET, ip.c_str(), &address.sin_addr);
  
  if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
  {
    if (errno == EADDRINUSE || errno == EACCES || errno == EADDRNOTAVAIL)
    {
      address.sin_port = 0;
      if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
        this->throwError("Fail at binding socket to host.");
      // Log le changement de port en utilisant getaddrinfo et en logant avec une classe dedie
      std::cout << "An other port is used to listen socket_fd " << socket_fd << std::endl;
    } 
    else
      this->throwError("Fail at binding socket to host.");
  }

  if (listen(socket_fd, 10) < 0)
    this->throwError("Fail at put socket on listening mode.");

  struct epoll_event  e;

  e.events = EPOLLIN;
  e.data.fd = socket_fd;

  if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, socket_fd, &e) == -1)
    this->throwError("Adding socket to epoll failed.");

}

void  Webserv::run()
{
  std::cout << "Webserv run !" << std::endl;
  struct epoll_event  e;
  struct epoll_event  events[10];
  int                 nb_events = 0;

  while (Webserv::is_running)
  {
    if ((nb_events = epoll_wait(this->_epoll_fd, events, 10, -1)) == -1)
      break;

    for (int i = 0; i < nb_events; i++)
    {
      if (this->_sockets.find(events[i].data.fd) != this->_sockets.end())
      {
        struct sockaddr_in c_addr;
        socklen_t addr_len = sizeof(c_addr);
        int client_fd = accept(events[i].data.fd, (struct sockaddr *)&c_addr, &addr_len);
        if (client_fd < 0)
          this->throwError("Accept failed");
				int flag = fcntl(client_fd, F_GETFL, 0);
        if (fcntl(client_fd, F_SETFL, flag | O_NONBLOCK) == -1)
          this->throwError("fcntl failed");
        e.events = EPOLLIN;
        e.data.fd = client_fd;
        Client* client = new Client(events[i].data.fd, client_fd);
        epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, client_fd, &e);
        if (this->_clients[client_fd])
          delete this->_clients[client_fd];
        this->_clients[client_fd] = client;
      }
      else if (this->_clients.find(events[i].data.fd) != this->_clients.end())
      {
        Client* client = this->_clients[events[i].data.fd];
        if (client->receive())
        {
          Server* server = this->_sockets[client->getServerFd()]->front();
          server->handle(client->getRequest());
          server->send(events[i].data.fd);
        }
        if (client->isClose())
          this->close(events[i].data.fd);
      }
    }
  }
}

void  Webserv::close(int fd)
{
  if (this->_clients.find(fd) == this->_clients.end())
  {
    if (this->_clients[fd])
      delete this->_clients[fd];
    this->_clients.erase(fd);
  }
  shutdown(fd, SHUT_RDWR);
  ::close(fd);
}

void  Webserv::setMaxBodySize(const std::string value)
{
  this->_max_body_size = value;
}

void  Webserv::throwError(std::string message) const
{
  throw Webserv::WebservException(message.insert(0, "Webserv Error : "));
}
