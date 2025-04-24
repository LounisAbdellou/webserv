#include "Request.hpp"
#include "Client.hpp"

Request::Request(Client& client) : _client(client), _status(E_REQUEST_CREATED), _type(E_REQUEST_OTHER), _maxBody(false)
{
  _size = 0;
	_chunkSize = 0;
  _pipe[0] = -1;
  _pipe[1] = -1;
  this->init();
}

Request::Request(const Request& src) : _client(src._client) { (void)src; }

Request& Request::operator=(Request& src) { return src; }

Request::~Request() { this->clean(); }

void  Request::init()
{
  _setters["method"] = &Request::setMethod;
  _setters["path"] = &Request::setPath;
  _setters["query"] = &Request::setQuery;
  _setters["protocol"] = &Request::setProtocol;
  _setters["args"] = &Request::setArgs;
  _setters["size"] = &Request::setSize;
  _setters["pipe"] = &Request::setPipe;
  
  _getters["method"] = &Request::getMethod;
  _getters["path"] = &Request::getPath;
  _getters["query"] = &Request::getQuery;
  _getters["protocol"] = &Request::getProtocol;
  _getters["ready"] = &Request::getReady;
  _getters["type"] = &Request::getType;
  _getters["file"] = &Request::getFile;
  _getters["list"] = &Request::getList;
  _getters["cgi"] = &Request::getCgi;
  _getters["max_body"] = &Request::getMaxBody;
}

void Request::set(const std::string key, std::string value) 
{
  if (this->_setters.find(key) == this->_setters.end()) return ;
  (this->*_setters[key])(value);
}

bool Request::has(const std::string key) const 
{
  return !(this->_setters.find(key) == this->_setters.end());
}

bool Request::isset(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return false;
  return (!(this->*_getters.at(key))().empty());
}

std::string Request::get(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return "";
  return (this->*_getters.at(key))();
}

std::string Request::get(const std::string container, const std::string key) const
{
  if (container.compare("args"))
    return "";
  return this->getArgs(key);
}

void  Request::set(Request::Status status)
{
  this->_status = status;
}

void  Request::set(Request::Type type)
{
  this->_type = type;
}

int*  Request::pipe(const std::string value)
{
  this->set("pipe", value);
  return this->_pipe;
}

int   Request::socket() const
{
  return this->_client.socket();
}

void  Request::parse(const char* buffer, size_t bytes)
{
  if (this->_status == E_REQUEST_CREATED)
    this->parseHeader(buffer, bytes);
  else if (this->_status == E_REQUEST_HEADER)
    this->parseArgs(buffer, bytes);
  else if (this->_status == E_REQUEST_ARGS || this->_status == E_REQUEST_BODY)
    this->parseBody(buffer, bytes);
}

void  Request::parseHeader(const char* buffer, size_t bread)
{
  std::string header(buffer, bread);
  std::string attr[3] = { "method", "path", "protocol" };
  size_t      begin = 0;
  size_t      end = 0;

  for (size_t i = 0; i < 3; i++) 
  {
    begin = header.find_first_not_of(" ", end);
    if (i != 2)
     end = header.find_first_of(" ", begin);
    else
     end = header.find_first_of("\n", begin);

    if (end != std::string::npos)
      this->set(attr[i], header.substr(begin, end - begin));
  }

  if (!this->update())
  {
    header.erase(0, end);
    return parseArgs(header.c_str(), bread - end);
  }
}

void  Request::parseArgs(const char* buffer, size_t bread)
{
  std::string header(buffer, bread);
  size_t      body = header.find("\r\n\r\n");
  size_t      begin = 0;
  size_t      end = 0;

  while (end != body)
  {
    if (end != 0)
      begin = end + 1;
    end = header.find("\r\n", begin);
    
    if (end != std::string::npos)
      this->set("args", header.substr(begin, end - begin));
    else
      break;
  }

  if (end == body)
    this->set(E_REQUEST_ARGS);

  if (!this->update())
  {
    header.erase(0, end + 4);
    return parseBody(header.c_str(), bread - (end + 4));
  }
}

void  Request::parseBody(const char* buffer, size_t bread)
{
  if (this->_args["Transfer-Encoding"] == "chunked")
  {
		return this->parseChunk(std::string(buffer, bread));
  }

  this->_size -= this->_client.write((char*)buffer, bread);

  if (this->_size < 0)
  {
    this->set(E_REQUEST_BAD);
    this->_client.remove();
  }

  if (this->_size == 0)
    this->set(E_REQUEST_COMPLETE);
}

void	Request::parseChunk(std::string buffer)
{
  this->_chunk.append(buffer);

  while (true) {
    if (this->_chunkSize < 1) {
      size_t pos = this->_chunk.find("\r\n");
      if (pos == std::string::npos) {
        return;
      }

      this->_chunk.erase(pos, 2);
      this->_chunkContent.append(
          this->_chunk.substr(pos, this->_chunk.size() - pos));
      this->_chunk.erase(pos, this->_chunk.size() - pos);

      this->_chunkSize = Parser::strtoll(this->_chunk, 16);
      // J'crois c la mais j'suis pas sur... 
      this->_size += this->_chunkSize;
      if (this->_client.maxBody() > -1 && this->_size > this->_client.maxBody())
        this->_maxBody = true;
      if (!this->_chunk.compare("0")) {
        this->_status = Request::E_REQUEST_COMPLETE;
        return;
      }

      this->_chunk.clear();
    }

    if (this->_chunk.size() > 0) {
      this->_chunkContent.append(this->_chunk.substr(0, this->_chunk.size()));
      this->_chunk.erase(0, this->_chunk.size());
    }

    if (this->_chunkSize + 2 > this->_chunkContent.size()) {
      this->_chunkSize -= this->_chunkContent.size();
			this->_client.write((char *)this->_chunkContent.substr(0, this->_chunkContent.size()).c_str(),	this->_chunkContent.size());
      this->_chunkContent.erase(0, this->_chunkContent.size());
      return;
    }

		this->_client.write((char *)this->_chunkContent.substr(0, this->_chunkSize).c_str(), this->_chunkSize);
    this->_chunkContent.erase(0, this->_chunkSize);
    this->_chunkSize = 0;

    if (this->_chunkContent.size() > 2) {
      this->_chunk.append(
          this->_chunkContent.substr(2, this->_chunkContent.size() - 2));
    }

    this->_chunkContent.clear();
  }
}

