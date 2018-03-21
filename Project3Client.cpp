/* Author: Are Oelsner
 * Networks Project 2
 */

#include "NetworkHeader.h"

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
  struct sockaddr_in m_sockaddr_in; // sockaddr_in
  char m_msg[BUFFSIZE];             // Outgoing HELLO message
  char m_rcv[BUFFSIZE];             // Incoming ACK message buffer
  char m_bye[BUFFSIZE];             // Outgoing BYE message
  int m_bytesReceived;              // Bytes received
  int m_totalBytesReceived;         // Tota bytes received


  /// Create a TCP socket
  if((m_soc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithError((char*)"socket() failed");


  /// Construct Server Address Structure
  memset(&m_sockaddr_in, 0, sizeof(m_sockaddr_in));       // Zeros out structure
  m_sockaddr_in.sin_family = AF_INET;                     // Internet protocol 
  m_sockaddr_in.sin_port = htons(serverPort);             // Address port 16 bits

  // Handles both IP and name address input
  // www.cplusplus.com/forum/general/92837/
  if((remoteHost = gethostbyname(hostName)) != NULL) {  // If host is domain name
    m_sockaddr_in.sin_addr.s_addr =  *((unsigned long *) remoteHost->h_addr_list[0]);
    printf("Name: %u\n", *remoteHost->h_addr_list[0]);
  //printf("Name: %s, \t addr: %u\n", remoteHost->h_addr, address);
  }
  else { // Host address is address
    addr = inet_addr(hostName);   // converts format of address to binary
    remoteHost = gethostbyaddr((char *)&addr, 4, AF_INET);
    m_sockaddr_in.sin_addr.s_addr = addr;  // Internet Address 32 bits
    printf("Address: %s, \t addr: %u\n", hostName, addr);
  }


  /// Establish a connection to the server
  // int connect(int socket, struct sockaddr *foreignAddress, unsigned int addressLength)
  if(connect(m_soc, (struct sockaddr *)&m_sockaddr_in, sizeof(m_sockaddr_in)) < 0)
    DieWithError((char*)"connect() failed");


  /// Communication with server
  // Creates HELLO message
  char * m_vers = (char*)"CS332 ";
  char * m_type = (char*)"HELLO ";
  strcpy(m_msg, m_vers);            // Sets version field
  strcat(m_msg, m_type);            // Sets type field
  strcat(m_msg, firstName);         // Sets first name field
  strcat(m_msg, " ");
  strcat(m_msg, lastName);          // Sets last name field
  strcat(m_msg, "\n");
  //printf("Hello message: %s/n", m_msg);


  // Send message
  if(send(m_soc, m_msg, strlen(m_msg), 0) != (unsigned int)strlen(m_msg))
    DieWithError((char*)"send() sent a different number of bytes than expected");

  //change to while((m_bytesReceived = recv(m_soc, m_rcv, BUFFSIZE-1, 0)) <= 0)
  // Receive Message
  m_totalBytesReceived = 0;
  printf("ServerMessage: ");
  while(strchr(m_rcv, '\n') == NULL) {
    if((m_bytesReceived = recv(m_soc, m_rcv, BUFFSIZE-1, 0)) <= 0)
      DieWithError((char*)"recv() failed or connection closed prematurely");
    m_totalBytesReceived += m_bytesReceived;
    m_rcv[m_bytesReceived] = '\0'; 
    printf("%s", m_rcv);
  }
  printf("\n");

  // Creates BYE message
  m_type = strtok(m_rcv, " ");
  char * m_cookie = strtok(NULL, " ");
  strcpy(m_bye, m_vers); 
  strcat(m_bye, "BYE "); 
  strcat(m_bye, m_cookie);
  strcat(m_bye, "\n");
  //printf("Bye message: %s/n", m_bye);

  // Send BYE message
  if(send(m_soc, m_bye, strlen(m_bye), 0) != (unsigned int)strlen(m_bye))
    DieWithError((char*)"send() bye sent a different number of bytes than expected");


  /// Close connection
  close(m_soc);

  exit(0);
  printf("Exited\n");
}



