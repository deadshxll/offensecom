#include <string.h>

#include "../oc_helper.h"
#include "commands/cmd_help.h"
#include "commands/cmd_quit.h"
#include "oc_cmd_manager.h"
#include "oc_ui.h"

void oc_cmd_manager_handle_command(char *command) {
  // Formatting the command to a argc/argv type format
  if (strlen(command) < 1)
    return;

  int argc = -1;
  char **argv = split_string(command, ' ', &argc);

  // Removing the first character of the command (which should be the '/')
  memmove(argv[0], argv[0] + 1, strlen(argv[0]));

  if (strcmp(argv[0], "help") == 0) {
    help_command(argc, argv);
  } else if (strcmp(argv[0], "quit") == 0) {
    quit_command(argc, argv);
  } else {
    offensecom_ui_createmessage("", "&bUnkown command, Try '/help'.");
  }
}
