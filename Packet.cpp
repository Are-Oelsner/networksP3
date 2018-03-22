#include "Packet.h"

Packet::
Packet() {
  version = (char*)"0110";
  type = (char*)"000";
  X = (char*)"0";
  length = (char*)"1";
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
  char msg[1000];
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
        printf("Error: username size, %u, too big", size);
        return NULL;
      }
      strcat(msg, r_data[i].first);
      while(size < 8) {
        msg[size] = '\0';
        size++;
      }
      strcat(msg, to_string(r_data[i].second).c_str());
    }
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
}

void
Packet::
setData(vector<pair<char*, unsigned int>> list) {
}


