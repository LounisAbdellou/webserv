#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "AHttpMessage.hpp"
#include <map>

class Request : public AHttpMessage {
public:
  enum Status {
    E_REQUEST_BAD = 1,
    E_REQUEST_COMPLETE = 2,
    E_REQUEST_INCOMPLETE = 3,
    E_REQUEST_HEADER_INCOMPLETE = 4,
  };

  Request();
  ~Request() {};

  int getContentLength() const;
  std::string getPath() const;
  std::string getHost() const;
  std::string getMethod() const;
  Status getStatus() const;

  void set(const std::string &key, const std::string &value);

  void appendRawData(std::string &fragment);
  void clean();

private:
  std::string _path;
  std::string _host;
  std::string _method;
  std::string _protocol;

  bool _isChucked;
  int _contentLength;
  Status _status;

  std::map<std::string, void (Request::*)(const std::string &)> _setters;

  Request(const Request &src);
  Request &operator=(const Request &src);

  void setHost(const std::string &host);
  void setContentLength(const std::string &contentLength);
  void setIsChucked(const std::string &transferEncoding);
  bool setMethod(std::string &method);
  bool setPath(std::string &path);
  bool setProtocol(std::string &protocol);

  void parseHeader();
  size_t parseRequestLine(const std::string &header);
  void handleChunkedBody(std::string &fragment);
  void appendRawHeader(std::string &fragment);
  void appendRawBody(std::string &fragment);
};

#endif // !REQUEST_HPP
