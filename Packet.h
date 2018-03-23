/* Author: Are Oelsner
 * Networks Project 3
 * Packet class for constructing and parsing packets
 */
#ifndef __PACKET_H__
#define __PACKET_H__

#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
using namespace std;

class Packet {
  private:
    ///Private Variables
    char* msg1;
    // Packet Components
      char* version;              // Version field (4 bits) set to 0110 or 0x6
      char* type;                 // Type field (3 bits) Query: 000, Resp: 100
      char* X;                    // X (1 bit) 0 for Q, 0 for neg response, 1 for good response
      char* length;      // Length field (8 bit) unsigned char represents # data items
      char* queryID;             // Query-ID randomly genned 16 bit num client
                                    //    uses to map responses to outstanding requests
      char* checksum;            // Checksum(16 bit) ones complement checksum of entire packet
      char* q_data;                  // Query data stores null terminated hostname string
      vector<pair<char*, char*>> r_data;    // Response Data- (8 char + 4 bytes)/pair
  public:
    ///Constructors
    // Default Constructor
    Packet();

    ~Packet();
    
    // Constructs Message from Packet Data
    char* constructMSG();        // Constructs char message from packet variables TODO char*?
    void parse(char* msg);       // Parses message into Packet

    ///Setters
    void setVersion(char* c)  {version = c;}
    void setType(char* c)     {type    = c;}
    void setX(char* c)        {X       = c;}
    void setLength(char* c)   {length  = c;}
    void setQueryID(char* c)  {queryID = c;}

    ///Getters
    char* getVersion()  {return version;}
    char* getType()     {return type;}
    char* getX()        {return X;}
    char* getLength()   {return length;}
    char* getQueryID()  {return queryID;}
    char* getChecksum() {return checksum;}
    char* getQData()    {return q_data;}
    vector<pair<char*, char*>> getData() {return r_data;}
    
    ///Functions
    void genQueryID();
    int computeChecksum();
    int checkChecksum();
    void setData(char* hostname); // Ex. "mathcs02\0"
    void setData(char** data, int numEntries);
    void printPacket();
    char* printData();

    ///Helper Functions
    //www.cplusplus.com/forum/beginner/187933/
    char* toBinary(int c);
};

#endif
