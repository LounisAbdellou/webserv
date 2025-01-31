#include "Location.hpp"

Location::Location()
{
  _setters["root"] = &Location::setRoot;
  _setters["index"] = &Location::setIndex;
  _setters["error_page"] = &Location::setErrorPage;
  _setters["allowed_method"] = &Location::setAllowedMethod;
  _setters["allow_listing"] = &Location::setAllowListing; 
  _setters["redirect"] = &Location::setRedirect;
}

Location::Location(const Location& cpy)
{
  (void)cpy;
}

Location& Location::operator=(Location& cpy)
{
  return cpy;
}

Location::~Location()
{
}

bool  Location::has(const std::string key) const
{
  return !(this->_setters.find(key) == this->_setters.end());
}

void  Location::set(const std::string key, const std::string value)
{
  (this->*_setters[key])(value);
}

void  Location::setRoot(const std::string value)
{
  this->_root = value;
}

void  Location::setIndex(const std::string value)
{
  this->_index = value;
}

void  Location::setErrorPage(const std::string value)
{
  this->_error_page = value;
}

void  Location::setAllowedMethod(const std::string value)
{
  this->_allowed_method = value;
}

void  Location::setAllowListing(const std::string value)
{
  this->_allow_listing = value;
}

void  Location::setRedirect(const std::string value)
{
  this->_redirect = value;
}

