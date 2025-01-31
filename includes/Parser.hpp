#ifndef __PARSER__
#define __PARSER__

#include <iostream>
#include <fstream> 
#include <string>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <stdlib.h>       // exit()
#include "Server.hpp"
#include "Location.hpp"

class Parser {
  public:
    static void               throwError(std::string message, Location* location = NULL);
    static void               handleFile(std::ifstream& file);
    static bool               handleClosure(const std::string& line);
    static bool               handleBlock(const std::string& line, const std::string type);
    static std::string        getKey(const std::string& line);
    static std::string        getValue(const std::string& key, const std::string& line);
    static std::string        getLocation(const std::string& line);

    
    class		WebservParseException : public std::invalid_argument
    {
      public:
        WebservParseException(std::string what) : std::invalid_argument(what) {}
    };

  private:
    Parser();
    Parser(const Parser& cpy);
    const Parser& operator=(const Parser& cpy);
    ~Parser();
};

#endif
