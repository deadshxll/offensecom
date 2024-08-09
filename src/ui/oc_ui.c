#include <ctype.h>
#include <math.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>

#include "../net/oc_socket.h"
#include "../oc_helper.h"
#include "../offensecom.h"
#include "oc_cmd_manager.h"
#include "oc_ui.h"

int colorcode_to_pair(int colorcode);
void printw_colored(char *format, ...);
void handle_input(int c);
void draw_chatbox();
void draw_statusbar();
void draw_inputbox();

char input_box_contents[2048] = "";

void offensecom_ui_initialize() {
  chat_message *tail;
  tail = (chat_message *)malloc(sizeof(chat_message));
  strlcpy(tail->username, "", 64);
  strlcpy(tail->contents, "&4&lOFFENSECOM CLIENT v1.1_dev", 2048);
  tail->previous = NULL;

  latest_message = tail;

  initscr();             // Initializes ncurses
  nodelay(stdscr, true); // Stops waiting for 'getch' to recieve input
  noecho();              // Stop writing whatever the user types on the screen
  start_color();         // Enables use of color manipulation routines
  use_default_colors();  // Allow the terminal to reset color to an unspecified
                         // default value
  keypad(stdscr, TRUE);  // Capture input

  // Initialze the required color pairs
  init_pair(1, COLOR_BLACK, -1);
  init_pair(2, COLOR_BLUE, -1);
  init_pair(3, COLOR_GREEN, -1);
  init_pair(4, COLOR_CYAN, -1);
  init_pair(5, COLOR_RED, -1);
  init_pair(6, COLOR_MAGENTA, -1);
  init_pair(7, COLOR_YELLOW, -1);
  init_pair(8, COLOR_WHITE, -1);

  init_pair(9, COLOR_WHITE, COLOR_RED); // Status bar colors
}

void offensecom_ui_start() {
  oc_ui_running = 1;

  while (oc_ui_running == 1) {
    // Step 1. Collect keyboard input
    handle_input(getch());

    // Step 2. Draw the chat box
    draw_chatbox();

    // Step 3. Draw the status bar
    draw_statusbar();

    // Step 4. Draw the input box
    draw_inputbox();

    sleep_ms(1000 / ui_refresh_rate);
  }

  offensecom_ui_terminate();
}

/*
Terminate the offensecom user interface
*/
void offensecom_ui_terminate() {
  oc_ui_running = 0;
  nodelay(stdscr, false);
  endwin();
  exit(0);
}

void offensecom_ui_createmessage(char *username, char *message) {

  if (strlen(message) > 0) {
    chat_message *new;
    new = (chat_message *)malloc(sizeof(chat_message));
    strlcpy(new->username, username, 64);
    strlcpy(new->contents, message, 2048);
    new->previous = latest_message;
    latest_message = new;
  }
}

/*
Input Handling
*/
void handle_input(int c) {
  if (c == -1)
    return;

  if (c == 263) { // If the key pressed is 'backspace'
    input_box_contents[strlen(input_box_contents) - 1] = '\0';
  } else if (c == 10) { // If the key pressed is 'enter'
    if (input_box_contents[0] == '/') {
      oc_cmd_manager_handle_command(input_box_contents);
    } else {
      offensecom_ui_createmessage(user_name, input_box_contents);
      if (offensecom_socket_is_connected() == 1)
        offensecom_socket_send(input_box_contents);
    }
    strcpy(input_box_contents, "");
  } else if (isprint(c)) {
    char tmp[2] = {c};
    strlcat(input_box_contents, tmp, 2048);
  }
}

/*

DRAWING FUNCTIONS

*/

void draw_chatbox() {
  int uimax_x, uimax_y;                        // Terminal size
  int cursor_y;                                // Line cursor
  chat_message *curr_message = latest_message; // Current message

  getmaxyx(stdscr, uimax_y, uimax_x);

  cursor_y = uimax_y - 4;

  while (curr_message != NULL && cursor_y > -1) {
    // Since lines can be multiple lines, we need to determine how many lines
    // the message is.
    // An empty username is considered to be a Non-User message, such as a
    // server message.

    int username_length = strlen(curr_message->username);
    int message_length = strlen(curr_message->contents);

    char tmp[2048];
    if (username_length < 1) { // Empty username
      strlcpy(tmp, curr_message->contents, 2047);
    } else {
      sprintf(tmp, "<%s> %s", curr_message->username, curr_message->contents);
    }

    int line_count = floor(strlen(tmp) / uimax_x) + 1;
    for (int i = 0; i < message_length; i++) {
      if (curr_message->contents[i] == '\n') {
        line_count++;
      }
    }

    // Now before we print our messages, we must clear the lines on our way to
    // where we want to write.
    for (int i = 0; i < line_count; i++) {
      move(cursor_y - i, 0);
      clrtoeol();
    }
    // Update the cursor for the next iteration
    cursor_y = cursor_y - line_count;

    if (username_length > 1) {
      printw_colored("&4<&r%s&4>&r ", curr_message->username);
    }

    printw_colored("%s", curr_message->contents);

    curr_message = curr_message->previous;
  }
}

