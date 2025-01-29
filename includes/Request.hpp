#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "AHttpMessage.hpp"

class Request : public AHttpMessage {
private:
  std::string path;
  std::string method;
  std::string contentType;

public:
  Request();
  Request(const Request &src);
  virtual ~Request() {};

  Request &operator=(const Request &src);

  void parseData();
};

#endif // !REQUEST_HPP
