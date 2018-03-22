/* Author: Are Oelsner
 * Networks Project 3
 */

#include "NetworkHeader.h"
#include "Packet.cpp"

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


  printf("Host: %s\tPort: %u\n", serverHost, serverPort);

  /* Networking code starts here */
  /// Variables
  int m_soc;                        // Socket id

  struct sockaddr_in destAddr;      // sockaddr_in sent
  struct sockaddr_in srcAddr;       // sockaddr_in received

  Packet p_query;                     // Outgoing Packet
  //char m_query[BUFFSIZE];             // Outgoing message
  char* m_query;             // Outgoing message

  Packet p_rcv;                     // Incoming Packet
  char m_rcv[BUFFSIZE];             // Incoming ACK message buffer

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
    printf("Name: %u\n", *remoteHost->h_addr_list[0]);
  }
  else { // Host address is address
    addr = inet_addr(hostName);   // converts format of address to binary
    remoteHost = gethostbyaddr((char *)&addr, 4, AF_INET);
    destAddr.sin_addr.s_addr = addr;  // Internet Address 32 bits
    printf("Address: %s, \t addr: %u\n", hostName, addr);
  }


  /// Communication with server
  // Creates Query message
  p_query.setVersion((char*)"0110");
  p_query.setType((char*)"000");
  p_query.setX((char*)"0");
  p_query.setQueryID((char*)"8675309188843228");
  p_query.setData(hostname); 
  p_query.computeChecksum();

  m_query = p_query.constructMSG();



  //printf("Query message: %s, %u\n", m_query, strlen(m_query));
  printf("Query message: %s\t Length:  %lu\n", m_query, strlen(m_query));


  // Send message
  int m_bytesSent = 0;
  int m_totalBytesSent = 0;
  while(m_bytesSent != (int)strlen(m_query)) {
    if((m_bytesSent = sendto(m_soc, m_query, (unsigned int)strlen(m_query), 0, (struct sockaddr *)&destAddr, sizeof(destAddr))) <= 0)
      DieWithError((char*)"sendto() sent a different number of bytes than expected");
    m_totalBytesSent += m_bytesSent;
  }
  printf("Sent %u bits\n", m_totalBytesSent);

  //change to while((m_bytesReceived = recv(m_soc, m_rcv, BUFFSIZE-1, 0)) <= 0)
  // Receive Message
  fromSize = sizeof(srcAddr);
  m_totalBytesReceived = 0;
  printf("ServerMessage: \t");
  while(strchr(m_rcv, '\n') == NULL) {
    printf("Receiving Message\n");
    if((m_bytesReceived = recvfrom(m_soc, m_rcv, BUFFSIZE-1, 0, (struct sockaddr *)&srcAddr, &fromSize)) <= 0)
      DieWithError((char*)"recv() failed or connection closed prematurely");
    printf("Receiving Message - After If\n");
    m_totalBytesReceived += m_bytesReceived;
    m_rcv[m_bytesReceived] = '\0'; 
    printf("%s", m_rcv);
  }
  printf("\n");
  if(destAddr.sin_addr.s_addr != srcAddr.sin_addr.s_addr) {
    DieWithError((char*)"Error: received a packet from unknown source.\n");
  }

  // Parse m_rcv into Packet object
  p_rcv.parse(m_rcv);


//// Creates BYE message
//m_type = strtok(m_rcv, " ");
//char * m_cookie = strtok(NULL, " ");
//strcpy(m_bye, m_vers); 
//strcat(m_bye, "BYE "); 
//strcat(m_bye, m_cookie);
//strcat(m_bye, "\n");
////printf("Bye message: %s/n", m_bye);


  /// Close connection
  close(m_soc);

  exit(0);
  printf("Exited\n");
}



