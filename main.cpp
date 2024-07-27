#include <ncurses.h>
#include <unistd.h>  // for usleep
#include <iostream>
#include <vector>

using namespace std;

const int WIDTH = 50;
const int HEIGHT = 20;
const char BULLET = '|';
const char BOX = '#';
const char EMPTY = ' ';
const char WIDGET_CHAR = '=';
const int WIDGET_WIDTH = 3;
const int WIDGET_HEIGHT = 1;
int widgetX = WIDTH / 2;
int score = 0;
bool gameOver = false;

struct Bullet {
  int x, y;
};

struct Box {
  int x, y;
};

vector<Bullet> bullets;
vector<Box> boxes;

void draw() {
  clear();

  // Draw the widget
  for (int i = 0; i < WIDGET_WIDTH; ++i) {
    mvaddch(HEIGHT - WIDGET_HEIGHT, widgetX + i, WIDGET_CHAR);
  }

  // Draw bullets
  for (auto& bullet : bullets)
    if (bullet.y >= 0 && bullet.y < HEIGHT) mvaddch(bullet.y, bullet.x, BULLET);

  // Draw boxes
  for (auto& box : boxes)
    if (box.y >= 0 && box.y < HEIGHT) mvaddch(box.y, box.x, BOX);

  mvprintw(HEIGHT, 0, "Score: %d", score);
  refresh();
}

void update() {
  for (auto& bullet : bullets) bullet.y -= 1;

  for (auto& box : boxes) box.y += 1;

  bullets.erase(
      remove_if(
          bullets.begin(), bullets.end(), [](Bullet& b) { return b.y < 0; }),
      bullets.end());
  boxes.erase(
      remove_if(
          boxes.begin(), boxes.end(), [](Box& b) { return b.y >= HEIGHT; }),
      boxes.end());
}

void checkCollisions() {
  for (auto bullet = bullets.begin(); bullet != bullets.end(); ++bullet) {
    for (auto box = boxes.begin(); box != boxes.end();) {
      if (bullet->x == box->x && bullet->y == box->y) {
        box = boxes.erase(box);
        bullet = bullets.erase(bullet);
        score++;
        break;
      } else {
        ++box;
      }
    }
  }

  for (auto& box : boxes)
    if (box.y == HEIGHT - 1 && box.x >= widgetX &&
        box.x < widgetX + WIDGET_WIDTH)
      gameOver = true;
}

void shootBullet() {
  bullets.push_back({widgetX + WIDGET_WIDTH / 2, HEIGHT - WIDGET_HEIGHT - 1});
}

void spawnBox() {
  boxes.push_back({rand() % WIDTH, 0});
}

void moveWidget(int direction) {
  if (direction == KEY_LEFT && widgetX > 0)
    widgetX--;
  else if (direction == KEY_RIGHT && widgetX + WIDGET_WIDTH < WIDTH)
    widgetX++;
}

int main() {
  initscr();
  noecho();
  curs_set(FALSE);
  keypad(stdscr, TRUE);  // Enable reading arrow keys

  timeout(100);  // Non-blocking getch

  while (!gameOver) {
    int ch = getch();
    switch (ch) {
      case ' ':
        shootBullet();
        break;
      case KEY_LEFT:
      case KEY_RIGHT:
        moveWidget(ch);
        break;
    }

    if (rand() % 10 < 1) spawnBox();

    update();
    checkCollisions();
    draw();

    usleep(100000);
  }

  endwin();

  cout << "Game Over! Boxes destroyed: " << score << endl;
  return 0;
}
