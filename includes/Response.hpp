#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "ABasicClass.hpp"
#include <map>

class Response {
  public:
    Response();
    ~Response() {};
    
    enum Status 
    {
      E_RESPONSE_CREATED = 0,
      E_RESPONSE_HEADER = 1,
      E_RESPONSE_COMPLETE = 2
    };
    
    enum Type 
    {
      E_RESPONSE_OTHER = 0,
      E_RESPONSE_DOC = 1,
      E_RESPONSE_PIPE = 2,
    };

    void                                init();
    void                                set(const std::string key, const std::string value);
    void                                set(Response::Status status);
    void                                set(Response::Type type);
    bool                                has(const std::string key) const;
    bool                                isset(const std::string key) const;
    std::string                         get(const std::string key) const;
    int*                                pipe(const std::string value);

    void                                clean();
    
  
  private:
    Response(const Response &src);
    Response &operator=(const Response &src);
    
    std::map<std::string, void (Response::*)(const std::string &)>   _setters;
    std::map<std::string, std::string(Response::*)() const >         _getters;
    
    Status                              _status;
    Type                                _type;
    int                                 _pipe[2];
    std::map<std::string, std::string>  _codes;

    std::map<std::string, std::string>  _args;
    std::string                         _header;

    void                                setHeader(std::string& value);
    void                                setPipe(std::string& value);

    std::string                         getHeader() const;
    std::string                         getType() const;
    std::string                         getStatus() const;
    std::string                         getDone() const;
};

#endif // !RESPONSE_HPP
