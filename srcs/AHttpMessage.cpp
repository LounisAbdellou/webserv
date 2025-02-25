#include "AHttpMessage.hpp"

const std::string AHttpMessage::OK = "200 OK";

const std::string AHttpMessage::CREATED = "201 Created";

const std::string AHttpMessage::MOVED_PERM = "301 Moved Permanently";

const std::string AHttpMessage::FOUND = "302 Found";

const std::string AHttpMessage::TEMP_REDIRECT = "307 Temporary Redirect";

const std::string AHttpMessage::PERM_REDIRECT = "308 Permanent Redirect";

const std::string AHttpMessage::BAD_REQUEST = "400 Bad Request";

const std::string AHttpMessage::NOT_FOUND = "404 Not Found";

const std::string AHttpMessage::FORBIDDEN = "403 Forbidden";

const std::string AHttpMessage::NOT_ALLOWED = "405 Method Not Allowed";

const std::string AHttpMessage::LENGTH_REQUIRED = "411 Length Required";

const std::string AHttpMessage::HEADER_TOO_LARGE =
    "431 Request Header Fields Too Large";

const std::string AHttpMessage::INTERNAL_SERVER_ERROR =
    "500 Internal Server Error";

const std::string AHttpMessage::HTTP_VERSION = "505 HTTP Version Not Supported";

AHttpMessage::AHttpMessage() : _responseCode(""), _header(""), _body("") {}

AHttpMessage::AHttpMessage(const AHttpMessage &src) { *this = src; }

AHttpMessage &AHttpMessage::operator=(const AHttpMessage &src) {
  if (this != &src) {
    this->_responseCode = src._responseCode;
    this->_header = src._header;
    this->_body = src._body;
  }

  return *this;
}

std::string AHttpMessage::getBody() const { return this->_body; }

std::string AHttpMessage::getResponseCode() const {
  return this->_responseCode;
}

void AHttpMessage::setResponseCode(const std::string &code) {
  this->_responseCode = code;
}
