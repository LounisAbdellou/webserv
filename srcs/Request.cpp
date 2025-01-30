#include "Request.hpp"
#include "AHttpMessage.hpp"

Request::Request() : AHttpMessage() {
  this->path = "";
  this->method = "";
}

Request::Request(const Request &src) : AHttpMessage(src) { *this = src; }

Request &Request::operator=(const Request &src) {
  if (this != &src) {
    this->path = src.path;
    this->method = src.method;
  }

  return *this;
}

void Request::parseData() {
  bool isPath = false;

  for (size_t i = 0; i < this->_rawData.size(); i++) {
    if (this->_rawData[i] == ' ' && !isPath) {
      isPath = true;
      continue;
    } else if (this->_rawData[i] == ' ' && isPath) {
      break;
    }

    if (!isPath) {
      this->method.push_back(this->_rawData[i]);
    } else {
      this->path.push_back(this->_rawData[i]);
    }
  }
}
