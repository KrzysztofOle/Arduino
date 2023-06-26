#define MACADDRESS 0x00,0x01,0x02,0x03,0x04,0x05
#define MYIPADDR 192,168,3,30
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,3,1
#define MYGW 192,168,3,1
#define LISTENPORT 12345
#define UARTBAUD 9600
#include <UIPEthernet.h>
#include "utility/logging.h"
EthernetServer server = EthernetServer(LISTENPORT);

void PrintNetworkSettings()
{
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());
}

void setup() {
  Serial.begin(UARTBAUD);
  Serial.println("Start Server");
  uint8_t mac[6] = {MACADDRESS};
  
  uint8_t myIP[4] = {MYIPADDR};
  uint8_t myMASK[4] = {MYIPMASK};
  uint8_t myDNS[4] = {MYDNS};
  uint8_t myGW[4] = {MYGW};

  Serial.println("begin...");
  Ethernet.begin(mac,myIP,myDNS,myGW,myMASK);
  Ethernet.begin(mac);  //DHCP
  Serial.println("print setings...");
  PrintNetworkSettings();
  server.begin();
}
void loop() {
  size_t size;
  if (EthernetClient client = server.available())
  {
      while((size = client.available()) > 0)
      {
          uint8_t* msg = (uint8_t*)malloc(size+1);
          memset(msg, 0, size+1);
          size = client.read(msg,size);
          free(msg);
      }
      client.stop();
    }
}
