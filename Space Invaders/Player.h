#include <string>
#include <vector>

struct pacObj {
  const char pacch = '^';
  int x = 4;
  int y = 6;
  void takestep(std::vector <std::string>& mymap, bool rightdir) {
    mymap.at(y).at(x) = ' ';
    rightdir ? ++x : --x;
  }
};