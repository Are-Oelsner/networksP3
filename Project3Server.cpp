/* Author: Are Oelsner
 * Networks Project 3
 * UDP Who Server
 */

#include "NetworkHeader.h"

#define MAXPENDING 5 //Maximum outstanding connection requests

/* function declarations */
// TCP Client handling function
void HandleTCPClient(int clntSocket); 

char *cookie; //TODO rework this

int main (int argc, char *argv[]) {

  // Argument parsing variables
  int servSock;                    // Socket descriptor for server
  int clntSock;                    // Socket descriptor for client
  struct sockaddr_in servAddr; // Local Address
  struct sockaddr_in clntAddr; // Client Address
  unsigned short serverPort = atoi(SERVER_PORT);     // Server port
  unsigned int clntLen;            // Length of client address data structure

  if (argc != 5) {
    printf("Error: Usage Project1Server -s <cookie> -p <port>\n");
    exit(1);
  }

  char c;
  int i;

  // Parses input arguments
  for (i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      c = argv[i][1];

      /* found an option, so look at next
       * argument to get the value of 
       * the option */
      switch (c) {
        case 's':
          cookie = argv[i+1];
          break;
        case 'p':
          serverPort =atoi(argv[i+1]);
          break;
        default:
          break;
      }
    }
  }

  /* Networking code starts here */

  /// Create a TCP socket
  if((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithError((char*)"socket() failed");


  /// Construct Local Address Structure
  memset(&servAddr, 0, sizeof(servAddr));     // Zeros out structure
  servAddr.sin_family = AF_INET;                // Internet protocol 
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming Internet Address 32 bits
  servAddr.sin_port = htons(serverPort);        // Local Address port 16 bits


  /// Bind to the local address
  if(bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    DieWithError((char*)"bind() failed");

  /// Mark the socket so it listens for incoming connections
  if(listen(servSock, MAXPENDING) < 0)
    DieWithError((char*)"listen() failed");


  for(;;) {//TODO while(true)
    clntLen = sizeof(clntAddr);       // Sets size of in-out parameter

    // Wait for a client to connect
    if((clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen)) < 0)
      DieWithError((char*)"accept() failed");

    printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));

    HandleTCPClient(clntSock);
  }
  exit(0);
  printf("Exited\n");
}


void HandleTCPClient(int clntSocket) {

  /// Variables
  char m_rcv[BUFFSIZE];             // Incoming HELLO message buffer
  char m_bye[BUFFSIZE];             // Incoming BYE message buffer
  char m_msg[BUFFSIZE];             // Outgoing ACK message
  int rcvMsgSize;                   // Bytes received
  int m_totalBytesReceived;         // Tota bytes received TODO delete


  //if((rcvMsgSize = recv(clntSocket, m_rcv, BUFFSIZE, 0)) < 0) //Buffsize -1? <= 0?
  //  DieWithError((char*)"recv() failed or connection closed prematurely");

  //while(rcvMsgSize > 0) {

  /// Receives Hello Message from client
  m_totalBytesReceived = 0;
  printf("Client HELLO Message: ");
  while(strchr(m_rcv, '\n') == NULL) {
    if((rcvMsgSize = recv(clntSocket, m_rcv, BUFFSIZE-1, 0)) <= 0)
      DieWithError((char*)"recv() failed or connection closed prematurely");
    m_totalBytesReceived += rcvMsgSize;
    m_rcv[rcvMsgSize] = '\0'; 
    printf("%s", m_rcv);
  }
  printf("\n");

  // Parse Received HELLO message
  char * m_vers = (char*)"CS332 "; //TODO
  char * m_type = strtok(m_rcv, " ");
  char * firstname = strtok(NULL, " ");
  char * lastname = strtok(NULL, " ");

  // Construct ACK message
  strcpy(m_msg, m_vers); 
  strcat(m_msg, "ACK "); 
  strcat(m_msg, cookie);
  strcat(m_msg, "\n");


  // Send ACK message
  if(send(clntSocket, m_msg, strlen(m_msg), 0) != (unsigned int)strlen(m_msg))
    DieWithError((char*)"send() sent a different number of bytes than expected");


  /// Receives BYE Message from client
  m_totalBytesReceived = 0;
  printf("Client BYE Message: ");
  while(strchr(m_bye, '\n') == NULL) {
    if((rcvMsgSize = recv(clntSocket, m_bye, BUFFSIZE-1, 0)) <= 0)
      DieWithError((char*)"recv() failed or connection closed prematurely");
    m_totalBytesReceived += rcvMsgSize;
    m_bye[rcvMsgSize] = '\0'; 
    printf("%s", m_bye);
  }
  printf("\n");


  // Parse Received BYE message
  m_vers = (char*)"CS332 "; //TODO
  m_type = strtok(m_bye, " ");
  char * m_cookie = strtok(NULL, " ");


  /// Close connection
  close(clntSocket);

}



