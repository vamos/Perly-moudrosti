
/*
   Projekt kurzu ISA 2019
   Whois tazatel
   xdvora2t@stud.fit.vutbr.cz
*/

#include "isa-tazatel.h"


/*
 * @brief Prints short help to terminal
 */
void printHelp() {

   cout << "usage: ./isa-tazatel [OPTION] [OPTION] ..."  << endl;
   cout << endl;
   cout << "OPTION                         DESCRIPTION"  << endl;
   cout << endl;
   cout << "Mandatory:" << endl;
   cout << "-q <IPv4/IPv6/hostname>        query"        << endl;
   cout << "-w <IPv4/IPv6/hostname>        whois server" << endl;
   cout << endl;
   cout << "Optional:" << endl;
   cout << "-d <IPv4>                      dns server"   << endl;
   cout << "-h                             this help"    << endl;
   cout << endl;

   exit(0);
}

void errorHandler(string input) {
   
   cerr << "Err // " << input << endl;
   exit(1);
}
/*
 * @brief  parseWhois auxiliary function
 * input:  dns answer
 * output: last word from line
 *
 * source: https://stackoverflow.com/questions/7853686/c-find-return-the-last-word-in-the-string-variable-text-string-getfirstwordt
 */
string getLastword(string str){

   int i = str.length() - 1; // last character
   while (i != 0 && !isspace(str[i])) {
     --i;
   }
   string lastword = str.substr(i+1); // +1 to skip leading space
   return lastword;
}


/*
 * @brief resolves IPv6 address to hostname
 * input:  IPv6 address
 * output: hostname
 *
 * source: https://stackoverflow.com/questions/34362095/getnameinfo-can-it-be-used-to-return-multiple-hostnames-for-a-single-ip-addres
 */
void ipv4ToHost (string ipv4Addr, char *outputHost){

   char host[BUF_SIZE];
   struct sockaddr_in socketAddress;
   socketAddress.sin_family = AF_INET;
   inet_pton(AF_INET, ipv4Addr.c_str(), &(socketAddress.sin_addr));
   getnameinfo((struct sockaddr *)&socketAddress, sizeof(socketAddress), host, BUF_SIZE, NULL, 0, 0);
   strcpy(outputHost, host);
   //cout << "[ipv4ToHost] " << host << endl;
}

/*
 * @brief  resolves IPv6 address to hostname
 * input:  IPv6 address
 * output: hostname
 *
 * source: https://stackoverflow.com/questions/34362095/getnameinfo-can-it-be-used-to-return-multiple-hostnames-for-a-single-ip-addres
 */
void ipv6ToHost (string ipv6Addr, char *outputHost){

   char host[BUF_SIZE_6];
   struct sockaddr_in6 socketAddress;
   socketAddress.sin6_family = AF_INET6;
   inet_pton(AF_INET6, ipv6Addr.c_str(), &(socketAddress.sin6_addr));
   int gniResult = getnameinfo((struct sockaddr *)&socketAddress, sizeof(socketAddress), host, BUF_SIZE_6, NULL, 0, 0);
   if (strcmp(host, ipv6Addr.c_str()) == 0) { // getnameinfo returns input
      cout << "Err // Cant resolve IPv6 address. (" << host << ")" << endl;
      exit(1);
   }
   strcpy(outputHost, host);
}

/*
 * @brief print input argument flags
 */
void printFlags(){
   cout << "QWD Flags: ";
   for ( int i = 0; i < FLAGS_SIZE; i++ ) {
      cout << flags[i] << "|";
   }
   cout << endl;
}

/*
 * @brief print input argument parameters
 */
void printParams(){

   cout << "qValue: " << qValue << endl;
   cout << "qHostname: " << qHostname << endl;
   cout << "wValue: " << wValue << endl;
   cout << "dValue: " << dValue << endl;
}
/*
 * @brief  translate hostname from -q argument to IP address
 * input:  hostname
 * output: IPv4 address
 */
