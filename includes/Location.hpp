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
    bool      isset(const std::string key) const;
    std::string                               get(const std::string key) const;
    bool                                                    isListingAllowed() const;
  
  private:
    Location(const Location& cpy);
    Location& operator=(Location& cpy);

    std::map<std::string, void(Location::*)(std::string)> _setters;
    std::map<std::string, std::string(Location::*)() const >  _getters;
    std::string                                           _key;
    std::string                                           _root;
    std::string                                           _index;
    std::string                                           _error_page;
    std::string                                           _allowed_method;
    std::string                                           _allow_listing;
    std::string                                           _redirect;

    void  setRoot(const std::string value);
    void  setKey(const std::string value);
    void  setIndex(const std::string value);
    void  setErrorPage(const std::string value);
    void  setAllowedMethod(const std::string value);
    void  setAllowListing(const std::string value);
    void  setRedirect(const std::string value);
    
    std::string                                             getIndex() const;
    std::string                                             getRoot() const;
    std::string                                             getKey() const;
    std::string                                             getAllowListing() const;
};

#endif
