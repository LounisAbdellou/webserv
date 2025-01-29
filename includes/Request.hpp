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

  std::map<std::string, void (Request::*)(const std::string &)> _setters;

  int _contentLength;
  RequestStatus _status;

  long long _strtoll(const std::string &str, int base) const;

public:
  Request();
  Request(const Request &src);
  ~Request() {};

  Request &operator=(const Request &src);

  void set(const std::string &key, const std::string &value);

  int &getContentLength() const;
  RequestStatus getStatus() const;

  void parseHeader();
  size_t parseRequestLine(const std::string &header);

  void appendRawHeader(std::string &fragment);
  void appendRawBody(std::string &fragment);
  void appendRawData(std::string &fragment);
};

#endif // !REQUEST_HPP
