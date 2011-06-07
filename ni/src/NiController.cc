#include "NiController.hh"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

unsigned char start[5] = "star";
unsigned char stop[5] = "stop";


NiController::NiController() {
	//NI_IP = "192.76.172.199";
}
void NiController::Configure(const eudaq::Configuration & param) {

}
void NiController::TagsSetting(){
	//ev.SetTag("DET", 12);
}
void NiController::GetProduserHostInfo(){
	/*** get Producer information, NAME and INET ADDRESS ***/
	gethostname(ThisHost, MAXHOSTNAME);
	printf("----TCP/Producer running at host NAME: %s\n", ThisHost);
	hclient = gethostbyname(ThisHost);
	bcopy ( hclient->h_addr, &(client.sin_addr), hclient->h_length);
	printf("----TCP/Producer INET ADDRESS is: %s \n", inet_ntoa(client.sin_addr));
}
void NiController::Start(){
	ConfigClientSocket_Send(start, sizeof(start) );
}
void NiController::Stop(){
	ConfigClientSocket_Send(stop, sizeof(stop) );
}
void NiController::ConfigClientSocket_Open(){
	/*** Network configuration for NI, NAME and INET ADDRESS ***/
	data = inet_addr("192.76.172.199");
	hconfig = gethostbyaddr(&data, 4, AF_INET);
	if ( hconfig == NULL) {
		EUDAQ_ERROR("ConfSocket: get HOST error  " );
		perror("gethostbyname()");
		exit(1);
	} else{
		bcopy ( hconfig->h_addr, &(config.sin_addr), hconfig->h_length);
		printf("----TCP/NI crate INET ADDRESS is: %s \n", inet_ntoa(config.sin_addr));
		printf("----TCP/NI crate INET PORT is: %d \n", PORT_CONFIG );
	}
	if ((sock_config = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		EUDAQ_ERROR("ConfSocket: Create socket error  " );
		perror("socket()");
		exit(1);
	} else
		printf("----TCP/NI crate: The SOCKET is OK...\n");

	config.sin_family = AF_INET;
	config.sin_port = htons(PORT_CONFIG);
	memset(&(config.sin_zero), '\0', 8);
	if (connect(sock_config, (struct sockaddr *) &config, sizeof(struct sockaddr)) == -1) {
		EUDAQ_ERROR("ConfSocket: National Instruments crate doesn't running  " );
		perror("connect()");
		sleep(5);
		exit(1);
	} else
		printf("----TCP/NI crate: The CONNECT is OK...\n");
}
void NiController::DatatransportClientSocket_Open(){
	/*** Creation for the data transmit socket, NAME and INET ADDRESS ***/
	data_trans_addres = inet_addr("192.76.172.199");
	hdatatransport = gethostbyaddr(&data_trans_addres, 4, AF_INET);
	if ( hdatatransport == NULL) {
		EUDAQ_ERROR("DataTransportSocket: get HOST error " );
		perror("gethostbyname()");
		exit(1);
	} else{
		bcopy ( hdatatransport->h_addr, &(datatransport.sin_addr), hdatatransport->h_length);
		printf("----TCP/NI crate DATA TRANSPORT INET ADDRESS is: %s \n", inet_ntoa(datatransport.sin_addr));
		printf("----TCP/NI crate DATA TRANSPORT INET PORT is: %d \n", PORT_DATATRANSF );
	}
	if ((sock_datatransport = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		EUDAQ_ERROR("DataTransportSocket: create socket error " );
		perror("socket()");
		exit(1);
	} else
		printf("----TCP/NI crate DATA TRANSPORT: The SOCKET is OK...\n");
	//sleep (5);
	datatransport.sin_family = AF_INET;
	datatransport.sin_port = htons(PORT_DATATRANSF);
	memset(&(datatransport.sin_zero), '\0', 8);
	if (connect(sock_datatransport, (struct sockaddr *) &datatransport, sizeof(struct sockaddr)) == -1) {
		EUDAQ_ERROR("DataTransportSocket: National Instruments crate doesn't running  " );
		perror("connect()");
		exit(1);
	} else
		printf("----TCP/NI crate DATA TRANSPORT: The CONNECT is OK...\n");
}
unsigned int NiController::DataTransportClientSocket_ReadLength(const char string[4]) {
	unsigned int datalengthTmp;
	unsigned int datalength;
	if ((numbytes = recv(sock_datatransport, Buffer_length, 2, 0)) == -1) {
		EUDAQ_ERROR("DataTransportSocket: Read length error " );
		perror("recv()");
		exit(1);
	}
	datalengthTmp = 0;
	datalengthTmp = 0xFF & Buffer_length[0];
	datalengthTmp <<= 8;
	datalengthTmp += 0xFF & Buffer_length[1];
	datalength = datalengthTmp;

	return datalength;
}
char *NiController::DataTransportClientSocket_ReadData(int datalength) {
	if ((numbytes = recv(sock_datatransport, Buffer_data, datalength, 0)) == -1) {
		EUDAQ_ERROR("DataTransportSocket: Read data error " );
		perror("recv()");
		exit(1);
	}
	return Buffer_data;
}
void NiController::ConfigClientSocket_Send(unsigned char *text, size_t len){
	printf("size=%d ", len);
	if (send(sock_config, text, len, 0) == -1) 	perror("Server-send() error lol!");
}
void NiController::ConfigClientSocket_Close(){
	close(sock_config);
}
void NiController::DatatransportClientSocket_Close(){
	close(sock_datatransport);
}
NiController::~NiController() {
	//
}
