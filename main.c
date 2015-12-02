/*
 * File:   main.c
 * Author: leandropessoa
 *
 * Created on 2 de Dezembro de 2015, 01:13
 */

 #include <stdio.h>
 #include <unistd.h>
 #include <sys/socket.h>
 #include <bluetooth/bluetooth.h>
 #include <bluetooth/rfcomm.h>

/*
 *
 */
int main(int argc, char** argv) {
  struct sockaddr_rc addr = { 0 };
  char buf[1024] = { 0 };
    int s, status, bytes_read;
    char dest[18] = "00:11:12:07:05:78";

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    // send a message
  //  if( status == 0 ) {
  //      status = write(s, "hello!", 6);
  //  }

    if( status < 0 ) perror("Deu Ruim!");


bytes_read = read(s, buf, sizeof(buf));
  while( bytes_read > 0 ) {
      printf("received [%s]\n", buf);
  }

    close(s);
    return 0;
}
