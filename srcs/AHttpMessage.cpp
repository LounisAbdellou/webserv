#include "AHttpMessage.hpp"

AHttpMessage::AHttpMessage() : _rawData("") {}

AHttpMessage::AHttpMessage(const AHttpMessage &src) { *this = src; }

AHttpMessage &AHttpMessage::operator=(const AHttpMessage &src) {
  if (this != &src) {
    this->_rawData = src._rawData;
  }

  return *this;
}

std::string &AHttpMessage::getRawData() { return this->_rawData; }

// #include <iostream>
void AHttpMessage::appendRawData(const std::string &fragment) {
  // std::string test = this->_requestQueue.back()->getRawData();

  // for (size_t i = 0; i < fragment.size(); i++) {
  //   int test2 = fragment[i];
  //   std::cout << fragment[i] << " => " << test2 << std::endl;
  // }

  this->_rawData.append(fragment);
}
