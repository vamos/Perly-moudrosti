#include <unistd.h>
#include <stdlib.h>
#include <iostream>  		      // cout
#include <sstream>  		         // stringstream
#include <string.h>
#include <stdio.h>
#include <errno.h>               // errors
#include <arpa/inet.h>           // internet operations
#include <arpa/nameser.h>
#include <netinet/in.h>          // ns_initparse?
#include <netdb.h>   		      // gethostbyname
#include <csignal>
#include <resolv.h>		         // res_init()
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <functional>
#include <sys/types.h>
#include <fcntl.h>               // connection timeout

using namespace std;

#define FLAGS_SIZE   3           // size of array flags for all arguments q/w/d
#define MSG_SIZE     100         // input whois message or input parameter max size
#define BUF_SIZE     2048        // standard bufer size
#define BUF_SIZE_6   4096        // IPv6 buffer size

int options;                     // ParseArgs() auxiliary variable

char qValue[MSG_SIZE];           // input argument value
char wValue[MSG_SIZE];           // input argument value
char dValue[MSG_SIZE];           // input argument value

char qHostname[MSG_SIZE];        // hostname from q argument parameter

// vector of strings / lines from whois answer
set<string> whoisOutput;

// output of DNS, input to WHOIS IPv4
vector<string> queryIPs;  

// input argument flags / q w d
int flags[FLAGS_SIZE] = { 0, 0, 0, }; 

// keyword for parsing WHOIS output
vector<string> keywords {
        
   // ripe 
   {"inetnum"},
   {"netname"},        
   {"descr"},        
   {"address"},        
   {"country"},        
   {"admin-c"},        
   {"phone"},        

   // arin, afrinic
   {"NetRange"},        
   {"NetName"},        
   {"Organization"},        
   {"Address"},        
   {"Country"},        
   {"PostalCode"},        
   {"StateProv"},        
   {"City"},        
   
   // lacnic
   {"owner"},        
   {"aut-num"},        
   {"responsible"},        
   {"abuse-c"},        

   {"contact"},        
   {"org"},        
};


