/* Author: Are Oelsner
 * Networks Project 3
 * UDP Who Server
 */

#include "NetworkHeader.h"
#include "Packet.h"
#include "Database.cpp"

#define MAXPENDING 5 //Maximum outstanding connection requests

/* function declarations */
// UDP Client handling function
void HandleUDPClient(int sock, struct sockaddr_in clntAddr, unsigned int cliAddrLen, Packet p_rcv); 


int main (int argc, char *argv[]) {

  // Server variables
  int sock;                    // Socket descriptor for server
  struct sockaddr_in servAddr; // Local Address
  struct sockaddr_in clntAddr; // Client Address
  unsigned int cliAddrLen;     // Length of incoming message
  unsigned int clntLen;        // Length of client address data structure
  char* fileName;              // Database File name
  char rcvBuffer[BUFFSIZE];    // Buffer for received queries
  int recvMsgSize;             // Size of received query
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
  open_database(fileName);


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


  //int rcvMsgSize;                   // Bytes received
  //int totalBytesReceived;         // Tota bytes received 
  Packet p_rcv;

  for(;;) {// while(true)
    clntLen = sizeof(clntAddr);       // Sets size of in-out parameter

    // Block until receive message from a client
    //totalBytesReceived = 0;
    //printf("ServerMessage: \t");
    //while(strchr(rcvBuffer, '\n') == NULL) {
    //if((recvMsgSize = recvfrom(sock, rcvBuffer, BUFFSIZE, 0, (struct sockaddr *)&clntAddr, &cliAddrLen)) < 0)
    if((recvMsgSize = recvfrom(sock, &p_rcv, sizeof(Packet), 0, (struct sockaddr *)&clntAddr, &cliAddrLen)) < 0)
      DieWithError((char*)"recvfrom() failed");
  //if(clntAddr.sin_addr.s_addr != servAddr.sin_addr.s_addr) //TODO do I need this?
  //  DieWithError((char*)"Error: received a packet from unknown source.\n");
    //totalBytesReceived += rcvMsgSize;
    //rcvBuffer[totalBytesReceived] = '\0'; 
    printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));
    //}
    //HandleUDPClient(sock, clntAddr, cliAddrLen, rcvBuffer);
    HandleUDPClient(sock, clntAddr, cliAddrLen, p_rcv);


  }
  close(sock);
  close_database();
  exit(0);
  printf("Exited\n");
}


void HandleUDPClient(int sock, struct sockaddr_in clntAddr, unsigned int cliAddrLen, Packet p_rcv) {

  /// Variables
  Packet p_msg;                     // Response Packet
  char* m_msg;             // Outgoing Response message
  char** data;
  int numEntries;

  // Construct Packet from received Query
  p_rcv.printPacket();

  // Check database and return relevant data
  data = lookup_user_names(p_rcv.getQData(), &numEntries);
  if(data == nullPtr)  // invalid hostname - X defaults to 1
    p_msg.setX((char*)"0");   


  // Construct Response
  p_msg.setVersion((char*)"0110");
  p_msg.setType((char*)"100"); 
  p_msg.setQueryID((char*)"8675309188843228"); // TODO convert to random gen
  p_msg.setData(data, numEntries); 
  //p_msg.computeChecksum();

  m_msg = p_msg.constructMSG();

  printf("Sending Message: %s\n", p_msg.constructMSG());
  // Send Response message
  int bytesSent = 0;
  int totalBytesSent = 0;
  while(totalBytesSent < sizeof(Packet)) {
    if((bytesSent = sendto(sock, &p_msg, sizeof(Packet), 0, (struct sockaddr *)&clntAddr, sizeof(clntAddr))) <= 0)
      DieWithError((char*)"sendto() sent a different number of bytes than expected");
    totalBytesSent += bytesSent;
    printf("Sent %u bits of %lu\n", totalBytesSent, sizeof(Packet));
  }
}