int hostToAddr (char *input, struct addrinfo **output) {

   int ansr;
   struct addrinfo hints;
   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;             
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

      if ((ansr = getaddrinfo(input, "43", &hints, output)) != 0)    {
      cerr << "Err // " << input << " " << gai_strerror(ansr) << endl;
      exit(1);
   }
   return 0;
}


/*
 * @brief determine input version IPv4 / IPv6 / hostname
 */
int getVer(const char *input){

   char buffer[16];
   if (inet_pton(AF_INET, input, buffer)) {
      return 4;
   }
   if (inet_pton(AF_INET6, input, buffer)) {
      return 6;
   }
   if (gethostbyname(input)) {
      return 2;
   }
   cerr << "Err // Wrong argument parameters. (" << input << ")" << endl;
   exit(1);

}

/*
 * @brief resolves ip address to hostname
 */
int ipFromHost (const char *input, char *output) {

   hostent * hostTmp;
   in_addr * address;
   string whoisIp;
   hostTmp = gethostbyname(input);
   address = (in_addr*)hostTmp->h_addr;
   whoisIp =inet_ntoa(* address);
   strcpy(output,whoisIp.c_str());
   return 0;
}

/*
 * @brief parse command line arguments
 */ 
void parseArgs(int argc, char *argv[]) {
   while (( options = getopt(argc, argv, "q:w:d:h")) != -1) {
      switch (options) {
      case 'q':   // query of this IP
         flags[0] = getVer(optarg);
         strcpy(qValue,optarg);
         break;
      case 'w':   // whois server
         flags[1] = getVer(optarg);
         strcpy(wValue,optarg);
         break;
      case 'd':   // dns server
         flags[2] = getVer(optarg);
         strcpy(dValue,optarg);
         break;
      case 'h':   // help
         printHelp();
         break;
      default:
         break;
      }
   }

   // check if arguments are valid
   if ( flags[0] == 0 || flags[1] == 0 ) {
      cerr << "Error // Invalid arguments!" << endl;
      exit(1);
   }

   // IPv4 in q argument resolve to hostname
   // store address in input WHOIS vector
   if ( flags[0] == 4 ) {
      ipv4ToHost(qValue, qHostname);
      queryIPs.push_back(qValue);  
   }
   // IPv6 in q argument resolve to hostname
   // store address in input WHOIS vector
   if ( flags[0] == 6 ) {
      ipv6ToHost(qValue, qHostname);
      queryIPs.push_back(qValue);
   }
   // hostname in q argument
   if ( flags[0] == 2 ) {
      strcpy(qHostname, qValue);
   }
   
   // IPv4 in w argument
   if ( flags[1] == 4 ) {
      ipv4ToHost(wValue, wValue);
   }
   // IPv6 in w argument
   if ( flags[1] == 6 ) {
      ipv6ToHost(wValue, wValue);
   }
   // WHOIS hostname in w argument
   if ( flags[1] == 2 ) {
      strcpy(wValue, wValue);
   }
}

/*
 * @brief compare keyword in dictionary with WHOIS line
 */
int parseWhois(string input) {

   string firstWord = input.substr(0, input.find(":"));
   for ( int i=0; i <keywords.size(); i++ ){
      if (firstWord.compare(keywords[i]) == 0) {
         return 0;
      }   
   }
   return 1;
}

/*
 * @brief gets DNS  A, AAAA, MX, NS, PTR, CNAME and SOA records
 */
