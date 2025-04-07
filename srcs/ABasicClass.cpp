#include "ABasicClass.hpp"

ABasicClass::ABasicClass() {}

ABasicClass::ABasicClass(const ABasicClass& src) { (void)src; }

ABasicClass& ABasicClass::operator=(ABasicClass& src) { return src; }

void ABasicClass::set(const std::string key, std::string value) 
{
  if (this->_setters.find(key) == this->_setters.end()) return ;
  (this->*_setters[key])(value);
}

bool ABasicClass::has(const std::string key) const 
{
  return !(this->_setters.find(key) == this->_setters.end());
}

bool ABasicClass::isset(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return false;
  return (!(this->*_getters.at(key))().empty());
}

std::string ABasicClass::get(const std::string key) const 
{
  if (this->_getters.find(key) == this->_getters.end())
    return "";
  return (this->*_getters.at(key))();
}
