/* Author: Are Oelsner
 * Networks Project 3
 * UDP Who Server
 */

#include "NetworkHeader.h"
#include "Packet.cpp"
#include "Database.cpp"

#define MAXPENDING 5 //Maximum outstanding connection requests

bool debug = false; // determines information output

/* function declarations */
int main (int argc, char *argv[]) {

  // Server variables
  int sock;                    // Socket descriptor for server
  struct sockaddr_in servAddr; // Local Address
  struct sockaddr_in clntAddr; // Client Address
  unsigned int clntLen;        // Length of client address data structure
  char* fileName;              // Database File name
  Packet p_rcv;                // Received Query Packet
  Packet p_rsp;                // Response Packet
  unsigned short serverPort = atoi(SERVER_PORT);   // Server port

  if (argc != 5) {
    printf("Error: Usage Project1Server -s <cookie> -p <port>\n");
    exit(1);
  }

  // Argument Parsing Variables
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
        case 'd':
          fileName = argv[i+1];
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

  /// Create a UDP socket
  if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    DieWithError((char*)"socket() failed");


  /// Construct Local Address Structure
  memset(&servAddr, 0, sizeof(servAddr));     // Zeros out structure
  servAddr.sin_family = AF_INET;                // Internet protocol 
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming Internet Address 32 bits
  servAddr.sin_port = htons(serverPort);        // Local Address port 16 bits


  /// Bind to the local address
  if(bind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    DieWithError((char*)"bind() failed");



  for(;;) {// while(true)
    clntLen = sizeof(clntAddr);       // Sets size of in-out parameter

    // Block until receive message from a client
    if(recvfrom(sock, &p_rcv, sizeof(Packet), 0, (struct sockaddr *)&clntAddr, &clntLen) < 0)
      DieWithError((char*)"recvfrom() failed");
    printf("////////////////////////////////////////////////////\n");
    printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));


    if(debug)
      printPacket(&p_rcv);

    // Check database and return relevant data

    char** data;
    int numEntries;
    open_database(fileName);
    data = lookup_user_names(p_rcv.data, &numEntries);
    close_database();
    if(data == nullPtr)  // invalid hostname - X defaults to 1
      p_rsp.X = 0x0;   
    else
      p_rsp.X = 0x1;   

    // Construct Response
    p_rsp.version  = 0x6;
    p_rsp.type     = 0x4;
    p_rsp.length   = numEntries;
    p_rsp.queryID  = 0xa37c;
    p_rsp.checksum = 0x0024;
    setData(&p_rsp, data, numEntries); 
    if(debug) {
      printf("Packet to send-\n");
      printPacket(&p_rsp);

      printf("Sending Message:\n");
    }
    // Send Response message
    if(sendto(sock, &p_rsp, sizeof(Packet), 0, (struct sockaddr *)&clntAddr, sizeof(clntAddr)) <= 0)
      DieWithError((char*)"sendto() sent a different number of bytes than expected");
    if(debug)
      printf("Message Sent\n");

  }
  close(sock);
  exit(0);
  printf("Exited\n");
}
