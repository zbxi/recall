#include "program.hpp"

int main()
{
  try {
    zbxi::recall::Program{}.run();
  }
  catch(std::runtime_error const& e) {
    std::cout << "[RuntimeError] " << e.what() << std::endl;
  }
}
