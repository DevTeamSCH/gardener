#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h> 

//Local addresses
byte mac[] = {0x90, 0x91, 0x92, 0x93, 0x94, 0x95};
IPAddress ip(169, 254, 72, 169);

//Server addresses
IPAddress serverAddress = (169, 254, 72, 168);
int serverPort = 8080;

//PIN data
int lightDataPin = A5;
int moistureDataPin = A4;

//Data that will be sent to server
int moistureData = 0;
int lightData = 0;    

//UDP config
EthernetUDP udp;
unsigned int localPort = 8888; 

//Message buffers
String formattedMessage;
char messageBuffer[30]; 
//char packetBuffer[UDP_TX_PACKET_MAX_SIZE];



void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  Serial.println("Serial monitor started!");
  //Init
  Ethernet.begin(mac, ip);
  Serial.println("Successful ehernet initalization!");
  udp.begin(localPort);
  Serial.println("UDP port activated for communication"); //TODO write localport
}

void loop()
{
  // if there's data available, read a packet
  /*int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i = 0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);*/

  
  moistureData = analogRead(moistureDataPin);
  lightData = analogRead(lightDataPin);

  //Fill messageBuffer
  formattedMessage = String("light : " + String(lightData) + " moisture :" + String(moistureData));
  formattedMessage.toCharArray(messageBuffer, 30);

  udp.beginPacket(serverAddress, serverPort);
  udp.write(messageBuffer);
  udp.endPacket();
  Serial.println(messageBuffer);
  delay(3000);
}
