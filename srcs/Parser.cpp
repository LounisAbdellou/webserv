#include "Parser.hpp"

Parser::Parser()
{
}

Parser::Parser(const Parser& cpy)
{
  (void)cpy;
}

const Parser& Parser::operator=(const Parser& cpy)
{
  return cpy;
}

Parser::~Parser()
{
}

void  Parser::handleFile(std::ifstream& file)
{
  std::string line;

  while (std::getline(file, line))
  {
    if (std::cin.eof())
    {
      Parser::throwError("Parse Error: Problem occurs while reading file");
    }
    if (line.find_first_not_of(" \n\r\t") > line.length())
    {
      std::cout << "ligne vide" << std::endl;
      continue;
    }
    if (line.find("webserv") < line.length() && Parser::handleBlock(line, "webserv"))
      break;
    Parser::throwError("Parse Error: need a webserv block");
  }
}

bool  Parser::handleClosure(const std::string& line)
{
  if (line.find_first_not_of("} \n\t\r") < line.length() || std::count(line.begin(), line.end(), '}') != 1)
    Parser::throwError("Parse Error: Closure error in configuration file...");
  return true;
}

bool Parser::handleBlock(const std::string& line, const std::string type)
{
  std::size_t end = line.find(type) + type.length();
  if (!type.compare("location"))
  {
    end += (line.find_first_not_of(" \n\r\t", end) - end);
    end += (line.find_first_of(" \n\r\t", end) - end);
  }
  if (line.find_first_not_of("{ \n\r\t", end) < line.length() || std::count(line.begin(), line.end(), '{') != 1 )
    Parser::throwError("Parse Error: wrong block definition...");
  return true;
}

std::string Parser::getKey(const std::string& line)
{
  std::size_t begin = line.find_first_not_of(" \n\t\r");
  if (begin >= line.length())
    return std::string();
  std::size_t end = line.find_first_of(" \n\t\r", begin);
  if (end < line.length())
    return line.substr(begin, end - begin);
  return std::string();
}

std::string Parser::getValue(const std::string& key, const std::string& line)
{
  std::size_t begin = line.find(key);
  if (begin >= line.length())
    Parser::throwError("Parser Error: No value for instruction");
  begin += key.length();
  begin += (line.find_first_not_of(" \n\t\r", begin) - begin);
  std::size_t end = line.find_first_of(";", begin);
  if (end > line.length())
    Parser::throwError("Parser Error: Value need to terminate by ';'");
  return line.substr(begin, end - begin);
}

std::string Parser::getLocation(const std::string& line)
{
  std::string type = "location";
  std::size_t begin = line.find(type) + type.length();
  if (begin >= line.length())
    Parser::throwError("Parser Error: No path for bloc location");
  begin += (line.find_first_not_of(" \n\t\r", begin) - begin);
  std::size_t end = line.find_first_of("{ \n\t\r", begin);
  if (end > line.length())
    Parser::throwError("Parser Error: location line need to terminate by '{'");
  std::string location = line.substr(begin, end - begin);
  if (location.empty())
    Parser::throwError("No path for location bloc.");
  return location;
}

void  Parser::throwError(std::string message, Location* location)
{
  if (location)
    delete location;
  throw Parser::WebservParseException(message.insert(0, "Parser Error: "));
}
