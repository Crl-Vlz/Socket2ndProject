#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define TCP_PORT 5000
#define IP_ADDR "172.18.2.2"

void drawrock(WINDOW *win, int y, int x)
{
  mvwprintw(win, y, x, " ------");
  mvwprintw(win, y + 1, x, "/   $  \\");
  mvwprintw(win, y + 2, x, "| $^   |");
  mvwprintw(win, y + 3, x, "\\ $  & /");
  mvwprintw(win, y + 4, x, " ------");
}

void drawpaper(WINDOW *win, int y, int x)
{
  mvwprintw(win, y, x, " ------");
  mvwprintw(win, y + 1, x, "|      |");
  mvwprintw(win, y + 2, x, "|      |");
  mvwprintw(win, y + 3, x, "|      |");
  mvwprintw(win, y + 4, x, "|      |");
  mvwprintw(win, y + 5, x, "|      |");
  mvwprintw(win, y + 6, x, " ------");
}

void drawscissors(WINDOW *win, int y, int x)
{
  mvwprintw(win, y, x, "\\     /");
  mvwprintw(win, y + 1, x, "\\\\  //");
  mvwprintw(win, y + 2, x, " \\--/ ");
  mvwprintw(win, y + 3, x, " 0  0 ");
  mvwprintw(win, y + 4, x, "0 00 0");
  mvwprintw(win, y + 5, x, " 0  0 ");
}

void rectangle(WINDOW *win, int y1, int x1, int y2, int x2)
{
  mvwhline(win, y1, x1, 0, x2 - x1);
  mvwhline(win, y2, x1, 0, x2 - x1);
  mvwvline(win, y1, x1, 0, y2 - y1);
  mvwvline(win, y1, x2, 0, y2 - y1);
  mvaddch(y1, x1, ACS_ULCORNER);
  mvaddch(y2, x1, ACS_LLCORNER);
  mvaddch(y1, x2, ACS_URCORNER);
  mvaddch(y2, x2, ACS_LRCORNER);
}

void gamemenu(WINDOW *win)
{
  box(win, 0, 0);
  mvwprintw(win, 1, 11, "RPS");
  mvwprintw(win, 5, 2, "1. Play game");
  mvwprintw(win, 8, 2, "2. Help");
  mvwprintw(win, 11, 2, "3. Exit");
}

void handle_tcp_communication(WINDOW *win)
{

  int sock = 0;
  struct sockaddr_in serv_addr;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Socket creation error\n");
    close(sock);
    return;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(TCP_PORT);

  if (inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr) <= 0)
  {
    mvwprintw(win, 12, 2, "Communication Error");
    close(sock);
    return;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    mvwprintw(win, 12, 2, "Couldn't connect");
    close(sock);
    return;
  }

  werase(win);
  wrefresh(win);
  mvwprintw(win, 0, 25, "1. Rock 2.Paper 3.Scissors");
  rectangle(win, 2, 0, 10, 16);
  rectangle(win, 2, 60, 10, 76);
  wrefresh(win);
  int sym = 0;
  sym = wgetch(win) - '0';
  switch (sym)
  {
  case 1:
    drawrock(win, 3, 4);
    break;
  case 2:
    drawpaper(win, 3, 4);
    break;
  case 3:
    drawscissors(win, 3, 4);
    break;
  }
  wrefresh(win);
  // Send Data
  char message[1024];
  sprintf(message, "%d", sym);
  if (send(sock, message, 1024, 0) < 0)
  {
    mvwprintw(win, 12, 2, "Error sending data");
    close(sock);
    return;
  }
  // Read Data
  if (read(sock, message, 1024) < 0)
  {
    mvwprintw(win, 12, 2, "Error reading data");
    close(sock);
    return;
  }
  char me = message[0], res = message[1], other = message[2];
  // Print Opponent Action
  switch (other - '0')
  {
  case 1:
    drawrock(win, 3, 64);
    break;
  case 2:
    drawpaper(win, 3, 64);
    break;
  case 3:
    drawscissors(win, 3, 64);
    break;
  }
  // Print Result
  int mycode = me - '0', r = res - '0';
  if (r == 3)
  {
    mvwprintw(win, 12, 2, "It's a tie");
  }
  else if (r == mycode)
  {
    mvwprintw(win, 12, 2, "You won");
  }
  else
  {
    mvwprintw(win, 12, 2, "You lost");
  }
  close(sock);
}

