#include "Client.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const int PORT = 8080;
const int MAX_EVENTS = 10;

void setNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

std::string fileToText() {
  std::string line;
  std::string test;
  std::ifstream file;

  file.open("test.html");
  while (std::getline(file, line)) {
    test.append(line);
  }

  std::cout << test.size() << std::endl;

  return test;
}

// void handleClient(int clientSocket) {
//   char buffer[BUFFER_SIZE];
//   ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
//   if (bytesRead > 0) {
//     buffer[bytesRead] = '\0';
//     std::cout << "Request:\n" << buffer << std::endl;
//
//     // std::string test = fileToText();
//     //
//     // std::string response = "HTTP/1.1 200 OK\nContent-Length: 363\n\n";
//     // response.append(test);
//
//     std::string response =
//         "HTTP/1.1 200 OK\nContent-Length: 13\n\nHello World!";
//
//     send(clientSocket, response.c_str(), response.size(), 0);
//   }
//   close(clientSocket);
// }

int main() {
  int serverSocket, clientSocket;
  struct sockaddr_in serverAddr;
  int epollFd;
  struct epoll_event event, events[MAX_EVENTS];

  // Create socket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    std::cerr << "Error creating socket" << std::endl;
    return 1;
  }

  // Set socket to non-blocking
  setNonBlocking(serverSocket);

  // Bind socket
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(PORT);
  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    std::cerr << "Error binding socket" << std::endl;
    close(serverSocket);
    return 1;
  }

  // Listen for connections
  if (listen(serverSocket, SOMAXCONN) < 0) {
    std::cerr << "Error listening" << std::endl;
    close(serverSocket);
    return 1;
  }

  std::cout << "Server listening on port " << PORT << std::endl;

  // Create epoll instance
  epollFd = epoll_create1(0);
  if (epollFd < 0) {
    std::cerr << "Error creating epoll instance" << std::endl;
    close(serverSocket);
    return 1;
  }

  // Add server socket to epoll
  event.events = EPOLLIN;
  event.data.fd = serverSocket;
  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &event) < 0) {
    std::cerr << "Error adding server socket to epoll" << std::endl;
    close(serverSocket);
    close(epollFd);
    return 1;
  }

  // Client *client;
  std::map<int, Client *> clientMap;

  while (true) {
    int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
    if (numEvents < 0) {
      std::cerr << "Error in epoll_wait" << std::endl;
      break;
    }

    for (int i = 0; i < numEvents; ++i) {
      if (events[i].data.fd == serverSocket) {
        // Accept new connection
        clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket < 0) {
          std::cerr << "Error accepting connection" << std::endl;
          continue;
        }

        // Set client socket to non-blocking
        setNonBlocking(clientSocket);

        // Add client socket to epoll
        event.events = EPOLLIN | EPOLLET; // Edge-triggered mode
        event.data.fd = clientSocket;

        clientMap[clientSocket] = new Client(serverSocket, clientSocket);

        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) < 0) {
          std::cerr << "Error adding client socket to epoll" << std::endl;
          close(clientSocket);
        }
      } else {
        // handleClient(events[i].data.fd);
        Client *client = clientMap[events[i].data.fd];

        if (client->receive()) {
          Request *req = client->getCurrentRequest();

          std::cout << req->getRawData() << std::endl;

          std::string response =
              "HTTP/1.1 200 OK\nContent-Length: 13\n\nHello World!";

          send(events[i].data.fd, response.c_str(), response.size(), 0);
        }
      }
    }
  }

  close(serverSocket);
  close(epollFd);
  return 0;
}
