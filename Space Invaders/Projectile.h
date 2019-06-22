#include <string>
#include <vector>

struct proObj {
  const char proj = '!';
  int x;
  int y;
  bool flying = false;
  void fired() {
    flying = !flying;
  }
  void path(std::vector <std::string>& mymap) {
    mymap.at(y).at(x) = ' ';
    --y;
  }
};