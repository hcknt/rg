#include <ncurses.h>

#include <array>
#include <clocale>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
constexpr char WALL = '#';
constexpr const char* PLAYER_GLYPH = "☺";
constexpr const char* WALL_GLYPH = "▓";

constexpr std::array<const char*, 2> MAP_PATHS = {"maps/21x21.txt", "../maps/21x21.txt"};
} // namespace

class NcursesDemo {
public:
  NcursesDemo() = default;

  bool Init() {
    if (setlocale(LC_ALL, "") == nullptr) {
      return false;
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (!loadMapFromDisk()) {
      return false;
    }

    placePlayerAtSpawn();
    mIsRunning = true;
    return true;
  }

  void Run() {
    while (mIsRunning) {
      draw();
      const int ch = getch();
      handleInput(ch);
    }
  }

  void Shutdown() {
    mIsRunning = false;
    erase();
    mvaddstr(0, 0, "Press any key to exit.");
    refresh();
    getch();
    endwin();
  }

private:
  void draw() const {
    erase();

    mvaddstr(0, 0, "Use arrow keys or WASD to move. Press q to quit.");

    for (int y = 0; y < static_cast<int>(mMap.size()); ++y) {
      const std::string& row = mMap[y];
      for (int x = 0; x < static_cast<int>(row.size()); ++x) {
        const int drawY = y + 2;
        const int drawX = x;

        if (y == mPlayerY && x == mPlayerX) {
          mvaddstr(drawY, drawX, PLAYER_GLYPH);
        } else if (row[x] == WALL) {
          mvaddstr(drawY, drawX, WALL_GLYPH);
        } else {
          mvaddch(drawY, drawX, ' ');
        }
      }
    }

    std::ostringstream status;
    status << "Player: (" << mPlayerX << ", " << mPlayerY << ')';
    mvaddstr(static_cast<int>(mMap.size()) + 3, 0, status.str().c_str());
    refresh();
  }

  void handleInput(int ch) {
    int nextX = mPlayerX;
    int nextY = mPlayerY;

    switch (ch) {
    case KEY_UP:
    case 'w':
    case 'W':
      --nextY;
      break;
    case KEY_DOWN:
    case 's':
    case 'S':
      ++nextY;
      break;
    case KEY_LEFT:
    case 'a':
    case 'A':
      --nextX;
      break;
    case KEY_RIGHT:
    case 'd':
    case 'D':
      ++nextX;
      break;
    case 'q':
    case 'Q':
      mIsRunning = false;
      return;
    default:
      break;
    }

    if (isWalkable(nextY, nextX)) {
      mPlayerX = nextX;
      mPlayerY = nextY;
    }
  }

  [[nodiscard]] bool isWalkable(int y, int x) const {
    if (y < 0 || y >= static_cast<int>(mMap.size())) {
      return false;
    }
    if (x < 0 || x >= static_cast<int>(mMap[y].size())) {
      return false;
    }
    return mMap[y][x] != WALL;
  }

  void placePlayerAtSpawn() {
    mPlayerY = 1;
    mPlayerX = 1;
    if (isWalkable(mPlayerY, mPlayerX)) {
      return;
    }

    for (int y = 0; y < static_cast<int>(mMap.size()); ++y) {
      for (int x = 0; x < static_cast<int>(mMap[y].size()); ++x) {
        if (isWalkable(y, x)) {
          mPlayerY = y;
          mPlayerX = x;
          return;
        }
      }
    }

    mPlayerY = 0;
    mPlayerX = 0;
  }

  [[nodiscard]] bool loadMapFromDisk() {
    for (const char* path : MAP_PATHS) {
      std::ifstream file(path);
      if (!file.is_open()) {
        continue;
      }

      std::vector<std::string> loaded;
      std::string line;
      while (std::getline(file, line)) {
        if (line.empty()) {
          continue;
        }
        if (!loaded.empty() && line.size() != loaded.front().size()) {
          loaded.clear();
          break;
        }
        loaded.push_back(line);
      }

      if (!loaded.empty()) {
        mMap = loaded;
        return true;
      }
    }

    return false;
  }

  std::vector<std::string> mMap;
  int mPlayerX = 0;
  int mPlayerY = 0;
  bool mIsRunning = false;
};

int main() {
  NcursesDemo demo;
  if (!demo.Init()) {
    std::cerr << "Failed to initialize locale, ncurses, or load the map.\n";
    return 1;
  }

  demo.Run();
  demo.Shutdown();
  return 0;
}