void draw_statusbar() {
  int uimax_x, uimax_y; // Terminal Size
  getmaxyx(stdscr, uimax_y, uimax_x);

  // Cursor positions
  int x = 2;
  int y = uimax_y - 3;

  // DRAWING THE BANNER
  attron(COLOR_PAIR(9));
  move(y, 0);
  for (int i = 0; i < uimax_x; i++) {
    addch(' ');
  }

  move(y, uimax_x - strlen(client_name) - 2);
  printw("%s", client_name);

  // GATHERING THE DATA FOR THE STATUS BAR
  char local_time[256];
  char name[256];
  char is_connected[256];
  char latency[256];

  // Time data
  time_t t = time(NULL);
  struct tm lt = *localtime(&t);
  sprintf(local_time, "%02d:%02d:%02d", lt.tm_hour, lt.tm_min, lt.tm_sec);

  // Username data
  sprintf(name, "/U:'%s'", user_name);

  // Connection data
  if (offensecom_socket_is_connected() == 1)
    sprintf(is_connected, "/C:True"); // placeholder
  else
    sprintf(is_connected, "/C:False"); // placeholder

  // Latency data
  sprintf(latency, "/L:-1ms"); // placeholder

  // DRAWING THE DATA
  move(y, x);
  printw("%s", local_time);
  x = x + strlen(local_time) + 2;

  move(y, x);
  printw("%s", name);
  x = x + strlen(name) + 2;

  move(y, x);
  printw("%s", is_connected);
  x = x + strlen(is_connected) + 2;

  move(y, x);
  printw("%s", latency);
  x = x + strlen(latency) + 2;

  attroff(COLOR_PAIR(9));
}

// TODO: make the input scroll into view of what the user is typing
void draw_inputbox() {
  int uimax_x, uimax_y;

  getmaxyx(stdscr, uimax_y, uimax_x);

  move(uimax_y - 2, 0);
  clrtoeol();
  move(uimax_y - 1, 0);
  clrtoeol();

  move(uimax_y - 2, 0);
  printw(" [#%s] %s", chat_name, input_box_contents);
}

/*

HELPERS / OTHER

*/

/*
Print strings with colors

How this works is very similar to how Mojang implemented color coding in
Minecraft. An example would be "hello &4world&r!"
*/
void printw_colored(char *format, ...) {
  int pairs_used_amount; // Iterator(s)
  int current_pair;      // The current pair in use
  int pairs_used[128]; // The codes that were used while printing the string, we
                       // need this to deactivate the codes once finished.
  char buffer[2048];   // The formatted string

  // Here we format the string provided using 'vsprintf' with the arguments
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  pairs_used_amount = 0;
  for (int i = 0; buffer[i] != '\0'; i++) { // Iterating the buffer string
    // Checking if there is a color code being used
    if (buffer[i] == '&' && buffer[i + 1] != '\0') {
      // Converting the color code to a color pair for use by ncurses
      int pair = colorcode_to_pair(buffer[i + 1]);

      if (pair != -1) {  // If the code is valid
        if (pair == 0) { // If the pair is the reset code
          // Iterating through all the codes used and turning off their
          // attribute
          for (int j = 0; j < pairs_used_amount; j++) {
            attroff(pairs_used[j]);
          }
        } else { // If the pair is NOT the reset code
          attron(pair);

          // Checking if the pair is already in the 'pairs_used' array.
          // We could just not do this, but it's done for the sake of security
          bool is_present_in_array = false;
          for (int j = 0; j < pairs_used_amount - 1; j++) {
            if (pairs_used[j] == pair) {
              is_present_in_array = true;
              break;
            }
          }
          if (is_present_in_array == false)
            pairs_used[pairs_used_amount] = pair;

          pairs_used_amount++;
        }

        current_pair = pair;
        i++; // Skipping the color code
      }
    } else { // If there is no color code
      addch(buffer[i]);
    }
  }

  for (int j = 0; j < pairs_used_amount; j++) {
    attroff(pairs_used[j]);
  }
}

int colorcode_to_pair(int colorcode) {
  switch (colorcode) {
  case '0':
  case '8':
    return COLOR_PAIR(1); // &0 or &8 = black

  case '1':
  case '9':
    return COLOR_PAIR(2); // &1 or &9 = blue

  case '2':
  case 'a':
    return COLOR_PAIR(3); // &2 or &a = green

  case '3':
  case 'b':
    return COLOR_PAIR(4); // &3 or &b = cyan

  case '4':
  case 'c':
    return COLOR_PAIR(5); // &4 or &c = red

  case '5':
  case 'd':
    return COLOR_PAIR(6); // &5 or &d = magenta

  case '6':
  case 'e':
    return COLOR_PAIR(7); // &6 or &e = yellow

  case '7':
  case 'f':
    return COLOR_PAIR(8); // &7 or &f = white

  case 'l':
    return A_BOLD; // &l = bold

  case 'n':
    return A_UNDERLINE; // &n = underline

  case 'k':
    return A_ALTCHARSET; // &k = obfuscate

  case 'r': // &r = reset
    return 0;

  default:
    return -1;
  }
}
