#include "Response.hpp"
#include "Parser.hpp"

Response::Response() : _isListing(false), _isRedirect(false) {
  this->_responseCode = OK;
  _status_codes["200"] = OK;
  _status_codes["201"] = CREATED;
  _status_codes["301"] = MOVED_PERM;
  _status_codes["302"] = FOUND;
  _status_codes["307"] = TEMP_REDIRECT;
  _status_codes["308"] = PERM_REDIRECT;
  _status_codes["400"] = BAD_REQUEST;
  _status_codes["404"] = NOT_FOUND;
  _status_codes["403"] = FORBIDDEN;
  _status_codes["405"] = NOT_ALLOWED;
  _status_codes["411"] = LENGTH_REQUIRED;
  _status_codes["431"] = HEADER_TOO_LARGE;
  _status_codes["500"] = INTERNAL_SERVER_ERROR;
  _status_codes["505"] = HTTP_VERSION;
}

Response::Response(const Response &src) { *this = src; }

Response &Response::operator=(const Response &src) {
  if (this != &src) {
    this->_responseCode = src._responseCode;
  }

  return *this;
}

std::string Response::get() const { return this->_message; }

std::string Response::get(const std::string &status) const {
  if (this->_status_codes.find(status) == this->_status_codes.end())
    return OK;
  return this->_status_codes.at(status);
}

void Response::setAttribute(const std::string &key, const std::string &value) {
  this->_attributes[key] = value;
}

void Response::erase(size_t length) { this->_message.erase(0, length); }

void Response::generate(const std::string &fragment, Request &request) {
  std::map<std::string, std::string>::const_iterator it;

  if (this->_responseCode == CREATED && !_isRedirect) {
    this->setAttribute("Location",
                       "http://" + request.getHost() + request.getPath());
  }

  this->setAttribute("Content-Length", Parser::to_string(fragment.size()));
  this->setAttribute("Server", "Webserv");

  this->_header.append("HTTP/1.1 " + this->_responseCode + "\r\n");

  for (it = this->_attributes.begin(); it != this->_attributes.end(); it++) {
    this->_header.append(it->first + ": " + it->second + "\r\n");
  }

  this->_body = fragment;
  this->_message = this->_header + "\r\n" + this->_body;
}

void Response::generate(const std::string &fragment) {
  this->_message = fragment;
}

void Response::clean() {
  this->_responseCode = OK;
  this->_header.clear();
  this->_body.clear();
  this->_message.clear();
  this->_attributes.clear();
  this->_isListing = false;
}

std::string Response::getMessage() const { return this->_message; }

bool Response::getIsListing() const { return this->_isListing; }

void Response::setIsListing(bool value) { this->_isListing = value; }

bool Response::getIsRedirect() const { return this->_isRedirect; }

void Response::setIsRedirect(bool value) { this->_isRedirect = value; }
