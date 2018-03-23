/* Author: Are Oelsner
 * Networks Project 3
 * Packet class for constructing and parsing packets
 */

#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
using namespace std;

struct Packet {
  // Packet Components
  unsigned char version;    // Version field (4 bits) set to 0110 or 0x6
  unsigned char type;       // Type field (3 bits) Query: 000, Resp: 100
  unsigned char X;          // X (1 bit) 0 for Q, 0 for neg response, 1 for good response
  unsigned char length;     // Length field (8 bit) unsigned char represents # data items
  unsigned short queryID;    // Query-ID randomly genned 16 bit num client
  unsigned short checksum;   // Checksum(16 bit) ones complement checksum of entire packet
  char data[2048];        // Query data stores null terminated hostname string
};

///Functions

void
genQueryID(struct Packet *p) {
  srand(time(NULL));
  p->queryID = (unsigned short) rand() % 2147483648;
}

unsigned short
checksum(struct Packet *p) {
  unsigned short c1 = p->version;
  c1 <<= 3;
  c1 += p->type;
  c1 <<= 1;
  c1 += p->X;
  c1 <<= 8;
  c1 += p->length;
  unsigned short c2 = p->queryID;
  unsigned short c3 = p->checksum;
  int size = strlen(p->data);
  unsigned short* cx;
  int i = 0;
  int k = 0;
  while(i < size) {
    cx[k] = 0x0000;
    for(int j = 0; j < 16; j++) {
      if(i < size) 
        cx[j] = p->data[i];
      else
        cx[j] = '0';
      i++;
    }
    k++;
  }
  unsigned short sum = 0x0000;
  for(int l = 0; l < k; l++)
    sum = sum ^ cx[l];
  sum = sum ^ c1;
  sum = sum ^ c2;
  sum = sum ^ c3;
  return sum;
}

unsigned short
Checksum(unsigned short* p, int numBytes) {
  unsigned long sum = 0x0;
  unsigned short odd;
  unsigned short result;

  while(numBytes > 1) {
    sum += *p++;
    numBytes-=2;
  }
  if(numBytes == 1) {
    odd = 0;;
    *((unsigned char*)&odd)=*(unsigned char*)p; //TODO change to = *
    sum += odd;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum = sum + (sum >> 16);
  result = (unsigned short) ~sum;
  return result;
}

void
setData(struct Packet *p, char** data, int numEntries) {
  char* name;
  char* time;
  int size;
  char space[2] = " ";
  for(int i = 0; i < numEntries; i++) {
    name = strtok(data[i], ":");
    time = strtok(NULL, ":");
    size = strlen(name);
    strcat(p->data, name);
    for(int j = size; j < 8; j++)
      strcat(p->data, space);
    size = strlen(time);
    for(int j = size; j < 10; j++)
      strcat(p->data, space);
    strcat(p->data, time);
  }
}

void
printData(const struct Packet *p) {
  printf("Username\t      Time\n");
  for(int i = 0; i < p->length*18; i+= 18) {
    printf("%.*s\t%.*s\n", 8, p->data+i, 10, p->data+i+8);
  }
}

void
printPacket(const struct Packet *p) {
  printf("vers\ttype\tX\tlength\tqueryID\tchecksum\n");
  printf("%u\t%u\t%u\t%u\t%u\t%u\n", p->version, p->type, p->X, p->length, p->queryID, p->checksum);
  printData(p);
}





