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

//parcer fornecido pela NeuroSky
#include "ThinkGearStreamParser.h"

/*
 *
 */
#define PIN 1234
//aqui que a magia acontece...
void
handleDataValueFunc( unsigned char extendedCodeLevel,
                     unsigned char code,
                     unsigned char valueLength,
                     const unsigned char *value,
                     void *customData ) {

    if( extendedCodeLevel == 0 ) {

        switch( code ) {

            /* [CODE]: ATTENTION eSense */
            case( 0x04 ):
                printf( "Attention Level: %d\n", value[0] & 0xFF );
                break;

            /* [CODE]: MEDITATION eSense */
            case( 0x05 ):
                printf( "Meditation Level: %d\n", value[0] & 0xFF );
                break;
             //xo colocar o detector de raw data aki pra gente
                case( 0x80 ):
                    printf( "RAW DATA: %d\n", (value[0]<<8) | value[1] );
                    int rawData = (value[0]<<8) | value[1] ;
                    FILE *fp;
fp=fopen("test.raw", "a+");
fputc(rawData, fp);
fclose(fp);
                    break;
              //agora a informação de qualidade de sinal
              case( 0x02 ):
                  printf( "POOR SIGNAL: %d\n", value[0] & 0xFF );
                  break;

              //nivel da bateria
              case( 0x01 ):
                  printf( "Battery: %d\n", value[0] & 0xFF );
                  break;
            /* Other [CODE]s */
            default:
                printf( "EXCODE level: %d CODE: 0x%02X vLength: %d\n",
                        extendedCodeLevel, code, valueLength );
                printf( "Data value(s):" );
                int i = 0;
                for(i=0; i<valueLength; i++ ) printf( " %02X", value[i] & 0xFF );
                printf( "\n" );
        }
    }
}



int main(int argc, char** argv) {
  struct sockaddr_rc addr = { 0 };
  //buffer... definir um tamanho adequado
unsigned char buf = 0 ;
    int s, status, bytes_read;
    //endereço mac do dispositivo bt
    char dest[18] = "10:14:07:01:11:67";

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
//verifica se dispositivo está acessivel
    if( status != 0 ) perror("Deu Ruim!");
     else
     {
const char command[2] = {0x00};
      write(s, &command, 1);


       ThinkGearStreamParser parser;
     THINKGEAR_initParser( &parser, PARSER_TYPE_PACKETS,
                          handleDataValueFunc, NULL );

         while( bytes_read = read(s, &buf, sizeof(buf)) ) {


               THINKGEAR_parseByte( &parser, buf);


         }





     }


    close(s);
    return 0;
}
