#include "Response.hpp"
#include "Parser.hpp"

Response::Response() : _isListing(false) { this->_responseCode = OK; }

Response::Response(const Response &src) { *this = src; }

Response &Response::operator=(const Response &src) {
  if (this != &src) {
    this->_responseCode = src._responseCode;
  }

  return *this;
}

std::string Response::get() const {
  std::string response = this->_header + "\r\n" + this->_body;

  return response;
}

void Response::setAttribute(const std::string &key, const std::string &value) {
  this->_attributes[key] = value;
}

void Response::generate(const std::string &fragment, Request &request) {
  std::map<std::string, std::string>::const_iterator it;

  if (this->_responseCode == CREATED) {
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
}

void Response::clean() {
  this->_responseCode = OK;
  this->_header.clear();
  this->_body.clear();
  this->_attributes.clear();
  this->_isListing = false;
}

bool Response::getIsListing() const { return this->_isListing; }

void Response::setIsListing(bool value) { this->_isListing = value; }
