#include "Response.hpp"

Response::Response()
{
}

Response::Response(const Response& cpy)
{
  (void)cpy;
}

Response& Response::operator=(Response& cpy)
{
  return cpy;
}

Response::~Response()
{
}

std::string Response::get() const
{
  std::string response = "HTTP/1.1 200 OK\r\nDate: Mon, 10 Feb 2025 14:00:00 GMT\r\nServer: nginx/1.21.6\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 99\r\nConnection: keep-alive\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head><title>Success</title></head>\r\n<body><h1>200 OK</h1></body>\r\n</html>";
  return response;
}
