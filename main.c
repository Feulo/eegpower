/*
 * File:   main.c
 * Author: leandropessoa
 * Contributor: Feulo
 * Created on 2 de Dezembro de 2015, 01:13
 */

 #include <stdio.h>
 #include <unistd.h>
 #include <sys/socket.h>
 #include <bluetooth/bluetooth.h>
 #include <bluetooth/rfcomm.h>
 #include "ThinkGearStreamParser.h"	//parser fornecido pela NeuroSky

 #include <bluetooth/hci.h>
 #include <bluetooth/hci_lib.h>

 #include <jack/jack.h>
 #include <jack/ringbuffer.h>
 #include <jack/types.h>




 #define PIN 1234

 typedef struct {
	jack_client_t * client;
	jack_status_t status;
	jack_port_t ** jack_ports;
	float sample_rate;
	//sine_data data;
} eeg_signal;


//aqui que a magia acontece...
void handleDataValueFunc( unsigned char extendedCodeLevel,
	       		  unsigned char code,
			  unsigned char valueLength,
			  const unsigned char *value,
			  void *customData ) {
	int rawData,i;
	FILE *fp;

    	if( extendedCodeLevel == 0 ) {

        	switch( code ) {
                       	
			case( 0x04 ):	 /* [CODE]: ATTENTION eSense */
                		printf( "Attention Level: %d\n", value[0] & 0xFF );
                	break;

                       	case( 0x05 ): 	/* [CODE]: MEDITATION eSense */
                		printf( "Meditation Level: %d\n", value[0] & 0xFF );
                	break;
            
       			case( 0x80 ):	//xo colocar o detector de raw data aki pra gente
                    		printf( "RAW DATA: %d\n", (value[0]<<8) | value[1] );
                     		rawData = (value[0]<<8) | value[1] ;    
				fp=fopen("test.raw", "a+");
				fputc(rawData, fp);
				fclose(fp);
                    	break;
                          
			case( 0x02 ): //agora a informação de qualidade de sinal
                  		printf( "POOR SIGNAL: %d\n", value[0] & 0xFF );
                  	break;

                       	case( 0x01 ): 
                  		printf( "Battery: %d\n", value[0] & 0xFF );
                  	break;
            
            		default:	/* Other [CODE]s */
                		printf( "EXCODE level: %d CODE: 0x%02X vLength: %d\n", extendedCodeLevel, code, valueLength );
                		printf( "Data value(s):" );
                		i = 0;
                		for(i=0; i<valueLength; i++ ) printf( " %02X", value[i] & 0xFF );
                		printf( "\n" );
        	}
    	}
}



int main(int argc, char** argv) {

	struct sockaddr_rc addr = { 0 };	 //buffer... definir um tamanho adequado
	int s, status, bytes_read;		 
	char dest[18];// = "10:14:07:01:11:67";	//endereço mac do dispositivo bt
	const char command[2] = {0x00};
	int dev_id,sock,len,flags;
	int max_rsp, num_rsp;
	int i,pos;
	inquiry_info *ii = NULL;
	char name[248] = { 0 };
	char addr_1[19] = { 0 };
	unsigned char buf = 0 ;
	char **addrs;	
	eeg_signal * signal  = (eeg_signal *) malloc(sizeof(eeg_signal *));


	dev_id = hci_get_route(NULL);          //comunicacao com placa bluetooth
    	sock = hci_open_dev( dev_id );
	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
	        exit(1);
	}

        len  = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if( num_rsp < 0 ) perror("hci_inquiry");
	
	addrs = (char**)malloc(num_rsp*sizeof(char*));

	for (i = 0; i < num_rsp; i++) {
		ba2str(&(ii+i)->bdaddr, addr_1);
		memset(name, 0, sizeof(name));
		if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0)
		        strcpy(name, "[unknown]");
		addrs[i] = (char*)malloc(19*sizeof(char));
		memcpy(addrs[i], addr_1, 19);
		printf("%d) %s  %s\n", i,  addr_1, name);

 	}


	printf("Select device: ");
	scanf("%d",&pos);
	if( pos >= num_rsp || pos < 0){
		printf("not available\n");
		return 0;
	}

	printf("%s selected\n",addrs[pos]);
	strcpy(dest,addrs[pos]);
	free( ii );
	for(i = 0; i < num_rsp; i++)
		free(addrs[i]);
        free(addrs);
        close( sock );

	// allocate a socket
    	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    	// set the connection parameters (who to connect to)
    	addr.rc_family = AF_BLUETOOTH;
    	addr.rc_channel = (uint8_t) 1;
    	str2ba( dest, &addr.rc_bdaddr );

    	// connect to server
    	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

        if( status != 0 ){
	      	perror("eu Ruim");
    		exit(1);
	}
	
		write(s, &command, 1);
       		ThinkGearStreamParser parser;
     		THINKGEAR_initParser( &parser, PARSER_TYPE_PACKETS,handleDataValueFunc, NULL );
        
	//cria um cliente do jack

	if((signal->client = jack_client_open("Mindflex", JackNullOption, &signal->status)) == 0)
		printf("Problemas criando cliente\n");
	
	else	printf("Jack Client Created\n");
	
	//armazena o sample rate do jack num float
	signal->sample_rate = jack_get_sample_rate(signal->client);
	//acho q cria o buffer do q eh tocado
	signal->jack_ports = (jack_port_t **) malloc(1 * sizeof (jack_port_t *));
	// registra o output
	signal->jack_ports[0] = jack_port_register(signal->client, "OUTPUT" , JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	// sta a funçao de callback ainda preciso entender
	jack_set_process_callback(signal->client, oscillator_processCallback, signal);
	// ativa o cliente
	jack_activate(oscillator->client);



		
				
		
		
	//	while( bytes_read = read(s, &buf, sizeof(buf)) ) {
          //     		THINKGEAR_parseByte( &parser, buf);
         //	}
     	

    	close(s);
    	return 0;
}
