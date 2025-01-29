#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "AHttpMessage.hpp"
#include <map>

enum RequestStatus {
  REQUEST_BAD = 1,
  REQUEST_COMPLETE = 2,
  REQUEST_INCOMPLETE = 3,
  REQUEST_HEADER_INCOMPLETE = 4,
};

class Request : public AHttpMessage {
private:
  std::string _path;
  std::string _method;
  std::string _protocol;
  std::string _rawBody;
  std::string _rawHeader;

  bool _isChucked;
  int _contentLength;
  RequestStatus _status;

  std::map<std::string, void (Request::*)(const std::string &)> _setters;

  long long _strtoll(const std::string &str, int base) const;

public:
  Request();
  Request(const Request &src);
  ~Request() {};

  Request &operator=(const Request &src);

  int getContentLength() const;
  RequestStatus getStatus() const;
  std::string &getPath() const;
  std::string &getMethod() const;

  void set(const std::string &key, const std::string &value);
  void setContentLength(const std::string &contentLength);
  void setIsChucked(const std::string &transferEncoding);
  bool setMethod(std::string &method);
  bool setPath(std::string &path);
  bool setProtocol(std::string &protocol);

  void parseHeader();
  size_t parseRequestLine(const std::string &header);

  void clean();
  void handleChunkedBody(std::string &fragment);
  void appendRawHeader(std::string &fragment);
  void appendRawBody(std::string &fragment);
  void appendRawData(std::string &fragment);
};

#endif // !REQUEST_HPP
