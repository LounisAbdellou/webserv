#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "AHttpMessage.hpp"

class Request : public AHttpMessage {
private:
  bool _isComplete;
  std::string _path;
  std::string _method;

public:
  Request();
  Request(const Request &src);
  virtual ~Request() {};

  Request &operator=(const Request &src);

  void setIsComplete(bool isComplete);

  void parseData();
};

#endif // !REQUEST_HPP
