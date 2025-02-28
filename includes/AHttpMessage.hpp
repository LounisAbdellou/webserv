#ifndef A_HTTP_MESSAGE_HPP
#define A_HTTP_MESSAGE_HPP

#include <string>

class AHttpMessage {
protected:
  std::string _responseCode;
  std::string _header;
  std::string _body;

public:
  static const std::string OK;
  static const std::string CREATED;
  static const std::string MOVED_PERM;
  static const std::string FOUND;
  static const std::string TEMP_REDIRECT;
  static const std::string PERM_REDIRECT;
  static const std::string BAD_REQUEST;
  static const std::string NOT_FOUND;
  static const std::string FORBIDDEN;
  static const std::string NOT_ALLOWED;
  static const std::string LENGTH_REQUIRED;
  static const std::string HEADER_TOO_LARGE;
  static const std::string INTERNAL_SERVER_ERROR;
  static const std::string HTTP_VERSION;

  AHttpMessage();
  AHttpMessage(const AHttpMessage &src);
  virtual ~AHttpMessage() {};

  AHttpMessage &operator=(const AHttpMessage &src);

  std::string getBody() const;
  std::string getResponseCode() const;

  void setResponseCode(const std::string &responseCode);

  virtual void clean() = 0;
};

#endif // !A_HTTP_MESSAGE_HPP
