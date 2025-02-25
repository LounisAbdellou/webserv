#include "Request.hpp"
#include "AHttpMessage.hpp"
#include "Parser.hpp"
#include <iostream>
#include <vector>

Request::Request() : AHttpMessage() {
  this->_isChucked = false;
  this->_status = E_REQUEST_HEADER_INCOMPLETE;
  this->_setters["Host"] = &Request::setHost;
  this->_setters["Content-Length"] = &Request::setContentLength;
  this->_setters["Transfer-Encoding"] = &Request::setIsChucked;
}

Request::Request(const Request &src) : AHttpMessage(src) { *this = src; }

Request &Request::operator=(const Request &src) {
  if (this != &src) {
    this->_path = src._path;
    this->_method = src._method;
    this->_protocol = src._protocol;
    this->_status = src._status;
    this->_setters = src._setters;
    this->_isChucked = src._isChucked;
    this->_contentLength = src._contentLength;
  }

  return *this;
}

int Request::getContentLength() const { return this->_contentLength; }

std::string Request::getPath() const { return this->_path; }

std::string Request::getHost() const { return this->_host; }

std::string Request::getMethod() const { return this->_method; }

Request::Status Request::getStatus() const { return this->_status; }

void Request::set(const std::string &key, const std::string &value) {
  if (this->_setters.find(key) == this->_setters.end()) {
    return;
  }

  (this->*_setters[key])(value);
}

void Request::setHost(const std::string &host) { this->_host = host; }

void Request::setIsChucked(const std::string &transferEncoding) {
  if (transferEncoding == "chunked") {
    this->_isChucked = true;
  }
}

void Request::setContentLength(const std::string &contentLength) {
  this->_contentLength = Parser::strtoll(contentLength, 10);
}

bool Request::setMethod(std::string &method) {
  if (!Validator::validate("method", method)) {
    return false;
  }

  this->_method = method;
  return true;
}

bool Request::setPath(std::string &path) {
  if (!Validator::validate("path", path)) {
    return false;
  }

  this->_path = path;
  return true;
}

bool Request::setProtocol(std::string &protocol) {
  if (!Validator::validate("protocol", protocol)) {
    return false;
  }

  this->_protocol = protocol;
  return true;
}

void Request::parseHeader() {
  size_t begin = 0;

  std::vector<std::string> requestLine =
      Parser::getRequestLine(this->_header, begin);
  if (requestLine.size() < 3) {
    this->_status = E_REQUEST_BAD;
    this->_responseCode = BAD_REQUEST;
    return;
  }

  if (!this->setMethod(requestLine[0]) || !this->setPath(requestLine[1]) ||
      !this->setProtocol(requestLine[2])) {
    this->_status = E_REQUEST_BAD;
    this->_responseCode = BAD_REQUEST;
    return;
  }

  while (begin < this->_header.size()) {
    std::vector<std::string> attribute =
        Parser::getHeaderAttr(this->_header, begin);
    if (attribute.size() < 2) {
      this->_status = E_REQUEST_BAD;
      this->_responseCode = BAD_REQUEST;
      return;
    }

    this->set(attribute[0], attribute[1]);
  }

  if (this->_contentLength < 1 && !this->_isChucked) {
    this->_status = E_REQUEST_BAD;
    this->_responseCode = LENGTH_REQUIRED;
  }

  if (!this->_method.compare("POST"))
    this->_status = E_REQUEST_INCOMPLETE;
  else
    this->_status = E_REQUEST_COMPLETE;
}

void Request::appendRawHeader(std::string &fragment) {
  size_t pos = fragment.find("\r\n\r\n");

  if (pos == std::string::npos) {
    this->_header.append(fragment);

    return;
  }

  std::string str = fragment.substr(0, pos + 2);
  if (this->_header.size() + str.size() > 8000) {
    this->_status = E_REQUEST_BAD;
    this->_responseCode = HEADER_TOO_LARGE;
    return;
  }

  fragment.erase(0, pos + 4);

  this->_header.append(str);
  this->parseHeader();
}

void Request::handleChunkedBody(std::string &fragment) {
  std::string str;
  size_t begin = 0;
  size_t end = 0;
  static bool isContent;
  static long long chunkSize;
  long long maxSize = fragment.size();

  if (!chunkSize) {
    isContent = false;
  }

  while (begin < fragment.size()) {
    if (!isContent) {
      end = fragment.find("\r\n", begin);
      if (end == std::string::npos) {
        this->_status = E_REQUEST_BAD;
        this->_responseCode = BAD_REQUEST;
        return;
      }

      chunkSize = Parser::strtoll(fragment.substr(begin, end - begin), 16);

      if (chunkSize < 1) {
        this->_status = E_REQUEST_COMPLETE;
        std::cout << "ALALALLAL Mazel Tov 1 !!" << std::endl;
        break;
      }

      fragment.erase(begin, end + 2);
      maxSize -= (end - begin) + 2;
      begin = end + 2;
      isContent = true;
    }

    if (chunkSize < maxSize) {
      maxSize = chunkSize;
    }

    str = fragment.substr(begin, maxSize);
    fragment.erase(begin, maxSize);
    this->_body.append(str);

    chunkSize -= maxSize;
  }
}

void Request::appendRawBody(std::string &fragment) {
  if (this->_isChucked) {
    return handleChunkedBody(fragment);
  }

  size_t remainingLength = this->_contentLength - this->_body.size();

  if (remainingLength <= fragment.size()) {
    this->_status = E_REQUEST_COMPLETE;
  }

  /*if (remainingLength > fragment.length())*/
  /*  remainingLength = fragment.length();*/
  std::string str = fragment.substr(0, remainingLength);
  fragment.erase(0, remainingLength);

  this->_body.append(str);
}

void Request::appendRawData(std::string &fragment) {
  if (this->_status == E_REQUEST_HEADER_INCOMPLETE) {
    this->appendRawHeader(fragment);
  }

  if (this->_status == E_REQUEST_INCOMPLETE) {
    this->appendRawBody(fragment);
  }

  if (this->_status == E_REQUEST_COMPLETE && fragment.size() > 0) {
    this->_status = E_REQUEST_BAD;
    this->_responseCode = BAD_REQUEST;
  }
}

void Request::clean() {
  this->_path.clear();
  this->_method.clear();
  this->_protocol.clear();
  this->_host.clear();
  this->_header.clear();
  this->_body.clear();
  this->_responseCode.clear();
  this->_contentLength = 0;
  this->_isChucked = false;
  this->_status = E_REQUEST_HEADER_INCOMPLETE;
}
