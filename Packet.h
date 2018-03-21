/* Author: Are Oelsner
 * Networks Project 3
 * Packet class for constructing and parsing packets
 */
#ifndef __PACKET_H__
#define __PACKET_H__

#include <vector>
using namespace std;

class Packet {
  private:
    ///Private Variables
    // Packet Components
      char version[5];              // Version field (4 bits) set to 0110 or 0x6
      char type[4];                 // Type field (3 bits) Query: 000, Resp: 100
      char X[2];                    // X (1 bit) 0 for Q, 0 for neg response, 1 for good response
      unsigned char length[9];      // Length field (8 bit) unsigned char represents # data items
      char queryID[17];             // Query-ID randomly genned 16 bit num client
                                    //    uses to map responses to outstanding requests
      char checksum[17];            // Checksum(16 bit) ones complement checksum of entire packet
      char q_data;                  // Query data stores null terminated hostname string
      vector<pair<char, unsigned int>> r_data;    // Response Data- (8 char + 4 bytes)/pair
  public:
    ///Constructors
    // Default Constructor
    Packet();

    ~Packet();
    
    // Constructs Message from Packet Data
    char constructMSG();        // Constructs char message from packet variables TODO char*?
    void parse(const char msg);       // Parses message into Packet

    ///Setters
    void setVersion(char c[5])  {*version = *c;}
    void setType(char c[4])     {*type    = *c;}
    void setX(char c[2])        {*X       = *c;}
    void setQueryID(char c[17]) {*queryID = *c;}
    
    ///Functions
    void genQueryID();
    int computeChecksum();
    int checkChecksum();
    void setData(char hostname); // Ex. "mathcs02\0"
    void setData(vector<pair<char, unsigned int>> list);
};

#endif
