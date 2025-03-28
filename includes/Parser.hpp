#ifndef __PARSER__
#define __PARSER__

#include <iostream>
#include <fstream> 
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>       // exit()
#include "Server.hpp"
#include "Location.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>

class Parser {
  public:
    static void											throwError(std::string message, Location* location = NULL);
    static void											handleFile(std::ifstream& file);
    static bool											handleClosure(const std::string& line);
    static bool											handleBlock(const std::string& line, const std::string type);
    static std::string							getKey(const std::string& line);
    static std::string							getValue(const std::string& key, const std::string& line);
    static std::string							getLocation(const std::string& line);
    static std::string							getExtension(const std::string& path);
    static std::string							getFolder(const std::string& path);
    static std::string							getEntryHtmlTag(struct Entry entry, std::string &path);
    static std::string							getListingHtml(const std::vector<struct Entry> &dirEntries, const std::string &path);
    static std::vector<std::string>	getRequestLine(const std::string &header, size_t &pos);
    static std::vector<std::string>	getHeaderAttr(const std::string &header, size_t &pos);
    static long long								strtoll(const std::string &str, int base);
    static std::string							to_string(long long nbr);
    static std::vector<std::string> getSocketInfo(int socket);

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
