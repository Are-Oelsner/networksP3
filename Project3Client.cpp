/* Author: Are Oelsner
 * Networks Project 3
 */

#include "NetworkHeader.h"
#include "Packet.cpp"

/* function declarations */

int main (int argc, char *argv[]) {

  // Argument parsing variables
  char *firstName;
  char *lastName;
  char *serverHost = (char *)SERVER_HOST;
  unsigned short serverPort = atoi(SERVER_PORT);
  char *servPortString;

  char c;
  int i;

  if ((argc < 5) || (argc > 7)) {
    printf("Error: Usage Project0Client [-s <server IP>[:<port>]] -f <firstName> -l <lastName>\n");
    exit(1);
  }

  for (i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      c = argv[i][1];

      /* found an option, so look at next
       * argument to get the value of 
       * the option */
      switch (c) {
        case 'f':                 // First name case
          firstName = argv[i+1];
          break;
        case 'l':                 // Last name case
          lastName = argv[i+1];
          break;
        case 's':                 // Host and Port case
          serverHost = strtok(argv[i+1],":");
          if ((servPortString = strtok(NULL, ":")) != NULL) {
            serverPort = atoi(servPortString);
          }
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

  struct sockaddr_in srcAddr;       // sockaddr_in received
  struct sockaddr_in destAddr;      // sockaddr_in sent

  Packet p_query;                     // Outgoing Packet
  char m_query[BUFFSIZE];             // Outgoing message

  Packet p_rcv;                     // Incoming Packet
  char m_rcv[BUFFSIZE];             // Incoming ACK message buffer

  unsigned int fromSize;            // Size of received packet
  int m_bytesReceived;              // Bytes received
  int m_totalBytesReceived;         // Tota bytes received



  /// Create a TCP socket
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
  char * m_vers = (char*)"CS332 ";
  char * m_type = (char*)"HELLO ";
  strcpy(m_query, m_vers);            // Sets version field
  strcat(m_query, m_type);            // Sets type field
  strcat(m_query, firstName);         // Sets first name field
  strcat(m_query, " ");
  strcat(m_query, lastName);          // Sets last name field
  strcat(m_query, "\n");
  //printf("Query message: %s/n", m_query);


  // Send message
  if(sendto(m_soc, m_query, strlen(m_query), 0, (struct sockaddr *)&destAddr, sizeof(destAddr)) != (unsigned int)strlen(m_query))
    DieWithError((char*)"sendto() sent a different number of bytes than expected");

  //change to while((m_bytesReceived = recv(m_soc, m_rcv, BUFFSIZE-1, 0)) <= 0)
  // Receive Message
  fromSize = sizeof(srcAddr);
  m_totalBytesReceived = 0;
  printf("ServerMessage: ");
  while(strchr(m_rcv, '\n') == NULL) {
    if((m_bytesReceived = recvfrom(m_soc, m_rcv, BUFFSIZE-1, 0, (struct sockaddr *)&srcAddr, &fromSize)) <= 0)
      DieWithError((char*)"recv() failed or connection closed prematurely");
    m_totalBytesReceived += m_bytesReceived;
    m_rcv[m_bytesReceived] = '\0'; 
    printf("%s", m_rcv);
  }
  printf("\n");
  if(destAddr.sin_addr.s_addr != srcAddr.sin_addr.s_addr) {
    DieWithError((char*)"Error: received a packet from unknown source.\n");
  }

  // Parse m_rcv into Packet object
  p_rcv.parse(*m_rcv);


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



