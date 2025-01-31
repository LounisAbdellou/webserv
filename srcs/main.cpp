#include <iostream>
#include <string>
#include <csignal>
#include "Webserv.hpp"

void  handle_signal(int signal)
{
  (void)signal;
  std::cout << std::endl;
  Webserv::is_running = false;
}

int main(int ac, char **av) {
  if (ac > 2) {
    std::cout << "Error: wrong number of arguments" << std::endl;
    return 1;
  }

  Webserv     w;
  std::string filename;

  if (ac == 2)
    filename = av[1];
  std::cout << "Launching of Webserv..." << std::endl;
  try {
    w.configure(filename);
    w.init();
    signal(SIGINT, handle_signal);
    signal(SIGQUIT, handle_signal);
    w.run();
  }
  catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  signal(SIGINT, SIG_DFL);
  signal(SIGQUIT, SIG_DFL);
  return 0;
}