int dnsQuery(char *input, int dFlag){

   unsigned char buffer[BUF_SIZE];  // query buffer
   char outBuffer[BUF_SIZE];        // output buffer / aux. variable
   stringstream ss;                 // aux. variable
   int msgCount;                    // messages counter
   ns_msg nsMsg;                    // output message
   ns_rr rr;                        // resource records

   // resolver state
   res_state state = static_cast<res_state>(malloc(sizeof(*state)));

   //res_ninit(state); // init resolv.h structures
   res_init(); 
   
   // add dns address if -d parameter is set
   if ( flags[2] && dFlag) {
      // setting nameserver from -d argument     
      //printf("setting %s as nameserver\n", dValue);
      _res.nscount = 1;
      _res.nsaddr_list[0].sin_family = AF_INET;
      _res.nsaddr_list[0].sin_addr.s_addr = inet_addr(dValue);
      _res.nsaddr_list[0].sin_port = htons(53);   
   }

   // DNS A RECORD QUERY
   int response = res_query( input, ns_c_in, ns_t_a, buffer, sizeof(buffer));
   if (response < 0) {
      return 1;
      // if no response, continue
   } else {
      if (ns_initparse(buffer, response, &nsMsg) < 0) {
         errorHandler("ns_initparse");
      }
      int l = ns_msg_count(nsMsg, ns_s_an);
      for (int i = 0; i < l; i++) {

         if (ns_parserr(&nsMsg, ns_s_an, i, &rr) < 0) {
            errorHandler("ns_parserr");
         }
         // converts rr to presentable format
         if (ns_sprintrr(&nsMsg, &rr, NULL, NULL, outBuffer, sizeof(outBuffer)) < 0) {
            errorHandler("ns_sprintrr");
         }

         //printf("\t%s \n", outBuffer);
         // source: http://people.samba.org/bzr/jerry/slag/unix/query-srv.c
			char name[BUF_SIZE];
			int ret;
			struct in_addr ip;
			const unsigned char *p;
         p = ns_rr_rdata(rr);
         ip.s_addr = (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
         string aresult = inet_ntoa(ip);
         cout << "A:     " << aresult << endl;

         // input vector of IPs/hostnames for WHOIS
         queryIPs.push_back(aresult);
      }
   }

   // DNS AAAA RECORD QUERY
   response = res_query(input, ns_c_in, ns_t_aaaa, buffer, sizeof(buffer));
   if (response < 0) {
      // if no response, continue
   } else {
      if (ns_initparse(buffer, response, &nsMsg) < 0 ) {
         errorHandler("ns_initparse");
      }
      int l = ns_msg_count(nsMsg, ns_s_an);
      for (int i = 0; i < l; i++) {

         if (ns_parserr(&nsMsg, ns_s_an, i, &rr) < 0 ) {
            errorHandler("ns_parserr");
         }
         // converts rr to presentable format
         if (ns_sprintrr(&nsMsg, &rr, NULL, NULL, outBuffer, sizeof(outBuffer)) < 0 ) {
            errorHandler("ns_sprintrr");
         }
         string aaaa = getLastword(outBuffer);
         cout << "AAAA:  " << aaaa << endl;
      }
   }

   // DNS MX RECORD QUERY
   response = res_query(input, ns_c_in, ns_t_mx, buffer, sizeof(buffer));
   if (response < 0) {
      // if no response, continue
   } else {
      if (ns_initparse(buffer, response, &nsMsg) < 0 ) {
         errorHandler("ns_initparse");
      }
      int l = ns_msg_count(nsMsg, ns_s_an);
      for (int i = 0; i < l; i++) {

         if (ns_parserr(&nsMsg, ns_s_an, i, &rr) < 0 ) {
            errorHandler("ns_parserr");
         }
         if (ns_sprintrr(&nsMsg, &rr, NULL, NULL, outBuffer, sizeof(outBuffer)) < 0 ) {
            errorHandler("ns_sprintrr");
         }
         string mx = getLastword(outBuffer);
         cout << "MX:    " << mx << endl;
      }
   }

   // DNS CNAME RECORD QUERY
   response = res_query(input, ns_c_in, ns_t_cname, buffer, sizeof(buffer));
   if (response < 0) {
      // if no response, continue
   } else {
      if (ns_initparse(buffer, response, &nsMsg) < 0 ) {
         errorHandler("ns_initparse");
      }
      int l = ns_msg_count(nsMsg, ns_s_an);
      for (int i = 0; i < l; i++) {

         if (ns_parserr(&nsMsg, ns_s_an, i, &rr) < 0 ) {
            errorHandler("ns_parserr");
         }
         if (ns_sprintrr(&nsMsg, &rr, NULL, NULL, outBuffer, sizeof(outBuffer)) < 0 ) {
            errorHandler("ns_sprintrr");
         }
         string cname = getLastword(outBuffer);
         cout << "CNAME: " << cname << endl;
      }
   }
   // DNS PTR RECORD QUERY
   int a,b,c,d;
   char input_ptr[29];
   // format input
   sscanf(queryIPs[0].c_str(),"%d.%d.%d.%d",&a,&b,&c,&d);
   sprintf(input_ptr, "%d.%d.%d.%d.in-addr.arpa", d, c, b, a);

   response = res_query(input_ptr, ns_c_in, ns_t_ptr, buffer, sizeof(buffer));
   if (response < 0) {
      // if no response, continue
   } else {
      if (ns_initparse(buffer, response, &nsMsg) < 0 ) {
         errorHandler("ns_initparse");
      }
      int l = ns_msg_count(nsMsg, ns_s_an);
      for (int i = 0; i < l; i++) {

         if (ns_parserr(&nsMsg, ns_s_an, i, &rr) < 0 ) {
            errorHandler("ns_parserr");
         }
         if (ns_sprintrr(&nsMsg, &rr, NULL, NULL, outBuffer, sizeof(outBuffer)) < 0 ) {
            errorHandler("ns_sprintrr");
         }
         string ptr = getLastword(outBuffer);
         ptr = ptr.substr(0, ptr.size()-1);  // remove last dot
         cout << "PTR:   " << ptr << endl;

         // insert into input WHOIS vector
         //queryIPs.push_back(ptr);

      }
   }

   // DNS SOA RECORD QUERY
   string inputWithoutWww = input;
   size_t pos = inputWithoutWww.find("www.");
   if ( pos!=string::npos) { // www. found
      inputWithoutWww = inputWithoutWww.substr(pos+4);
      strcpy(input,inputWithoutWww.c_str());
   }
   response = res_query(input, ns_c_in, ns_t_soa, buffer, sizeof(buffer));
   if (response < 0) {
      // if no response, continue
   } else {
      if (ns_initparse(buffer, response, &nsMsg) < 0 ) {
         errorHandler("ns_initparse");
      }
      int l = ns_msg_count(nsMsg, ns_s_an);
      for (int i = 0; i < l; i++) {

         if (ns_parserr(&nsMsg, ns_s_an, i, &rr) < 0 ) {
            errorHandler("ns_parserr");
         }
         if (ns_sprintrr(&nsMsg, &rr, NULL, NULL, outBuffer, sizeof(outBuffer)) < 0 ) {
            errorHandler("ns_sprintrr");
         }
         string soa(outBuffer);  // converts char array to string
         stringstream ss(soa);
         istream_iterator<std::string> begin(ss);
         istream_iterator<std::string> end;
         vector<string> soaVector(begin, end);
         for ( int i = 0; i < soaVector.size(); i++ ) {
            size_t found = soaVector[i].find("." + inputWithoutWww);
            if (found != string::npos){
               cout << "SOA:   " << soaVector[i] << endl;
            }
         }
      }
   }

   // DNS NS RECORD QUERY
   response = res_query(input, ns_c_in, ns_t_ns, buffer, sizeof(buffer));
   if (response < 0) {
      // if no response, continue
   } else {

      if (ns_initparse(buffer, response, &nsMsg) < 0 ) {
         errorHandler("ns_initparse");
      }
      int l = ns_msg_count(nsMsg, ns_s_an);
      for (int i = 0; i < l; i++) {
         if (ns_parserr(&nsMsg, ns_s_an, i, &rr) < 0 ) {
            errorHandler("ns_parserr");
         }
         if (ns_sprintrr(&nsMsg, &rr, NULL, NULL, outBuffer, sizeof(outBuffer)) < 0 ) {
            errorHandler("ns_sprintrr");
         }
         string nsq = getLastword(outBuffer);
         cout << "NS:    " << nsq << endl;
      }
   }
   return 0;
}


/*
 * @brief performs WHOIS query
 * input: argument -q <query address>
 * input: argument -w <whois server>
 */
int whois(char *inputQArg, struct addrinfo *srvr) {

   int sckt, whoisAnswer;        // socket, whoisAnswer init
   char mssg[MSG_SIZE];          // input whois message
   char buffer[BUF_SIZE];        // output buffer
   int queryResult;              // aux. var.
   string line;                  // aux. var for parsing answer
   stringstream ss;              // aux. var for parsing answer

   cout << "=== WHOIS === " << inputQArg << endl;
   if ((sckt = socket(srvr->ai_family, srvr->ai_socktype, srvr->ai_protocol)) == -1) {
      perror("socket");
   }
   
   // set socket to nonblocking because we want to set a timeout
   int flags = fcntl(sckt,F_GETFL,0);
   if (flags >= 0) {
      flags = fcntl(sckt, F_SETFL, flags | O_NONBLOCK); 
   }
   int cnct = connect(sckt, srvr->ai_addr, srvr->ai_addrlen);
   
   // source: https://stackoverflow.com/questions/29913483/how-to-set-a-timeout-for-connect-and-how-to-measure-it/29915606
   if(cnct < 0) {  
      if(errno != EINPROGRESS && errno != EWOULDBLOCK) 
          return 1; 
   
      struct timeval tv; 
      tv.tv_sec = 3;            // timeout after 3 seconds
      tv.tv_usec = 0; 
      fd_set wset; 
      FD_ZERO(&wset); 
      FD_SET(sckt,&wset); 
      cnct = select(sckt+1,NULL,&wset,NULL,&tv); 
      if(cnct < 0) {
          close(sckt); 
          return 1; 
      } 
      else if (0 == cnct) { // timeout
          cerr<< "Err // Timeout" << endl; 
          close(sckt); 
          exit(1);
      } 
   }
   
   // format input WHOIS message
   sprintf(mssg, "%s\r\n", inputQArg);
   //cout << "mssg: " << mssg << endl;

   queryResult = send(sckt, &mssg, strlen(mssg), 0);

   // no record or query failed
   if (queryResult < 0) {
      return 1;
   };

   // fill buffer with WHOIS answer
   while ( (whoisAnswer = recv(sckt, buffer, sizeof(buffer), 0) ) ) {
   }

   // make stringstream from WHOIS answer
   ss << buffer;

   // WHOIS parse
   int foundCnt = 0; // counting found records
   while (getline(ss, line)) {
      if ( parseWhois(line) == 0 ) {
	      whoisOutput.insert(line);
         foundCnt++;
      }

   }
   if ( foundCnt == 0 ) { // no record found
      return 1;
   }

   // WHOIS output
   for (string const& line : whoisOutput) { // iterate throught set
      std::cout << line << endl;
   }
   //cout << "=== ORIGINAL ===" << endl;
   //cout << buffer << endl;
   return 0;
}


int main(int argc, char *argv[]) {

   // whois server structure
   struct addrinfo *server;

   // parse command line arguments
   parseArgs(argc, argv);
   
   // debug functions
   //printFlags();
   //printParams();
   
   // WHOIS parameter to addrinfo struct
   hostToAddr(wValue, &server);

   // make dns query for -q parameter hostname
   cout << "===DNS===" << endl;
   int dnsQResult = dnsQuery(qHostname, 1); // 1 - check -d flag

   // aux. variable for WHOIS output check
   int whoisOut;

   if ( dnsQResult ) {

      cerr << "Err // No response from -d nameserver" << endl;
      if ( flags[2] ) { // -d parameter set
         cout << "Trying nameserver in /etc/resolv.conf" << endl;
         dnsQuery(qHostname, 0); // 0 - dont check -d flag

      } else {
         // query hostname
         whois(qHostname, server);
         return 0;
      }
   }

   // WHOIS input variable init
   char cstr[queryIPs[0].size()+1];

   // WHOIS query
   for (int i=0; i<queryIPs.size(); i++){
      strcpy(cstr, queryIPs[i].c_str());   
      // query IP adresses in input WHOIS vector 
      whoisOut = whois(cstr, server);        
      if ( whoisOut == 0 ) {
         break;
      }
   }

   if ( whoisOut == 1 ) {  // query hostname
      whois(qHostname, server);
   }
   return 0;
}
