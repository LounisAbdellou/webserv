#include "Request.hpp"
#include "AHttpMessage.hpp"
#include "Parser.hpp"
#include <vector>

Request::Request() : AHttpMessage() {
  this->_isBinary = true;
  this->_isChucked = false;
  this->_status = E_REQUEST_HEADER_INCOMPLETE;
  this->_contentLength = 0;
  this->_setters["Host"] = &Request::setHost;
  this->_setters["Content-Type"] = &Request::setIsChucked;
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

bool Request::getIsBinary() const { return this->_isBinary; }

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

void Request::setStatus(Request::Status status) { this->_status = status; }

void Request::setHost(const std::string &host) { this->_host = host; }

void Request::setIsChucked(const std::string &transferEncoding) {
  if (transferEncoding == "chunked") {
    this->_isChucked = true;
  }
}

void Request::setIsBinary(const std::string &contentType) {
  if (contentType == "application/json" || contentType == "text/html" ||
      contentType == "text/plain") {
    this->_isBinary = false;
    return;
  }

  this->_isBinary = true;
}

void Request::setContentLength(const std::string &contentLength) {
  this->_contentLength = Parser::strtoll(contentLength, 10);
}

bool Request::setMethod(std::string &method) {
  if (!Validator::validate("method", method)) {
    this->_responseCode = BAD_REQUEST;
    return false;
  }

  this->_method = method;
  return true;
}

bool Request::setPath(std::string &path) {
  if (!Validator::validate("path", path)) {
    this->_responseCode = BAD_REQUEST;
    return false;
  }

  this->_path = path;
  return true;
}

bool Request::setProtocol(std::string &protocol) {
  if (!Validator::validate("protocol", protocol)) {
    this->_responseCode = HTTP_VERSION;
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

  if (!this->_contentLength && !this->_isChucked) {
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

void printTest(const std::string &fragment, std::string type) {
  std::cout << "///////////" << type << "/////////////" << std::endl;
  std::string::const_iterator it;
  for (it = fragment.begin(); it != fragment.end(); it++) {
    // if (isprint(*it)) {
    std::cout << *it;
    // }
  }
  std::cout << std::endl;
  std::cout << "/////////////" << type << "END////////////////" << std::endl;
}

void Request::handleChunkedBody(std::string fragment) {
  this->_chunk.append(fragment);

  while (true) {
    if (!_chunkSize) {
      size_t pos = this->_chunk.find("\r\n");
      if (pos == std::string::npos) {
        return;
      }

      this->_chunk.erase(pos, 2);
      this->_chunkContent.append(
          this->_chunk.substr(pos, this->_chunk.size() - pos));
      this->_chunk.erase(pos, this->_chunk.size() - pos);

      this->_chunkSize = Parser::strtoll(this->_chunk, 16);
      if (!this->_chunk.compare("0\r\n\r\n")) {
        std::cout << "OIJIODJSJI" << std::endl;
        this->_status = Request::E_REQUEST_COMPLETE;
        return;
      }
      this->_chunk.clear();
    }
    // 8b
    // c
    // 9b
    // c
    // 10b
    // c
    //
    // c2
    // 7b
    // c
    // 0b
    // EOT

    printTest(this->_chunk, "CHUNK");
    printTest(this->_chunkContent, "CONTENT");
    std::cout << "SIZE: " << this->_chunkSize << std::endl;

    if (this->_chunkContent.empty()) {
    }

    if (this->_chunkSize > this->_chunkContent.size() - 2) {
      std::cout << "PROC 1" << std::endl;
      this->_chunkSize -= this->_chunkContent.size();
      this->_body.append(
          this->_chunkContent.substr(0, this->_chunkContent.size()));
      this->_chunkContent.erase(0, this->_chunkContent.size());
      return;
    }

    std::cout << "PROC 2" << std::endl;
    this->_body.append(this->_chunkContent.substr(0, this->_chunkSize));
    this->_chunkContent.erase(0, this->_chunkSize);
    this->_chunkSize = 0;

    this->_chunk.append(
        this->_chunkContent.substr(2, this->_chunkContent.size() - 2));
    this->_chunkContent.erase(2, this->_chunkContent.size() - 2);
    this->_chunkContent.clear();

    // printTest(this->_chunk, "CHUNK");
    // printTest(this->_chunkContent, "CONTENT");
    // std::cout << "SIZE: " << this->_chunkSize << std::endl;
  }
}

void Request::appendRawBody(std::string &fragment) {
  if (this->_isChucked) {
    handleChunkedBody("12\r\nJust a simple ");
    handleChunkedBody("test\r\n12");
    handleChunkedBody("\r\nJust a simple test\r\n");
    handleChunkedBody("12\r\nJust a simple test\r\n0\r\n\r\n");
    return;
  }

  size_t remainingLength = this->_contentLength - this->_body.size();

  if (remainingLength <= fragment.size()) {
    this->_status = E_REQUEST_COMPLETE;
  }

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

  if (!this->_isChucked && this->_status == E_REQUEST_COMPLETE &&
      fragment.size() > 0) {
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
  this->_isBinary = true;
  this->_status = E_REQUEST_HEADER_INCOMPLETE;
}
