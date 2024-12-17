#include "RPN.hpp"
#include <iostream>

int main(int ac, char **av) {
  if (ac != 2) {
    std::cout << "Error: wrong number of arguments" << std::endl;

    return 1;
  }

  try {
    const RPN rpn(av[1]);

    rpn.printResult();
  } catch (const std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  }

  return 0;
}