int handle_login(WINDOW *win)
{
  werase(win);
  box(win, 0, 0);
  rectangle(win, 1, 10, 3, 40);
  mvwprintw(win, 2, 11, "Enter data");
  wgetch(win);
  echo();
  rectangle(win, 4, 10, 6, 40);
  mvwprintw(win, 5, 11, "Username>");
  wrefresh(win);
  char username[20];
  wscanw(win, "%s", username);
  char password[20];
  rectangle(win, 7, 10, 9, 40);
  mvwprintw(win, 8, 11, "Password>");
  wrefresh(win);
  wscanw(win, "%s", password);
  char pass[41];
  sprintf(pass, "%s:%s", username, password);
  // Pass the data

  int sock = 0;
  struct sockaddr_in serv_addr;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Socket creation error\n");
    close(sock);
    return -1;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(TCP_PORT);

  if (inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr) <= 0)
  {
    mvwprintw(win, 12, 2, "Communication Error");
    close(sock);
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    mvwprintw(win, 12, 2, "Couldn't connect");
    close(sock);
    return -1;
  }

  if (send(sock, "1", strlen("1"), 0) < 0)
  {
    mvwprintw(win, 12, 2, "Error sending data");
    close(sock);
    return -1;
  }

  if (send(sock, pass, 41, 0) < 0)
  {
    mvwprintw(win, 12, 2, "Error sending data");
    close(sock);
    return -1;
  }
  // Read Data
  if (read(sock, pass, 41) < 0)
  {
    mvwprintw(win, 12, 2, "Error reading data");
    close(sock);
    return -1;
  }

  int state = 0;
  if (pass[0] == '1'){
    mvwprintw(win, 12, 2, "User signed in");
    state = 1;
  }
  else{
    mvwprintw(win, 12, 2, "Couldn't sign in user");
  }

  close(sock);
  wgetch(win);
  noecho();
  return state;
}

int handle_signup(WINDOW *win)
{
  werase(win);
  box(win, 0, 0);
  rectangle(win, 1, 10, 3, 40);
  mvwprintw(win, 2, 11, "Enter data");
  wgetch(win);
  echo();
  rectangle(win, 4, 10, 6, 40);
  mvwprintw(win, 5, 11, "Username>");
  wrefresh(win);
  char username[20];
  wscanw(win, "%s", username);
  char password[20];
  rectangle(win, 7, 10, 9, 40);
  mvwprintw(win, 8, 11, "Password>");
  wrefresh(win);
  wscanw(win, "%s", password);
  char pass[41];
  sprintf(pass, "%s:%s", username, password);
  // Pass the data

  int sock = 0;
  struct sockaddr_in serv_addr;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Socket creation error\n");
    close(sock);
    return -1;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(TCP_PORT);

  if (inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr) <= 0)
  {
    mvwprintw(win, 12, 2, "Communication Error");
    close(sock);
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    mvwprintw(win, 12, 2, "Couldn't connect");
    close(sock);
    return -1;
  }

  if (send(sock, "2", strlen("2"), 0) < 0)
  {
    mvwprintw(win, 12, 2, "Error sending data");
    close(sock);
    return -1;
  }

  if (send(sock, pass, 41, 0) < 0)
  {
    mvwprintw(win, 12, 2, "Error sending data");
    close(sock);
    return -1;
  }
  // Read Data
  if (read(sock, pass, 41) < 0)
  {
    mvwprintw(win, 12, 2, "Error reading data");
    close(sock);
    return -1;
  }

  int state = 0;
  if (pass[0] == '1'){
    mvwprintw(win, 12, 2, "User signed in");
    state = 1;
  }
  else{
    mvwprintw(win, 12, 2, "Couldn't sign in user");
  }
  close(sock);
  wgetch(win);
  noecho();
  return state;
}

int mainmenu(WINDOW *win)
{
  werase(win);
  box(win, 0, 0);
  mvwprintw(win, 5, 12, "1. Login");
  mvwprintw(win, 7, 12, "2. Signup");
  mvwprintw(win, 9, 12, "3. Exit");
  wrefresh(win);
  int choice = 0;
  while (choice != 3)
  {
    choice = wgetch(win) - '0';
    switch (choice)
    {
    case 1:
      return handle_login(win);
    case 2:
      return handle_signup(win);
      break;
    case 3:
      delwin(win);
      refresh();
      endwin();
      exit(EXIT_SUCCESS);
      // Exit
      break;
    }
  }
}

int main()
{
  initscr();
  if (has_colors() == FALSE)
  {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);

  attron(COLOR_PAIR(1));
  WINDOW *mainwin = newwin(14, 80, (LINES - 14) / 2, (COLS - 80) / 2);

  int men = 0;
  while (men == 0)
    men = mainmenu(mainwin);
  werase(mainwin);

  box(mainwin, 0, 0);
  gamemenu(mainwin);
  wrefresh(mainwin);
  attroff(COLOR_PAIR(1));

  int choice = 0;
  while (choice != 3)
  {
    choice = wgetch(mainwin) - '0';
    switch (choice)
    {
    case 1:

      handle_tcp_communication(mainwin);

      wgetch(mainwin);
      werase(mainwin);
      gamemenu(mainwin);
      wrefresh(mainwin);
      // Play game
      break;
    case 2:
      werase(mainwin);
      wprintw(mainwin, "This program connects you with other players to have some ");
      wprintw(mainwin, "intense round of rock paper scissors!\n");
      wprintw(mainwin, "Remember:\n");
      wprintw(mainwin, "ROCK --> SCISSORS:\n");
      wprintw(mainwin, "SCISSORS --> PAPER:\n");
      wprintw(mainwin, "PAPER --> ROCK:\n");
      wrefresh(mainwin);
      wgetch(mainwin);
      werase(mainwin);
      gamemenu(mainwin);
      wrefresh(mainwin);
      // Help
      break;
    case 3:
      // Exit
      break;
    }
  }

  delwin(mainwin);
  refresh();
  endwin();
  return 0;
}
