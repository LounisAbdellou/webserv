#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Header.hpp"

class Response {
  public:
    Response();
    ~Response();
    
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

    enum Status 
    {
      E_RESPONSE_CREATED = 0,
      E_RESPONSE_CGI = 1,
      E_RESPONSE_HEADER = 2,
      E_RESPONSE_COMPLETE = 3
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
    void                                set(int bsend);
    void                                add(const std::string key, const std::string value);
    bool                                has(const std::string key) const;
    bool                                isset(const std::string key) const;
    std::string                         get(const std::string key) const;
    int*                                pipe(const std::string value);
    int                                 bsend(int bwrite = 0);

    void                                clean();
    
  
  private:
    Response(const Response &src);
    Response &operator=(Response &src);
    
    std::map<std::string, void (Response::*)(std::string &)>    _setters;
    std::map<std::string, std::string(Response::*)() const >    _getters;
    
    Status                              _status;
    Type                                _type;
    int                                 _pipe[2];
    std::map<std::string, std::string>  _codes;
    int                                 _bsend;

    std::map<std::string, std::string>  _args;
    std::string                         _header;

    void                                setHeader(std::string& value);
    void                                setPipe(std::string& value);

    std::string                         getHeader() const;
    std::string                         getType() const;
    std::string                         getStatus() const;
    std::string                         getDone() const;
    std::string                         getCgi() const;
};

#endif // !RESPONSE_HPP
