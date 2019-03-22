#include <SPI.h>
#include <Ethernet.h>

//Local MAC address
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x3A, 0xED};

//Metic for each plant
String name = "plant1";

//Server addresses
//IPAddress server(192,168,2,61);
char server[] = "donald.sch.bme.hu";
int serverPort = 10305;

//PIN data
int lightDataPin = A5;
int moistureDataPin = A4;

//Data that will be sent to server
int moistureData = 0;
int lightData = 0;

//POST data
String PostData = "[]";

//Ethernet client
EthernetClient client;

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds

void setup()
{
  Serial.begin(9600);
  //while (!Serial) {
    //; // wait for serial port to connect.
  //}
  Serial.println("Serial monitor started!");

  Serial.println("Trying to get an IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // initialize the Ethernet device not using DHCP:
    IPAddress ip = Ethernet.localIP();
    Ethernet.begin(mac, ip);
    Serial.print("My IP address: "+ ip);
  }

  //Give the Ethernet shield a second to initialize:
  delay(1000);

  //If you get a connection, report back via serial:
  if (client.connect(server, serverPort)) {
    Serial.println("Successful connection\n\n");
    
  } else {
    //If you didn't get a connection to the server:
    Serial.println("Connection failed\n\n");
    //Waiting for restart
    while(1){}
  }
  
}

void CollectData()
{
  //Collect 15 sample data with 1 sec cooldown
  int i;
  moistureData = 0;
  lightData = 0;
  for(i = 1; i<=15; i++)
  {
    moistureData += 1024-analogRead(moistureDataPin);
    Serial.println(lightData);
    lightData += 1024-analogRead(lightDataPin);
    delay(1000);
  }
  //Make an average value
  moistureData = moistureData / 15;
  lightData = lightData / 15;
}

void FillPostData(int data, String type)
{
  //Fill PostData
  
  //Metric is data.<type>.<name>
  //Example: data.light.plant1
  
  //Timestamp always -1  -> 
  //Graphite calculate it from package arrive time
  
  String metric = "\"data." + type + "." + name + "\"";
  PostData = String("[{\"metric\": " + metric +
          ", \"value\": " + String(data) +
          ", \"timestamp\": -1" + "}]");
  
  if(client.connect(server, serverPort)) 
  {
    // Make a HTTP POST request:
    //Route is /publish/gardener
    client.println("POST /publish/gardener HTTP/1.0");
    client.println("Host: " + String(server));
    client.println("User-Agent: Arduino/1.0");
    //Make sure keep-alive is turned on
    client.println("Connection: keep-alive");
    client.println("Keep-Alive: timeout=120, max=150");
    
    //JSON message format
    client.println("Content-Type: application/json;");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);

    //Info in serial monitor too
    Serial.println("The next information about plant:");
    Serial.println("light: " + String(lightData) + ", moisture:" + String(moistureData));
    Serial.println("is sent to server.\n");
  } 
  else 
  {
    //If we didn't get a connection to the server:
    Serial.println("Connection failed [POST]");
    return 0;
  }
}

void loop()
{
  int i = 0;
  for(i = 1; i<=15; i++)
  {
    //Collect data from our pins
    CollectData();

    //Setup post requests
    FillPostData(lightData, "light");
    FillPostData(moistureData, "moisture");

    delay(10000);
  }
  //TODO 
  //Ide jön a response ami a majd a konstansokat küldi a tresholdhoz :D
  //Teszt fázis alatt.
  getPlantInfo();
  
  delay(5000);
}

// this method makes a HTTP connection to the server:
void getPlantInfo() 
{
  // if there's a successful connection:
  if (client.connect(server, serverPort)) {
    //Send the HTTP GET request:
    client.println("GET /getinfo/" + name + " HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("User-Agent: Arduino/1.0");
    //Make sure keep-alive is turned on
    client.println("Connection: keep-alive");
    client.println("Keep-Alive: timeout=120, max=150");
    client.println();

  } 
  else 
  {
    //If we didn't get a connection to the server:
    Serial.println("connection failed [GET]");
  }

  if (client.available()) 
  {
    char c = client.read();
    Serial.write(c);
  }
}


String readString = String(100); //string for fetching data from address

///////////////////////
String teststring = String(100);
String finalstring = String(100);
String flag = String(2);
int ind1 = 0;
int ind2 = 0;
int pos = 0;
//////////////////////

void loop(){
// Create a client connection
Client client = server.available();
if (client) {
while (client.connected()) {
if (client.available()) {
char c = client.read();

//read char by char HTTP request
if (readString.length() < 100) {

//store characters to string
readString.append(c);
}

//if HTTP request has ended
if (c == '\n') {

///////////////
//Serial.println(readString);
//readString looks like "GET /?-0p1555-1p500t1000 HTTP/1.1"

 if(readString.contains("-")) { //test for servo control sring
 readString.replace('-', '#');
 pos = readString.length(); //capture string length
 //find start of servo command string (#)
 ind1 = readString.indexOf('#');
 //capture front part of command string
 teststring = readString.substring(ind1, pos);
 //locate the end of the command string
 ind2 = teststring.indexOf(' ');
 //capturing the servo command string from readString
 finalstring = readString.substring(ind1, ind2+ind1);
 //print "finalstring" to com port;
 Serial.println(finalstring); //print string with CR
   }
 ////////////////////////
 //GET /?Slidervalue0=1800&Submit=Sub+0 HTTP/1.1
 if(readString.contains("Slidervalue")) {
 ind1 = readString.indexOf('u');
 ind2 = readString.indexOf('&');
 finalstring = readString.substring(ind1+1, ind2);
 finalstring.replace('e', '#');
 finalstring.replace('=', 'p');
 Serial.println(finalstring);
 }
 ///////////////////
 
 //now output HTML data header
 client.println("HTTP/1.1 204 Zoomkat");
 client.println();
 client.println();
 delay(1);
 //stopping client
client.stop();

/////////////////////
//clearing string for next read
readString="";
teststring="";
finalstring="";
 
}}}}} 
