#include "cmd_help.h"
#include "../oc_ui.h"

int help_command(int argc, char **argv) {
  offensecom_ui_createmessage("", "&lCommand \t Result");
  offensecom_ui_createmessage("", "/help \t\t &bShow this menu");
  offensecom_ui_createmessage("", "/quit \t\t &bTerminate offensecom");

  return 0;
}
