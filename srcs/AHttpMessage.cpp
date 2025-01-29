#include "AHttpMessage.hpp"

AHttpMessage::AHttpMessage() : _rawData("") {}

AHttpMessage::AHttpMessage(const AHttpMessage &src) { *this = src; }

AHttpMessage &AHttpMessage::operator=(const AHttpMessage &src) {
  if (this != &src) {
    this->_rawData = src._rawData;
  }

  return *this;
}

void AHttpMessage::appendRawData(const std::string &fragment) {
  this->_rawData.append(fragment);
}
