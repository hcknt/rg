#include <stdlib.h>  // For system("clear")
#include <termios.h> // For terminal I/O settings
#include <unistd.h>  // For STDIN_FILENO, read

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// --- Platform-specific clear screen function ---
void ClearScreen() {
  system("clear");
}

// --- Platform-specific single-character input ---
// This is the proper POSIX way to get a single key
// press without needing the user to hit "Enter".
char GetInput() {
  struct termios oldt, newt;
  char c;

  // Get current terminal settings
  tcgetattr(STDIN_FILENO, &oldt);

  // Create new settings
  newt = oldt;

  // Disable canonical mode (line buffering) and echo
  newt.c_lflag &= ~(ICANON | ECHO);

  // Apply new settings immediately
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  // Read a single character
  c = getchar();

  // Restore old settings for std::cout
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  return c;
}

class Game {
public:
  std::vector<std::string> map;
  int playerX;
  int playerY;
  bool isRunning;

  Game() {
    // Initialize the game
    playerX = 10;
    playerY = 5;
    isRunning = true;

    if (!LoadMapFromDisk()) {
      std::cerr << "Failed to load map from maps/11x20.txt" << '\n';
      isRunning = false;
      return;
    }

    if (playerY < 0 || playerY >= static_cast<int>(map.size()) || playerX < 0 ||
        playerX >= static_cast<int>(map[playerY].size())) {
      std::cerr << "Player spawn position is outside the loaded map" << '\n';
      isRunning = false;
      return;
    }

    // Place player on the map
    map[playerY][playerX] = '@';
  }

  void Draw() {
    ClearScreen(); // Clear the console

    // Loop through the map and print it
    for (const std::string& row : map) {
      std::cout << row << '\n';
    }
    std::cout << "Use W, A, S, D to move. Q to quit." << '\n';
    std::cout << "Player position: (" << playerX << ", " << playerY << ")" << '\n';
  }

  void Update(char input) {
    // Erase old player position
    map[playerY][playerX] = ' ';

    // Update player position based on input
    int nextX = playerX;
    int nextY = playerY;

    switch (input) {
    case 'w':
      nextY--;
      break;
    case 's':
      nextY++;
      break;
    case 'a':
      nextX--;
      break;
    case 'd':
      nextX++;
      break;
    case 'q':
      isRunning = false;
      break;
    }

    // Check for collisions with walls
    if (nextY >= 0 && nextY < map.size() && nextX >= 0 && nextX < map[0].size() && map[nextY][nextX] != '#') {
      playerX = nextX;
      playerY = nextY;
    }

    map[playerY][playerX] = '@';
  }

private:
  bool LoadMapFromDisk();
};

bool Game::LoadMapFromDisk() {
  // Support running from repo root or tests/ via relative fallbacks
  const std::vector<std::string> mapPaths = {"maps/11x20.txt", "../maps/11x20.txt"};
  for (const std::string& path : mapPaths) {
    std::ifstream file(path);
    if (!file.is_open()) {
      continue;
    }

    map.clear();
    std::string line;
    while (std::getline(file, line)) {
      map.push_back(line);
    }

    if (!map.empty()) {
      return true;
    }
  }

  return false;
}

int main() {
  Game game;

  while (game.isRunning) {
    game.Draw();
    char input = GetInput(); // Get a single key press
    game.Update(input);
  }

  std::cout << "Game Over!" << '\n';
  return 0;
}
