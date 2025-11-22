#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

constexpr int GRID_SIZE = 25;

struct GridPoint {
  int x = 0;
  int y = 0;
};

struct Point {
  double x = 0.0;
  double y = 0.0;
};

GridPoint RoundPoint(const Point& p) {
  return {.x = static_cast<int>(std::round(p.x)), .y = static_cast<int>(std::round(p.y))};
}

Point LerpPoint(const Point& p0, const Point& p1, double t) {
  return {.x = p0.x + ((p1.x - p0.x) * t), .y = p0.y + ((p1.y - p0.y) * t)};
}

int DiagonalDistance(const GridPoint& p0, const GridPoint& p1) {
  return std::max(std::abs(p1.x - p0.x), std::abs(p1.y - p0.y));
}

std::vector<GridPoint> Line(const GridPoint& p0, const GridPoint& p1) {
  std::vector<GridPoint> points;
  const int steps = DiagonalDistance(p0, p1);
  const Point start{.x = static_cast<double>(p0.x), .y = static_cast<double>(p0.y)};
  const Point goal{.x = static_cast<double>(p1.x), .y = static_cast<double>(p1.y)};

  for (int step = 0; step <= steps; ++step) {
    const double t = steps == 0 ? 0.0 : static_cast<double>(step) / steps;
    points.push_back(RoundPoint(LerpPoint(start, goal, t)));
  }

  return points;
}

bool ReadPoint(const std::string& label, GridPoint& outPoint) {
  std::cout << label << " (x y): ";
  if (!(std::cin >> outPoint.x >> outPoint.y)) {
    return false;
  }
  if (outPoint.x >= 0 && outPoint.x < GRID_SIZE && outPoint.y >= 0 && outPoint.y < GRID_SIZE) {
    return true;
  }
  return false;
}

void RenderGrid(const std::vector<GridPoint>& points, const GridPoint& start, const GridPoint& end) {
  std::vector<std::string> grid(GRID_SIZE, std::string(GRID_SIZE, '.'));

  for (const auto& p : points) {
    if (p.x >= 0 && p.x < GRID_SIZE && p.y >= 0 && p.y < GRID_SIZE) {
      grid[p.y][p.x] = '#';
    }
  }

  grid[start.y][start.x] = 'A';
  grid[end.y][end.x] = 'B';

  for (int y = GRID_SIZE - 1; y >= 0; --y) {
    for (int x = 0; x < GRID_SIZE; ++x) {
      std::cout << grid[y][x];
    }
    std::cout << '\n';
  }
}

int main() {
  GridPoint start{};
  GridPoint end{};
  if (!ReadPoint("Start", start) || !ReadPoint("End", end)) {
    return 1;
  }

  const auto points = Line(start, end);
  RenderGrid(points, start, end);

  return 0;
}
