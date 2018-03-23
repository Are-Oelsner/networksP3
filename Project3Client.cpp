/* Author: Are Oelsner
 * Networks Project 3
 */

#include "NetworkHeader.h"
#include "Packet.cpp"

bool debug = false;

/* function declarations */

int main (int argc, char *argv[]) {

  // Argument parsing variables
  char *serverHost = (char *)SERVER_HOST;
  unsigned short serverPort = atoi(SERVER_PORT);

  int timeout;
  int maxRetries;
  char *hostname;


  char c;
  int i;

  if ((argc < 7) || (argc > 11)) {
    printf("Error: Usage ./Project3Client [-h <serverIP>] [-p <port>] -t <timeout> -i <max-retries> -d <hostname>\n");
    exit(1);
  }

  for (i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      c = argv[i][1];

      /* found an option, so look at next
       * argument to get the value of 
       * the option */
      switch (c) {
        case 't':                 // Timeout case in seconds
          timeout = atoi(argv[i+1]);
          break;
        case 'i':                 // Max-retries case
          maxRetries = atoi(argv[i+1]);
          break;
        case 'd':                 // Hostname case
          hostname = argv[i+1];
          break;
        case 'h':                 // Host case
          serverHost = argv[i+1];
          break;
        case 'p':                 // Port Case
          serverPort = atoi(argv[i+1]);
          break;
        default:
          break;
      }
    }
  }


  hostent* remoteHost;              // Constructs hostent
  char* hostName = serverHost;      // Rename 
  unsigned int addr;

  /* Networking code starts here */
  /// Variables
  int m_soc;                        // Socket id

  struct sockaddr_in destAddr;      // sockaddr_in sent
  struct sockaddr_in srcAddr;       // sockaddr_in received

  Packet p_query;                     // Outgoing Packet

  Packet p_rcv;                     // Incoming Packet

  unsigned int fromSize;            // Size of received packet
  int m_bytesReceived;              // Bytes received
  int m_totalBytesReceived;         // Tota bytes received



  /// Create a UDP socket
  if((m_soc = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    DieWithError((char*)"socket() failed");


  /// Construct Server Address Structure
  memset(&destAddr, 0, sizeof(destAddr));       // Zeros out structure
  destAddr.sin_family = AF_INET;                     // Internet protocol 
  destAddr.sin_port = htons(serverPort);             // Address port 16 bits

  // Handles both IP and name address input
  // www.cplusplus.com/forum/general/92837/
  if((remoteHost = gethostbyname(hostName)) != NULL) {  // If host is domain name
    destAddr.sin_addr.s_addr =  *((unsigned long *) remoteHost->h_addr_list[0]);
  }
  else { // Host address is address
    addr = inet_addr(hostName);   // converts format of address to binary
    remoteHost = gethostbyaddr((char *)&addr, 4, AF_INET);
    destAddr.sin_addr.s_addr = addr;  // Internet Address 32 bits
  }


  /// Communication with server
  // Creates Query message
  p_query.version  = 0x6;
  p_query.type     = 0x0;
  p_query.X        = 0x0;
  p_query.length   = 0x1;
  genQueryID(&p_query);
  p_query.checksum = 0x0024;
  strcat(p_query.data, hostname);

  if(debug) {
    printf("Sending Query\nPacket:");
    printPacket(&p_query);
  }

  // Send message
  int m_bytesSent = 0;
  int m_totalBytesSent = 0;
  while(m_bytesSent != (int)sizeof(p_query)) {
    if((m_bytesSent = sendto(m_soc, &p_query, sizeof(Packet), 0, (struct sockaddr *)&destAddr, sizeof(destAddr))) <= 0)
      DieWithError((char*)"sendto() sent a different number of bytes than expected");
    m_totalBytesSent += m_bytesSent;
    if(debug)
      printf("Sent %u bits of %lu\n", m_totalBytesSent, sizeof(p_query));
  }

  // Receive Message
  fromSize = sizeof(srcAddr);
  m_totalBytesReceived = 0;
  if(debug)
    printf("Receiving Message\n");
  while(m_bytesReceived < (int)sizeof(Packet)) {
    if((m_bytesReceived = recvfrom(m_soc, &p_rcv, sizeof(Packet), 0, (struct sockaddr *)&srcAddr, &fromSize)) <= 0)
      DieWithError((char*)"recv() failed or connection closed prematurely");
    m_totalBytesReceived += m_bytesReceived;
    if(debug)
      printf("Received %u bits of %lu\n", m_totalBytesReceived, sizeof(Packet));
  }

  if(debug) {
    printf("Packet Received:\n");
    printPacket(&p_rcv);
  }
  printData(&p_rcv);

  /// Close connection
  close(m_soc);

  exit(0);
  printf("Exited\n");
}



