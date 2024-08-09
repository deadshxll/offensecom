#pragma once
#include <stdlib.h>

static const char client_name[256] = "OFFENSECOM CLIENT v1.1_dev";
static const int ui_refresh_rate = 75;

typedef struct chat_message chat_message;
struct chat_message {
  char username[64];
  char contents[2048];
  chat_message *previous;
};

static chat_message *latest_message = NULL;
static int oc_ui_running = 0;

void offensecom_ui_initialize();
void offensecom_ui_start();
void offensecom_ui_terminate();

void offensecom_ui_createmessage(char *username, char *message);
