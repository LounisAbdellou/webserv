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
      Parser::throwError("Problem occurs while reading file");
    }
    if (line.find_first_not_of(" \n\r\t") > line.length())
    {
      std::cout << "ligne vide" << std::endl;
      continue;
    }
    if (line.find("webserv") < line.length() && Parser::handleBlock(line, "webserv"))
      break;
    Parser::throwError("Must begin with a webserv block");
  }
}

bool  Parser::handleClosure(const std::string& line)
{
  if (line.find_first_not_of("} \n\t\r") < line.length() || std::count(line.begin(), line.end(), '}') != 1)
    Parser::throwError("Closure error in configuration file...");
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
    Parser::throwError("Wrong block definition...");
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
    Parser::throwError("No value for instruction");
  begin += key.length();
  begin += (line.find_first_not_of(" \n\t\r", begin) - begin);
  std::size_t end = line.find_first_of(";", begin);
  if (end > line.length())
    Parser::throwError("Value must terminate by ';'");
  return line.substr(begin, end - begin);
}

std::string Parser::getLocation(const std::string& line)
{
  std::string type = "location";
  std::size_t begin = line.find(type) + type.length();
  if (begin >= line.length())
    Parser::throwError("No path for bloc location");
  begin += (line.find_first_not_of(" \n\t\r", begin) - begin);
  std::size_t end = line.find_first_of("{ \n\t\r", begin);
  if (end > line.length())
    Parser::throwError("Location line must terminate by '{'");
  std::string location = line.substr(begin, end - begin);
  if (location.empty())
    Parser::throwError("No path for location bloc.");
  return location;
}

std::string Parser::getEntryHtmlTag(struct Entry entry, std::string &path) {
	std::string htmlTag = "<tr><td>";
	
	if (path[path.size() - 1] != '/') {
		path.append("/");
	} 

	if (entry.isDir)
		htmlTag.append("&#128193 ");
	else
		htmlTag.append("&#128196 ");

	htmlTag.append("<a href=\"" + path + entry.name  + "\">" + entry.name + "</a>");
	htmlTag.append("</td></tr>");

	return htmlTag;
}

std::string	Parser::getListingHtml(const std::vector<struct Entry> &entries, const std::string &path) {
	std::string bPath = path.substr(9, path.size() - 9);

	std::vector<struct Entry>::const_reverse_iterator it;
	std::string listingHtml = "<html><head><title>Index of " + bPath + "</title>\
														</head><body><h1>Index of " + bPath + "</h1>\
														<table><thead><tr><th></th></tr></thead><tbody>";

	if (entries.empty()) {
		listingHtml.append("Folder Empty :(");
	} else {
		for (it = entries.rbegin(); it != entries.rend(); it++) {
			listingHtml.append(Parser::getEntryHtmlTag(*it, bPath));
		}
	}

	listingHtml.append("</tbody></table></body></html>");

	return listingHtml;
}

std::vector<std::string> Parser::getRequestLine(const std::string &header, size_t &begin) {
	std::vector<std::string> requestLine;

	while (requestLine.size() < 3) {
		const std::string separator = (requestLine.size() < 2) ? " " : "\r\n";
		size_t end = header.find(separator, begin);

		if (end == std::string::npos) { 
			return requestLine;
		}

		requestLine.push_back(header.substr(begin, end - begin));
		begin = end + separator.size();
	}

	return requestLine;
}

std::vector<std::string> Parser::getHeaderAttr(const std::string &header, size_t &begin) {
	std::vector<std::string> attribute;
	size_t end = header.find(": ", begin);

	if (end == std::string::npos) {
		return attribute;
	}

	attribute.push_back(header.substr(begin, end - begin));
	begin = end + 2;

	end = header.find("\r\n", begin);
	if (end == std::string::npos) {
		return attribute;
	}

	attribute.push_back(header.substr(begin, end - begin));
	begin = end + 2;

	return attribute;
}

long long Parser::strtoll(const std::string &str, int base) {
  long long result = 0;
  std::istringstream iss(str);
  std::string::const_iterator it = str.begin();

  if (*it == '-' || str.empty()) {
    return -1;
  }

  it++;

  if (base == 10) {
    for (; it != str.end(); it++) {
      if (!std::isdigit(*it)) {
        return -1;
      }
    }

    iss >> std::dec >> result;
  } else if (base == 16) {
    for (; it != str.end(); it++) {
      if (!std::isdigit(*it) && *it < 65 && *it > 70 && *it < 97 && *it > 102) {
        return -1;
      }
    }

    iss >> std::hex >> result;
  }

  return result;
}

std::string Parser::to_string(long long nbr) {
	int tmp;
	std::string str;

	while (nbr) {
		tmp = nbr % 10;
		str.insert(str.begin(), tmp + 48);
		nbr /= 10;
	}

	return str.empty() ? "0" : str;
}

std::string Parser::getExtension(const std::string& path)
{
  std::size_t dot = path.find_last_of(".");
  if (dot > path.length())
    return "";
  return path.substr(dot, path.length() - dot);
}

std::string Parser::getFolder(const std::string& path)
{
  std::size_t slash = path.find_last_of("/");
  if (slash > path.length())
    return path.substr(0);
  return path.substr(0, slash);
}

std::vector<std::string>  Parser::getSocketInfo(int socket)
{
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  std::vector<std::string> host;

  if (getsockname(socket, (struct sockaddr *)&addr, &addr_len) == -1) 
  {
    host.push_back("");
    host.push_back("");
    return host;
  }
  char ip_addr[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(addr.sin_addr), ip_addr, INET_ADDRSTRLEN);
  std::string ip = ip_addr;
  host.push_back(ip);
  std::string port = Parser::to_string(ntohs(addr.sin_port));
  host.push_back(port);

  return host;
}

void  Parser::throwError(std::string message, Location* location)
{
  if (location)
    delete location;
  throw Parser::WebservParseException(message.insert(0, "Parser Error: "));
}
