#include "Request.hpp"
#include "AHttpMessage.hpp"

Request::Request() : AHttpMessage() {
  this->_path = "";
  this->_method = "";
  this->_isComplete = false;
}

Request::Request(const Request &src) : AHttpMessage(src) { *this = src; }

Request &Request::operator=(const Request &src) {
  if (this != &src) {
    this->_path = src._path;
    this->_method = src._method;
    this->_isComplete = src._isComplete;
  }

  return *this;
}

void Request::setIsComplete(bool isComplete) { this->_isComplete = isComplete; }

void Request::parseData() {
  bool isPath = false;

  for (size_t i = 0; i < this->_rawData.size(); i++) {
    if (this->_rawData[i] == ' ' && !isPath) {
      isPath = true;
      continue;
    } else if (this->_rawData[i] == ' ' && isPath) {
      break;
    }

    if (isPath) {
      this->_path.push_back(this->_rawData[i]);
    } else {
      this->_method.push_back(this->_rawData[i]);
    }
  }
}
