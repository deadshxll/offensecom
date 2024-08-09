#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "net/oc_socket.h"
#include "offensecom.h"
#include "ui/oc_ui.h"

void *start_connection(void *vargsp) {
  offensecom_socket_connect("127.0.0.1", 8080);
  return NULL;
}

int main() {

  pthread_t ui_thread;
  pthread_create(&ui_thread, NULL, start_connection, NULL);
  pthread_detach(ui_thread);

  offensecom_ui_initialize();
  offensecom_ui_start();

  return 0;
}
