#include "Response.hpp"
#include "Parser.hpp"

const std::string Response::OK = "200 OK";
const std::string Response::CREATED = "201 Created";
const std::string Response::MOVED_PERM = "301 Moved Permanently";
const std::string Response::FOUND = "302 Found";
const std::string Response::TEMP_REDIRECT = "307 Temporary Redirect";
const std::string Response::PERM_REDIRECT = "308 Permanent Redirect";
const std::string Response::BAD_REQUEST = "400 Bad Request";
const std::string Response::NOT_FOUND = "404 Not Found";
const std::string Response::FORBIDDEN = "403 Forbidden";
const std::string Response::NOT_ALLOWED = "405 Method Not Allowed";
const std::string Response::LENGTH_REQUIRED = "411 Length Required";
const std::string Response::PAYLOAD_TOO_LARGE = "413 Payload Too Large";
const std::string Response::HEADER_TOO_LARGE = "431 Request Header Fields Too Large";
const std::string Response::INTERNAL_SERVER_ERROR = "500 Internal Server Error";
const std::string Response::GAT_TIMEOUT = "504 Gateway Timeout";
const std::string Response::HTTP_VERSION = "505 HTTP Version Not Supported";

Response::Response() : _status(E_RESPONSE_CREATED), _type(E_RESPONSE_OTHER)
{
  _codes["200"] = OK;
  _codes["201"] = CREATED;
  _codes["301"] = MOVED_PERM;
  _codes["302"] = FOUND;
  _codes["307"] = TEMP_REDIRECT;
  _codes["308"] = PERM_REDIRECT;
  _codes["400"] = BAD_REQUEST;
  _codes["404"] = NOT_FOUND;
  _codes["403"] = FORBIDDEN;
  _codes["405"] = NOT_ALLOWED;
  _codes["411"] = LENGTH_REQUIRED;
  _codes["413"] = PAYLOAD_TOO_LARGE;
  _codes["431"] = HEADER_TOO_LARGE;
  _codes["500"] = INTERNAL_SERVER_ERROR;
  _codes["504"] = GAT_TIMEOUT;
  _codes["505"] = HTTP_VERSION;

  this->_pipe[0] = -1;
  this->_pipe[1] = -1;
  this->_bsend = 0;

  this->init();
}

Response::Response(const Response &src) { (void)src; }

Response &Response::operator=(Response &src) { return src; }

Response::~Response() { this->clean(); }

void    Response::init()
{
  _setters["header"] = &Response::setHeader;
  _setters["pipe"] = &Response::setPipe;
  
  _getters["header"] = &Response::getHeader;
  _getters["status"] = &Response::getStatus;
  _getters["done"] = &Response::getDone;
  _getters["type"] = &Response::getType;
  _getters["cgi"] = &Response::getType;
  
}

void Response::set(const std::string key, std::string value) 
{
  if (this->_setters.find(key) == this->_setters.end()) return ;
  (this->*_setters[key])(value);
}

void  Response::add(const std::string key, std::string value)
{
  this->_args[key] = value;
}

bool Response::has(const std::string key) const 
{
  return !(this->_setters.find(key) == this->_setters.end());
}

bool Response::isset(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return false;
  return (!(this->*_getters.at(key))().empty());
}

std::string Response::get(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return "";
  return (this->*_getters.at(key))();
}

void  Response::set(Response::Status status)
{
  this->_status = status;
}

void  Response::set(Response::Type type)
{
  this->_type = type;
}

int*  Response::pipe(const std::string value)
{
  this->set("pipe", value);
  return this->_pipe;
}

void    Response::setHeader(std::string& value)
{
  this->_header = "HTTP/1.1 " + this->_codes[value] + "\r\n";
}

void  Response::setPipe(std::string& value)
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

std::string Response::getHeader() const
{
  std::string res = this->_header;
  for (std::map<std::string, std::string>::const_iterator it = this->_args.begin(); it != this->_args.end(); it++) 
  {
    res.append(it->first + ": " + it->second + "\r\n");
  }
  return res;
}

std::string Response::getType() const
{
  if (this->_type == E_RESPONSE_DOC)
    return "DOC";
  if (this->_type == E_RESPONSE_PIPE)
    return "PIPE";
  
  return ""; 
}

std::string Response::getStatus() const
{
  return this->_status > E_RESPONSE_CGI ? "OK" : ""; 
}

std::string Response::getDone() const
{
  return this->_status == E_RESPONSE_COMPLETE ? "OK" : ""; 
}

std::string Response::getCgi() const
{
  return this->_status == E_RESPONSE_CGI ? "OK" : ""; 
}

int Response::bsend(int bwrite)
{
  this->_bsend += bwrite;
  return this->_bsend;
}

void Response::clean() 
{
  this->set(E_RESPONSE_CREATED);
  this->set(E_RESPONSE_OTHER);
  this->_header.clear();
  this->_args.clear();
  if (this->_pipe[0] > 1)
    ::close(this->_pipe[0]);
  this->_pipe[0] = -1;
  if (this->_pipe[1] > 1)
    ::close(this->_pipe[1]);
  this->_pipe[1] = -1;
  this->_bsend = 0;
}
