#ifndef __REQUEST__
#define __REQUEST__

#include <string>

class Request {
  public:
    Request();
    ~Request();

    void  clean();
    bool  error() const;
    
  private:
    Request(const Request& cpy);
    Request& operator=(Request& cpy);
};

#endif
