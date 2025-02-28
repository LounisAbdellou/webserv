#include "Location.hpp"

Location::Location() : _key("/"), _root(""), _index("index.html"), _allow_listing("") 
{
  _error_page["0"] = "error.html";

  _setters["key"] = &Location::setKey;
  _setters["root"] = &Location::setRoot;
  _setters["index"] = &Location::setIndex;
  _setters["error_page"] = &Location::setErrorPage;
  _setters["allowed_method"] = &Location::setAllowedMethod;
  _setters["allow_listing"] = &Location::setAllowListing; 
  _setters["redirect"] = &Location::setRedirect;
  
  _getters["key"] = &Location::getKey;
  _getters["root"] = &Location::getRoot;
  _getters["index"] = &Location::getIndex;
  _getters["error_page"] = &Location::getErrorPage;
  _getters["allowed_method"] = &Location::getAllowedMethod;
  _getters["allow_listing"] = &Location::getAllowListing;
  _getters["redirect"] = &Location::getRedirect;

  this->set("allowed_method", "ALL");
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

bool  Location::isset(const std::string key) const
{
  if (this->_getters.find(key) == this->_getters.end())
    return false;
  return (!(this->*_getters.at(key))().empty());
}

bool  Location::isset(const std::string container, const std::string key) const
{
  if (!container.compare("allowed_method"))
    return !this->getAllowedMethod(key).empty();
  return false;
}

std::string  Location::get(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return "";
  return (this->*_getters.at(key))();
}

std::string Location::get(const std::string container, const std::string key) const
{
  if (!container.compare("error_page"))
    return this->getErrorPage(key); 
  if (!container.compare("redirect"))
    return this->getRedirect(key); 
  return "";
}

void  Location::setKey(const std::string key)
{
  this->_key = key;
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
  std::vector<std::string> lst_code;
  std::size_t begin = 0;
  std::size_t end = begin;

  while (end < value.length())
  {
    end = value.find_first_of(" ", begin);
    if (end > value.length())
      end = value.length();
    std::string elem = value.substr(begin, end - begin);
    if (Validator::validate("error_code", elem))
      lst_code.push_back(elem);
    else
    {
      if (lst_code.empty())
        this->_error_page["0"] = elem;
      for (std::vector<std::string>::iterator it = lst_code.begin(); it < lst_code.end(); ++it)
      {
        this->_error_page[*it] = elem;
      }
      lst_code.clear();
    }
    begin = end + 1;
  }
}

void  Location::setAllowedMethod(const std::string value)
{
  this->_allowed_method.clear();
  if (!value.compare("ALL"))
  {
    this->_allowed_method.push_back("GET");
    this->_allowed_method.push_back("POST");
    this->_allowed_method.push_back("DELETE");
    return;
  }
  
  std::size_t begin = 0;
  std::size_t end = begin;

  while (end < value.length())
  {
    end = value.find_first_of(" ", begin);
    if (end > value.length())
      end = value.length();
    std::string method = value.substr(begin, end - begin);
    bool allow = !method.compare("GET") || !method.compare("POST") || !method.compare("DELETE"); 
    if (allow && std::count(this->_allowed_method.begin(), this->_allowed_method.end(), method) == 0)
      this->_allowed_method.push_back(method);
    begin = end + 1;
  }
}

void  Location::setAllowListing(const std::string value)
{
  if (!value.compare("true") || !value.compare("TRUE"))
    this->_allow_listing = value;
}

void  Location::setRedirect(const std::string value)
{
  std::size_t end = value.find_first_of(" ", 0);

  if (end > value.length()) return;

  std::string code = value.substr(0, end);
  
  std::size_t begin = value.find_first_not_of(" ", end);

  if (begin > value.length()) return;

  std::string url = value.substr(begin, value.length() - begin);

  if (code.length() != 3 || (!std::isdigit(code[0]) || !std::isdigit(code[1]) || !std::isdigit(code[2])))
    return;
  
  this->_redirect[0] = code;
  this->_redirect[1] = url;
}

std::string  Location::getIndex() const
{
  return this->_index;
}

std::string  Location::getRoot() const
{
  return this->_root;
}

std::string  Location::getKey() const
{
  return this->_key;
}

std::string  Location::getAllowListing() const
{
  return this->_allow_listing;
}

std::string  Location::getAllowedMethod() const
{
  std::string allowed;
  for (std::vector<std::string>::const_iterator it = this->_allowed_method.begin(); it != this->_allowed_method.end(); ++it)
  {
    allowed.append(*it);
    allowed.append(" ");
  }
  return allowed;
}

std::string  Location::getAllowedMethod(const std::string& method) const
{
  if (std::find(this->_allowed_method.begin(), this->_allowed_method.end(), method) == this->_allowed_method.end())
    return "";
  return method;
}

std::string  Location::getRedirect() const
{
  if (this->_redirect[0].empty() || this->_redirect[1].empty())
    return "";
  std::string redirect = this->_redirect[0];
  redirect.append(" ");
  redirect.append(this->_redirect[1]);
  return redirect;
}

std::string  Location::getRedirect(const std::string& key) const
{
  if (!key.compare("code"))
    return this->_redirect[0];
  if (!key.compare("url"))
    return this->_redirect[1];
  return "";
}

std::string  Location::getErrorPage() const
{
  if (this->_error_page.find("0") == this->_error_page.end())
    return "";
  return this->_error_page.at("0");
}

std::string  Location::getErrorPage(const std::string& code) const
{
  std::string err = code.substr(0, 3);
  if (this->_error_page.find(err) == this->_error_page.end())
  {
    if (this->_error_page.find("0") == this->_error_page.end())
      return "";
    return this->_error_page.at("0");
  }
  return this->_error_page.at(err);
}
