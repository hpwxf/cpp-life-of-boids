#include <iostream>

// Classical test are executable with main function
// A test is failed if the execution crashed or if the main function returns a non zero exit code
int main() {
  int x = 1;
  if (x != x) {
    return 1;
  }
  return 0;  // OK !
}