#include "Packet.h"

Packet::
Packet() {
  version = (char*)"0110";
  type = (char*)"000";
  X = (char*)"0";
  length = (char*)"00000001";
  queryID = (char*)"8675309188843228";
  checksum =(char*)"1111111111111111";//TODO
  q_data = (char*)"mathcs04";
}

Packet::
~Packet() {
}

char*
Packet::
constructMSG() {
  char msg[8192];
  strcpy(msg, version);            // Sets version field
  strcat(msg, type);            // Sets type field
  strcat(msg, X);         // Sets first name field
  strcat(msg, length);
  strcat(msg, queryID);          // Sets last name field
  strcat(msg, checksum);
  if(strcmp(q_data, "") != 0)
    strcat(msg, q_data);
  else {
    for(unsigned int i = 0; i < r_data.size(); i++) {
      int size = strlen(r_data[i].first);
      if(size > 8) {
        printf("Error: username %s of size, %u, too big", r_data[i].first, size);
        return NULL;
      }
      strcat(msg, r_data[i].first);
      while(size < 8) { // Makes sure each username takes 8 bits
        strcat(msg, (char*)"*"); // Character to be replaced by null character
        size++;
      }
      strcat(msg, r_data[i].second);
    }
  }
  char* tmp = strchr(msg, '*'); // Replaces all tmp *'s with null characters
  while(tmp != NULL) {
    tmp[0] = '\0';
    tmp = strchr(tmp, '*');
  }
  msg1 = (char*) malloc (strlen(msg));
  msg1 = msg;
  return msg1;
}

void
Packet::
parse(char* msg) {
  version = (char*) malloc (4);
  type = (char*) malloc (3);
  X = (char*) malloc (1);
  length = (char*) malloc (8);
  queryID = (char*) malloc (16);
  checksum = (char*) malloc (16);

  int offset = 0;
  strncpy(version,  (const char*)&msg[offset],        4);
  strncpy(type,     (const char*)&msg[offset += 4],   3);
  strncpy(X,        (const char*)&msg[offset += 3],   1);
  strncpy(length,   (const char*)&msg[offset += 1],   8);
  strncpy(queryID,  (const char*)&msg[offset += 16],  16);
  strncpy(checksum, (const char*)&msg[offset += 16],  16);
  //int Length[9];
  //sprintf(Length, "%d", length);

  int Length = atoi(length);
  for(int i = 0; i < Length; i++) {

  }
}

void
Packet::
genQueryID() {
}

int 
Packet::
computeChecksum() {
}

int
Packet::
checkChecksum() {
}

void
Packet::
setData(char* hostname) {
  q_data = hostname;
  length = (char*)"00000001";
}

void
Packet::
setData(char** data, int numEntries) {
  char* name;
  int time;
  for(int i = 0; i < numEntries; i++) {
    name = strtok(data[i], ":");
    time = atoi(strtok(NULL, ":"));
    pair<char*, char*> tmp = make_pair(name, toBinary(time));
    r_data.emplace_back(tmp);
  }
}

void
Packet::
printPacket() {
  printf("\n%s%s%s%s%s\n%s%s\n", version, type, X, length, queryID, checksum, printData());
}

char*
Packet::
printData() {
  if(strcmp(q_data, "") != 0)
    return q_data;
  else {
    char* data = (char*)"";
    int size = 0;
    for(int i = 0; i < (int)r_data.size(); i++) {
      strcat(data, r_data[i].first);
      size = strlen(r_data[i].first);
      while(size < 8) { // Makes sure each username takes 8 bits
        strcat(data, (char*)"*"); // Character to be replaced by null character
        size++;
      }
      strcat(data, r_data[i].second);
    }
    char* tmp = strchr(data, '*'); // Replaces all tmp *'s with null characters
    while(tmp != NULL) {
      tmp[0] = '0';
      tmp = strchr(tmp, '*');
    }
    return data;
  }
}

char*
Packet::
toBinary(const int c) {
  char* binary = (char*)"";
  unsigned int mask = 1 << (sizeof(int) * 8 - 1);
  for(int i = 0; i < (int)sizeof(int)*8; i++) {
    if((c & mask) == 0)
      binary[i] = '0';
    else 
      binary[i] = '1';
    mask >>= 1;
  }
  return binary;
}
//  char* binary = (char*)""; // unsigned chars for both of these?
//unsigned char byte[4];
//byte[0] = (c >> 24) & 0xFF;
//byte[1] = (c >> 16) & 0xFF;
//byte[2] = (c >> 8) & 0xFF;
//byte[3] = c & 0xFF;
//char* b0 = (char*)&byte[0];
//char* b1 = (char*)&byte[1];
//char* b2 = (char*)&byte[2];
//char* b3 = (char*)&byte[3];
//strcpy(binary, b0);
//strcat(binary, b1);
//strcat(binary, b2);
//strcat(binary, b3);
//printf("Binary: %s\n", binary);
//return binary;



