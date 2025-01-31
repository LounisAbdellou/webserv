#ifndef __LOCATION__
#define __LOCATION__

#include <iostream>
#include <string>
#include <vector>
#include <map>
  

class Location {
  public:
    Location();
    ~Location();
    
    bool      has(const std::string key) const;
    void      set(const std::string key, const std::string value);
  
  private:
    Location(const Location& cpy);
    Location& operator=(Location& cpy);

    std::map<std::string, void(Location::*)(std::string)> _setters;
    std::string                                           _root;
    std::string                                           _index;
    std::string                                           _error_page;
    std::string                                           _allowed_method;
    std::string                                           _allow_listing;
    std::string                                           _redirect;

    void  setRoot(const std::string value);
    void  setIndex(const std::string value);
    void  setErrorPage(const std::string value);
    void  setAllowedMethod(const std::string value);
    void  setAllowListing(const std::string value);
    void  setRedirect(const std::string value);
};

#endif
