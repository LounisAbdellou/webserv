#include "Validator.hpp"

Validator::Validator()
{
}

Validator::Validator(const Validator& cpy)
{
  (void)cpy;
}

const Validator& Validator::operator=(const Validator& cpy)
{
  return cpy;
}

Validator::~Validator()
{
}

std::map<std::string, bool(*)(std::string&)>   Validator::_validators;

void  Validator::init()
{
  Validator::_validators["listen"] = &Validator::validateListen;
  Validator::_validators["path"] = &Validator::validatePath;
  Validator::_validators["directory"] = &Validator::validateDirectory;
  Validator::_validators["file"] = &Validator::validateFile;
  Validator::_validators["method"] = &Validator::validateMethod;
  Validator::_validators["ip"] = &Validator::validateIp;
  Validator::_validators["port"] = &Validator::validatePort;
  Validator::_validators["protocol"] = &Validator::validateProtocol;
}

bool  Validator::validate(std::string key, std::string& value)
{
  if (Validator::_validators.find(key) == Validator::_validators.end())
    return false;
  return (Validator::_validators[key])(value);
}

bool  Validator::validateListen(std::string& value)
{
  if (value.find_first_not_of("0123456789.:") < value.length())
    return false;

  std::size_t del = value.find(":");
  std::size_t add = 1;
  if (del > value.length())
  {
    del = 0;
    add = 0;
  }
  std::string ip = value.substr(0, del);
  std::string port = value.substr(del + add, value.length());

  if (!ip.empty() && !Validator::validate("ip", ip))
    return false;

  if (!port.empty() && !Validator::validate("port", port))
    return false;

  if (ip.empty())
    value = port.insert(0, "0.0.0.0:");
  
  return true;
}

bool  Validator::validateIp(std::string& value)
{
  std::size_t begin = 0;
  std::size_t end = 0;
  for (std::size_t i = 0; i < 3; i++)
  {
    end = value.find_first_of(".", begin);
    if (end - begin > 3)
      return false;
    begin = end + 1;
  }
  if (value.length() - begin > 3)
    return false;
  return true;
}

bool  Validator::validatePort(std::string& value)
{
  if (value.find_first_not_of("0123456789") < value.length() || value.length() > 5)
    return false;
  
  int p_value = std::atoi(value.c_str());
  
  if (p_value > 49151 || p_value < 1)
    return false;
  return true;
}

bool  Validator::validatePath(std::string& value)
{
  (void)value;
  return true;
}

bool  Validator::validateDirectory(std::string& value)
{
  if (!Validator::validate("path", value))
    return false;
  return true;
}

bool  Validator::validateFile(std::string& value)
{
  if (!Validator::validate("path", value))
    return false;
  return true;
}

bool  Validator::validateMethod(std::string& value)
{
	if (value != "GET" || value != "POST" || value != "DELETE")
		return false;
  return true;
}

bool  Validator::validateProtocol(std::string& value)
{
	if (value != "HTTP/1.1")
		return false;
  return true;
}

void  Validator::throwError(std::string message)
{
  throw Validator::WebservValidationException(message.insert(0, "Validator Error: "));
}
