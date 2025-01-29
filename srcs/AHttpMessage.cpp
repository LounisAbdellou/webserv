#include "AHttpMessage.hpp"

AHttpMessage::AHttpMessage() : _rawData("") {}

AHttpMessage::AHttpMessage(const AHttpMessage &src) { *this = src; }

AHttpMessage &AHttpMessage::operator=(const AHttpMessage &src) {
  if (this != &src) {
    this->_rawData = src._rawData;
  }

  return *this;
}

std::string AHttpMessage::getRawData() const { return this->_rawData; }
