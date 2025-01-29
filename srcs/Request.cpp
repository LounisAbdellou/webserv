#include "Request.hpp"
#include "AHttpMessage.hpp"
#include <cctype>
#include <iostream>
#include <sstream>

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
  this->_path = "";
  this->_method = "";
  this->_status = REQUEST_HEADER_INCOMPLETE;
  // this->_setters["Content-Length"] = &Request::setContentLength;
  // this->_headerAttributes["Content-Length"] = "";
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

RequestStatus Request::getStatus() const { return this->_status; }

void Request::set(const std::string &key, const std::string &value) {
  if (this->_setters.find(key) == this->_setters.end()) {
    return;
  }

  // this->_setters[key] = value;
}

void Request::parseHeader() {
  size_t i = 0;
  size_t j = 0;
  std::string key;
  std::string value;
  int spaceCount = 0;
  bool isFirstLine = true;

  while (i < this->_rawHeader.size()) {
    if (!isFirstLine) {
      j = this->_rawHeader.find(": ", i);
      if (j == std::string::npos) {
        break;
      }

      key = this->_rawHeader.substr(i, j - i);
      i = j + 2;

      j = this->_rawHeader.find("\r\n", i);
      if (j == std::string::npos) {
        break;
      }

      value = this->_rawHeader.substr(i, j - i);
      i = j + 2;

      std::cout << key << " -> " << value << std::endl;

      continue;
    }

    if (spaceCount < 2) {
      j = this->_rawHeader.find(" ", i);
      if (j == std::string::npos) {
        break;
      }

      value = this->_rawHeader.substr(i, j - i);
      i = j + 1;

      if (spaceCount == 0) {
        this->_method = value;
      } else if (spaceCount == 1) {
        this->_path = value;
      }

      spaceCount++;
    } else {
      isFirstLine = false;
      j = this->_rawHeader.find("\r\n", i);
      if (j == std::string::npos) {
        break;
      }

      this->_protocol = this->_rawHeader.substr(i, j - i);
      i = j + 2;
    }
  }

  // if (this->_method != "GET" && this->_method != "POST" &&
  //     this->_method != "DELETE") {
  //   this->_status = REQUEST_BAD;
  // } else if (this->_method == "POST") {
  //   std::string encoding = this->findHeaderAttribute("Transfer-Encoding");
  //
  //   if (encoding == "chunked") {
  //     this->_isChunked = true;
  //   }
  //
  //   long long contentLength =
  //       this->_strtoll(this->findHeaderAttribute("Content-Length"), 10);
  //   if (contentLength <= 0 && !this->_isChunked) {
  //     this->_status = REQUEST_BAD;
  //     return;
  //   }
  //
  //   this->_contentLength = contentLength;
}

// this->_status = REQUEST_INCOMPLETE;
// }

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
