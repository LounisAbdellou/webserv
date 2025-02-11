#ifndef __REQUEST__
#define __REQUEST__

#include <string>

class Request {
  public:
    Request();
    ~Request();

    void  clean();
    
  private:
    Request(const Request& cpy);
    Request& operator=(Request& cpy);
};

#endif
