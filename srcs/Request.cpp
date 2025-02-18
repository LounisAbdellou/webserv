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
  this->_isChucked = false;
  this->_status = REQUEST_HEADER_INCOMPLETE;
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

RequestStatus Request::getStatus() const { return this->_status; }

void Request::set(const std::string &key, const std::string &value) {
  if (this->_setters.find(key) == this->_setters.end()) {
    return;
  }

  (this->*_setters[key])(value);
}

void Request::setIsChucked(const std::string &transferEncoding) {
  if (transferEncoding == "chunked") {
    this->_isChucked = true;
  }
}

void Request::setContentLength(const std::string &contentLength) {
  this->_contentLength = this->_strtoll(contentLength, 10);
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
      Parser::getRequestLine(this->_rawHeader, begin);
  if (requestLine.size() < 3) {
    this->_status = REQUEST_BAD;
    return;
  }

  if (!this->setMethod(requestLine[0]) || !this->setPath(requestLine[1]) ||
      !this->setProtocol(requestLine[2])) {
    this->_status = REQUEST_BAD;
    return;
  }

  while (begin < this->_rawHeader.size()) {
    std::vector<std::string> attribute =
        Parser::getHeaderAttr(this->_rawHeader, begin);
    if (attribute.size() < 2) {
      this->_status = REQUEST_BAD;
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
        this->_status = REQUEST_BAD;
        return;
      }

      chunkSize = this->_strtoll(fragment.substr(begin, end - begin), 16);

      if (chunkSize < 1) {
        this->_status = REQUEST_COMPLETE;
        std::cout << "ALALALLAL Mazel Tov 1 !!" << std::endl;
        break;
      }

      maxSize -= (end - begin) + 2;
      begin = end + 2;
      isContent = true;
    }

    if (chunkSize < maxSize) {
      maxSize = chunkSize;
    }

    str = fragment.substr(begin, maxSize);
    this->_rawBody.append(str);
    this->_rawData.append(str);

    chunkSize -= maxSize;
    if (chunkSize < 1) {
      this->_status = REQUEST_COMPLETE;
      std::cout << "ALALALLAL Mazel Tov 2 !!" << std::endl;
      break;
    }
  }
  fragment.erase(0, fragment.size());
}

void Request::appendRawBody(std::string &fragment) {
  if (this->_isChucked) {
    return handleChunkedBody(fragment);
  }

  size_t remainingLength = this->_contentLength - this->_rawBody.size();

  if (remainingLength <= fragment.size()) {
    this->_status = REQUEST_COMPLETE;
  }

  std::string str = fragment.substr(0, remainingLength);
  fragment.erase(0, remainingLength);

  this->_rawBody.append(str);
  this->_rawData.append(str);
}

void Request::appendRawData(std::string &fragment) {
  if (this->_status == REQUEST_HEADER_INCOMPLETE) {
    this->appendRawHeader(fragment);
  }

  if (this->_status == REQUEST_INCOMPLETE) {
    this->appendRawBody(fragment);
  }

  if (this->_status == REQUEST_COMPLETE && fragment.size() > 0) {
    this->_status = REQUEST_BAD;
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
  this->_isChucked = false;
  this->_status = REQUEST_HEADER_INCOMPLETE;
}
