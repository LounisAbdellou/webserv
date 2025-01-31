#ifndef __VALIDATOR__
#define __VALIDATOR__

#include <iostream>
#include <fstream> 
#include <string>
#include <map>
#include <algorithm>
#include <locale>
#include <cstdlib>
#include <stdexcept>
#include <stdlib.h>       // exit()

class Validator {
  public:
    
    static void throwError(std::string message);
    static void init();
    static bool validate(std::string key, std::string& value);
    
    class		WebservValidationException : public std::invalid_argument
    {
      public:
        WebservValidationException(std::string what) : std::invalid_argument(what) {}
    };

  private:
    Validator();
    Validator(const Validator& cpy);
    const Validator& operator=(const Validator& cpy);
    ~Validator();
    
    static std::map<std::string, bool(*)(std::string&)>    _validators;
    static bool                                           validateListen(std::string& value);
    static bool                                           validateIp(std::string& value);
    static bool                                           validatePort(std::string& value);
    static bool                                           validatePath(std::string& value);
    static bool                                           validateDirectory(std::string& value);
    static bool                                           validateFile(std::string& value);
    static bool                                           validateMethod(std::string& value);
};

#endif