bool  Request::update()
{
  if (this->_status < E_REQUEST_HEADER)
  {
    if (this->isset("method") && this->isset("ressource") && this->isset("protocol"))
      this->set(E_REQUEST_HEADER);
  }

  if (this->_status > E_REQUEST_CREATED)
  {
    if (this->_type <= E_REQUEST_LIST || (this->_type == E_REQUEST_CGI && !this->get("method").compare("GET")))
      this->set(E_REQUEST_COMPLETE);
  }

  return this->_status == E_REQUEST_COMPLETE || this->_status == E_REQUEST_BAD;
}

void  Request::setMethod(std::string& value)
{
  if (value.compare("GET") && value.compare("POST") && value.compare("DELETE"))
    return this->set(E_REQUEST_BAD);

  if (!value.compare("POST"))
    this->set(E_REQUEST_POST);

  this->_method = value;
}

void  Request::setPath(std::string& value)
{
  size_t  query = value.find_first_of("?");
  
  if (query != std::string::npos)
  {
    this->_path = value.substr(0, query);
    this->set("query", value.substr(query + 1));
  }
  else 
    this->_path = value;
  
  std::string ext = Parser::getExtension(this->_path);
  
  if (!ext.compare(".php") || !ext.compare(".py"))
    this->set(E_REQUEST_CGI);
}

void  Request::setQuery(std::string& value)
{
  this->_query = value;
}

void  Request::setProtocol(std::string& value)
{
  this->_protocol = value;
}

void  Request::setArgs(std::string& value)
{
  size_t del = value.find_first_of(":");
  if (del == std::string::npos)
    return this->set(E_REQUEST_BAD);

  size_t val = value.find_first_not_of(" ", del + 1);

  this->_args[value.substr(1, del - 1)] = value.substr(val, value.find_last_not_of(" \r\n\t\v\f"));

  if (value.find("Content-Length") != std::string::npos)
    this->set("size", this->_args["Content-Length"]);
}

void  Request::setPipe(std::string& value)
{
  if (!value.compare("open"))
  {
    if (this->_pipe[0] < 0 && this->_pipe[1] < 0 && ::pipe(this->_pipe) < 0)
      throw Parser::WebservParseException("");
    return ;
  }

  for (size_t i = 0; i < 2; i++) 
  {
    if (this->_pipe[0] > -1)
    {
      ::close(this->_pipe[i]);
      this->_pipe[i] = -1;
    }
  }
}

void  Request::setSize(std::string& value)
{
  this->_size = ::atoi(value.c_str());
  if (this->_client.maxBody() > -1 && this->_size > this->_client.maxBody())
    this->_maxBody = true;
}

std::string  Request::getMethod() const
{
  return this->_method;
}

std::string  Request::getPath() const
{
  return this->_path;
}

std::string  Request::getQuery() const
{
  return this->_query;
}

std::string  Request::getProtocol() const
{
  return this->_protocol;
}

std::string  Request::getArgs(const std::string& key) const
{
  if (this->_args.find(key) == this->_args.end())
    return "";
  return this->_args.at(key);
}

std::string  Request::getReady() const
{
  return this->_status >= E_REQUEST_COMPLETE ? "OK" : "";
}

std::string  Request::getError() const
{
  return this->_status == E_REQUEST_BAD ? "OK" : "";
}

std::string  Request::getType() const
{
  if (this->_type == E_REQUEST_POST)
    return "DOC";
  if (this->_type == E_REQUEST_CGI || this->_type == E_REQUEST_LIST)
    return "PIPE";
  
  return "";
}

std::string  Request::getFile() const
{
  return this->_status == E_REQUEST_BODY ? "OK" : "";
}

std::string  Request::getList() const
{
  return this->_type == E_REQUEST_LIST ? "OK" : "";
}

std::string  Request::getCgi() const
{
  return this->_type == E_REQUEST_CGI ? "OK" : "";
}

std::string  Request::getMaxBody() const
{
  return this->_maxBody ? "OK" : "";
}

void  Request::clean()
{
  this->set(E_REQUEST_CREATED);
  this->set(E_REQUEST_OTHER);
  this->_size = 0;
  this->_maxBody = false;
  this->_method.clear();
  this->_path.clear();
	this->_chunk.clear();
	this->_chunkContent.clear();
  this->_query.clear();
  this->_protocol.clear();
  this->_args.clear();
  if (this->_pipe[0] > 1)
    close(this->_pipe[0]);
  this->_pipe[0] = -1;
  if (this->_pipe[1] > 1)
    close(this->_pipe[1]);
  this->_pipe[1] = -1;
}
