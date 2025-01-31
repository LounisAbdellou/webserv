#ifndef __RESPONSE__
#define __RESPONSE__

#include <string>

class Response {
  public:
    Response();
    ~Response();

    std::string get() const;
    
  private:
    Response(const Response& cpy);
    Response& operator=(Response& cpy);
};

#endif
