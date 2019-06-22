struct invObj {
  char alien = 'A';
  bool alive;
  int x;
  int y;
  bool moveright;

  invObj() {
    alive = true;
    x = 1;
    y = 1;
    moveright = true;
  }
  void kill() { alive = false; }
  void direction() {
    moveright = !moveright;
  }
};