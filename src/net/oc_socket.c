
// https://www.geeksforgeeks.org/socket-programming-cc/

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../offensecom.h"
#include "../ui/oc_ui.h"
#include "oc_socket.h"

/*

This is an example use case for the offensecom client.

All this does is connect to a socket & recieves messages and displays it in the
offensecom ui.

At line 111 in ui/oc_ui.c, you will find that when you send a message through
the offensecom client, it will call the `offensecom_socket_send` to send a
message if the socket is connected.

*/

int status, valread, client_fd, oc_socket_connected;
void offensecom_socket_connect(char ip[], int port) {
  struct sockaddr_in serv_addr;
  char buffer[4096] = {0};
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  // Convert IPv4 and IPv6 addresses from text to binary
  // form
  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
    return;
  }

  if ((status = connect(client_fd, (struct sockaddr *)&serv_addr,
                        sizeof(serv_addr))) < 0) {
    return;
  }

  oc_socket_connected = 1;
  char tmp[256];
  sprintf(tmp, "&2Connected to %s:%d", ip, port);
  offensecom_ui_createmessage("", tmp);

  while (1) {
    memset(buffer, 0, sizeof(buffer));
    valread = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (valread == 0)
      break;

    int msglen = strlen(buffer);
    if (buffer[msglen - 1] == '\n') {
      buffer[msglen - 1] = '\0';
    }

    // If we recieve a message, then create a message with the username "SERVER"
    // In offensecom, if there is no username, then the message would be
    // displayed without the username tags (<username>), making it kind of like
    // a broadcast / message.
    if (msglen > 1)
      offensecom_ui_createmessage("SERVER", buffer);
  }

  oc_socket_connected = 0;
  offensecom_ui_createmessage("", "&4Disconnected.");

  // closing the connected socket
  close(client_fd);
}

void offensecom_socket_send(char *message) {
  send(client_fd, message, strlen(message), 0);
}

int offensecom_socket_is_connected() { return oc_socket_connected; }
