#include "Request.hpp"
#include "AHttpMessage.hpp"
#include "Parser.hpp"
#include <cctype>
#include <iostream>
#include <sstream>
#include <vector>

long long Request::_strtoll(const std::string &str, int base) const {
  long long result = -1;
  std::istringstream iss(str);
  std::string::const_iterator it = str.begin();

  if (*it == '-') {
    return result;
  }

  it++;

  if (base == 10) {
    for (; it != str.end(); it++) {
      if (!std::isdigit(*it)) {
        return result;
      }
    }

    iss >> std::dec >> result;
  } else if (base == 16) {
    for (; it != str.end(); it++) {
      if (!std::isdigit(*it) && *it < 65 && *it > 70 && *it < 97 && *it > 102) {
        return result;
      }
    }

    iss >> std::hex >> result;
  }

  return result;
}

Request::Request() : AHttpMessage() {
  this->_status = REQUEST_HEADER_INCOMPLETE;
  this->_setters["Content-Length"] = &Request::setContentLength;
}

Request::Request(const Request &src) : AHttpMessage(src) { *this = src; }

Request &Request::operator=(const Request &src) {
  if (this != &src) {
    this->_path = src._path;
    this->_method = src._method;
    this->_status = src._status;
    this->_setters = src._setters;
  }

  return *this;
}

int Request::getContentLength() const { return this->_contentLength; }

RequestStatus Request::getStatus() const { return this->_status; }

void Request::set(const std::string &key, const std::string &value) {
  if (this->_setters.find(key) == this->_setters.end()) {
    return;
  }

  (this->*_setters[key])(value);
}

void Request::setContentLength(const std::string &contentLength) {
  this->_contentLength = this->_strtoll(contentLength, 10);
}

void Request::parseHeader() {
  size_t begin = 0;

  std::vector<std::string> requestLine =
      Parser::getRequestLine(this->_rawHeader, begin);
  if (requestLine.size() < 3) {
    return;
  }

  this->_method = requestLine[0];
  this->_path = requestLine[1];
  this->_protocol = requestLine[2];

  while (begin < this->_rawHeader.size()) {
    std::vector<std::string> attribute =
        Parser::getHeaderAttr(this->_rawHeader, begin);
    if (attribute.size() < 2) {
      return;
    }

    this->set(attribute[0], attribute[1]);
  }

  this->_status = REQUEST_INCOMPLETE;
}

void Request::appendRawHeader(std::string &fragment) {
  size_t pos = fragment.find("\r\n\r\n");

  if (pos == std::string::npos) {
    this->_rawHeader.append(fragment);
    this->_rawData.append(fragment);

    return;
  }

  std::string str = fragment.substr(0, pos + 2);
  if (this->_rawHeader.size() + str.size() > 8000) {
    this->_status = REQUEST_BAD;
    return;
  }

  fragment.erase(0, pos + 4);

  this->_rawHeader.append(str);
  this->_rawData.append(str + "\r\n");
  this->parseHeader();
}

void Request::appendRawBody(std::string &fragment) {
  int maxLength = this->getContentLength();
  int remainingLength = maxLength - this->_rawBody.size();

  if (remainingLength < 1) {
    this->_status = REQUEST_COMPLETE;
    return;
  }

  std::string str = fragment.substr(0, remainingLength);
  fragment.erase(0, remainingLength);

  this->_rawBody.append(str);
  this->_rawData.append(str);
}

void Request::appendRawData(std::string &fragment) {
  switch (this->_status) {
  case REQUEST_HEADER_INCOMPLETE:
    this->appendRawHeader(fragment);
  case REQUEST_INCOMPLETE:
    this->appendRawBody(fragment);
  case REQUEST_COMPLETE:
    if (fragment.size() > 0)
      this->_status = REQUEST_BAD;
  default:
    return;
  }
}

void Request::clean() {
  this->_path.clear();
  this->_method.clear();
  this->_protocol.clear();
  this->_rawData.clear();
  this->_rawHeader.clear();
  this->_rawBody.clear();
  this->_contentLength = 0;
  this->_status = REQUEST_HEADER_INCOMPLETE;
}
