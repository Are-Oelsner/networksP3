/* Author: Are Oelsner
 * Networks Project 3
 */

#include "NetworkHeader.h"
#include "Packet.cpp"

bool debug = false;

void CatchAlarm(int ignored);          /*Handler for SIGALRM     */

int tries = 0;

int main (int argc, char *argv[]) {

  char *serverHost = (char *)SERVER_HOST;
  unsigned short serverPort = atoi(SERVER_PORT);

  int timeout;
  int maxRetries;
  char *hostname;

  // Argument parsing variables
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


  /* Networking code starts here */
  /// Variables
  hostent* remoteHost;              // Constructs hostent
  char* hostName = serverHost;      // Rename 
  unsigned int addr;
  int m_soc;                        // Socket id
  struct sockaddr_in destAddr;      // sockaddr_in sent
  struct sockaddr_in srcAddr;       // sockaddr_in received
  Packet p_query;                   // Outgoing Packet
  Packet p_rcv;                     // Incoming Packet
  unsigned int fromSize;            // Size of received packet
  struct sigaction myAction;        // For setting signal handler

  /// Create a UDP socket
  if((m_soc = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    DieWithError((char*)"socket() failed");

  // Set signal handler for alarm signal
  myAction.sa_handler = CatchAlarm;

  if(sigfillset(&myAction.sa_mask) < 0) // block everything in handler
    DieWithError((char*)"sigfillset() failed");
  myAction.sa_flags = 0;

  if(sigaction(SIGALRM, &myAction, 0) < 0)
    DieWithError((char*)"sigaction() failed for SIGALRM");


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
  strcat(p_query.data, hostname);
  p_query.checksum = 0x0000;
  //p_query.checksum = checksum(&p_query);
  p_query.checksum = Checksum((void*)&p_query, sizeof(p_query));

  if(debug) {
    printf("Sending Query\nPacket:");
    printPacket(&p_query);
  }

retransmit: // Goto label for retransmit after timeout

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
  alarm(timeout);
  while(recvfrom(m_soc, &p_rcv, sizeof(Packet), 0, (struct sockaddr *)&srcAddr, &fromSize) < 0 
      || p_rcv.queryID != p_query.queryID || p_rcv.version != p_query.version || p_rcv.type != 4 
      || srcAddr.sin_addr.s_addr != destAddr.sin_addr.s_addr || Checksum((void*)&p_rcv, sizeof(p_rcv)) != 0)
    if(errno == EINTR) { // Alarm went off 
      if(tries < maxRetries) {
        printf("timed out, %d more tries...\n", maxRetries-tries);
        if(sendto(m_soc, &p_query, sizeof(Packet), 0, (struct sockaddr *)&destAddr, sizeof(destAddr)) != sizeof(Packet))
          DieWithError((char*)"sendto() sent a different number of bytes than expected");
        alarm(timeout);
      }else
        DieWithError((char*)"No Response");
    }else
      DieWithError((char*)"recvfrom() failed");
  // If recvfrom() got something cancel timeout
  alarm(0);

  if(debug)
    printf("Response Received:\n");

  if(debug)
    printPacket(&p_rcv);


  if(p_rcv.length > 0)
    printData(&p_rcv);
  else if(p_rcv.X == 1)
    printf("No users at hostname %s\n", hostname);

  /// Close connection
  close(m_soc);

  exit(0);
  printf("Exited\n");
}

void CatchAlarm(int ignored) {
  tries += 1;
}


