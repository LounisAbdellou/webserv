#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "AHttpMessage.hpp"
#include "Request.hpp"
#include <map>

class Response : public AHttpMessage {
private:
  std::map<std::string, std::string> _attributes;
  std::map<std::string, std::string> _status_codes;
  std::string _message;
  bool _isListing;
  bool _isRedirect;

public:
  Response();
  Response(const Response &src);
  ~Response() {};

  Response &operator=(const Response &src);

  void setAttribute(const std::string &key, const std::string &value);

  std::string get() const;
  std::string get(const std::string &status) const;

  void erase(size_t length);
  void generate(const std::string &fragment, Request &request);
  void generate(const std::string &fragment);
  void clean();

  std::string getMessage() const;

  bool getIsListing() const;
  void setIsListing(bool value);

  bool getIsRedirect() const;
  void setIsRedirect(bool value);
};

#endif // !RESPONSE_HPP
