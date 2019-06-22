#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

#include "gotoxy.h"
#include "Invader.h"
#include "Player.h"
#include "Projectile.h"

using namespace std;

const int INIT_INVADERS = 7;

//Loads map from file
void loadmap(vector<string>& mapchoice) {
  ifstream mapFile;
  string map,
    input;
  do {
    cout << "The name of the map to play (i.e. map1.txt): " << endl;
    getline(cin, map);
    mapFile.open(map);
    if (!mapFile)
      cout << "Map not found!" << endl;
  } while (!mapFile);

  while (getline(mapFile, input)) {
    mapchoice.push_back(input);
  }
  system("cls");
}

//init all elements related to the game (your initial location, aliens, map, etc)
void init(pacObj& pac, vector <invObj>& invaders, vector<string>& mymap) {
  loadmap(mymap);
  invObj invader;

  for (int i = 0; i < INIT_INVADERS; i++) {
    invader.x = i + 1;
    invaders.push_back(invader);
    mymap.at(invader.y).at(invader.x) = invader.alien;
  }
  pac.x = mymap.at(0).size() / 2;
  pac.y = mymap.size() - 2;

  mymap.at(pac.y).at(pac.x) = pac.pacch;
  for (string line : mymap) {
    cout << line << endl;
  }

}//init

//Draws and update the screen
void updateScreen(pacObj& pac, vector <invObj>& invaders, vector<string>& mymap, proObj bullet, int score)
{

  gotoxy(0, 0);

  //Draws invaders
  for (int i = 0; i < invaders.size(); i++) {
    if (invaders.at(i).alive) {
      mymap.at(invaders.at(i).y).at(invaders.at(i).x) = invaders.at(i).alien;
    }
    else {
      mymap.at(invaders.at(i).y).at(invaders.at(i).x) = ' ';
    }
  }

  //Draw projectile
  mymap.at(pac.y).at(pac.x) = pac.pacch;
  if (bullet.flying) {
    mymap.at(bullet.y).at(bullet.x) = bullet.proj;
  }

  //Draw map
  for (string line : mymap) {
    cout << line << endl;
  }
  cout << "Score: " << score << endl;
}

void moveInvaders(vector <invObj>& invaders, vector<string>& map, bool& gameOver, proObj& bullet) {
  int x, j;
  //Move left or right
  if (invaders.at(0).moveright) {
    x = invaders.back().x + 1;
    j = invaders.at(0).x;
  }
  else {
    x = invaders.at(0).x - 1;
    j = invaders.back().x;
  }

  //Collision Detection
  switch (map.at(invaders.at(0).y).at(x)) {
    case 'x':
    case ' ':
    case '!':
      map.at(invaders.at(0).y).at(j) = ' ';
      for (int i = 0; i < invaders.size(); i++) {
        invaders.at(i).x = invaders.at(i).x + 1 - (2 * (!invaders.at(0).moveright));
      }
      break;

    case '#':
      invaders.at(0).direction();
      for (int i = 0; i < invaders.size(); i++) {
        map.at(invaders.at(i).y).at(invaders.at(i).x) = ' ';
        invaders.at(i).y += 1;
      }
      if (invaders.at(0).y == map.size() - 1) {
        gameOver = false;
      }
      break;

    case '^':
      gameOver = false;
      break;
    }

}

//Updates the size of invaders
void updateInvaders(vector <invObj>& invaders, vector<string>& map) {
  if (invaders.size() > 1) {
    while (!invaders.front().alive) {
      map.at(invaders.front().y).at(invaders.front().x) = ' ';
      invaders.erase(invaders.begin());
    }
    while (!invaders.back().alive) {
      map.at(invaders.back().y).at(invaders.back().x) = ' ';
      invaders.pop_back();
    }
  }
}

void fireBullet(vector <invObj>& invaders, vector<string>& map, proObj& bullet, int& score) {
  //Checks if Alien walks into projectile
  for (int i = 0; i < invaders.size(); i++) {
    if ((invaders.at(i).x == bullet.x) && (invaders.at(i).y == bullet.y) && invaders.at(i).alive) {
      invaders.at(i).kill();
      bullet.fired();
      score++;
    }
  }

  //Update Position of projectile
  bullet.path(map);

  //Checks new position of the projectile
  switch (map.at(bullet.y).at(bullet.x)) {
  case '#':
  case 'x':
    bullet.fired();
    break;

  case 'A':
    bullet.fired();
    for (int i = 0; i < invaders.size(); i++) {
      if (invaders.at(i).x == bullet.x && invaders.at(i).alive) {
        score++;
        invaders.at(i).kill();
      }
    }
    break;
  }

}

int main()
{

  int speed = 100;
  int score = 0;
  bool game_running = true;
  vector <invObj> invaders;
  vector<string> mymap;
  pacObj pac;
  proObj bullet;

  init(pac, invaders, mymap);

  do {
    this_thread::sleep_for(chrono::milliseconds(speed));

    if (GetAsyncKeyState(VK_RIGHT) && pac.x < mymap.at(0).size() - 2) {
      pac.takestep(mymap, true);
    }
    if (GetAsyncKeyState(VK_LEFT) && pac.x > 1) {
      pac.takestep(mymap, false);
    }

    if (GetAsyncKeyState(VK_SPACE) && !bullet.flying) {
      bullet.x = pac.x;
      bullet.y = pac.y;
      bullet.fired();
    }

    //speed
    if (GetAsyncKeyState(VK_DOWN)) {
      speed += 10;
      if (speed > 200) {
        speed -= 10;
      }
    }
    if (GetAsyncKeyState(VK_UP)) {
      speed -= 10;
      if (speed < 10) {
        speed += 10;
      }
    }

    //exits out of game
    if (GetAsyncKeyState(VK_ESCAPE)) {
      game_running = false;
    }
    if (bullet.flying) {
      fireBullet(invaders, mymap, bullet, score);
    }

    updateInvaders(invaders, mymap);
    moveInvaders(invaders, mymap, game_running, bullet);
    updateScreen(pac, invaders, mymap, bullet, score);
  } while ((game_running == true) && (score != INIT_INVADERS));

  cout << "\n\nGAME OVER";
  system("pause>nul");

  return 0;
}