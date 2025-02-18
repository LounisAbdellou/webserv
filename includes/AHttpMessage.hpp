#ifndef A_HTTP_MESSAGE_HPP
#define A_HTTP_MESSAGE_HPP

#include <string>

class AHttpMessage {
protected:
  std::string _rawData;

public:
  AHttpMessage();
  AHttpMessage(const AHttpMessage &src);
  virtual ~AHttpMessage() {};

  AHttpMessage &operator=(const AHttpMessage &src);

  std::string getRawData() const;

  virtual void appendRawData(std::string &fragment) = 0;
};

#endif // !A_HTTP_MESSAGE_HPP
