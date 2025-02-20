#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "AHttpMessage.hpp"
#include "Request.hpp"
#include <map>

class Response : public AHttpMessage {
private:
  std::map<std::string, std::string> _attributes;

public:
  Response();
  Response(const Response &src);
  ~Response() {};

  Response &operator=(const Response &src);

  void setAttribute(const std::string &key, const std::string &value);

  std::string get() const;
  void generate(std::string &fragment, Request &request);
  void clean();
};

#endif // !RESPONSE_HPP
