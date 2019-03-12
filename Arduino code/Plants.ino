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

void loop()
{
  moistureData = analogRead(moistureDataPin);
  lightData = analogRead(lightDataPin);

  //Fill PostData
  //(Only for light now.)
  //Metric is light/moisutre + plant name
  //Timestamp always -1  -> Graphite calculate it from package arrive time
  String metric = "\"data.light." + name + "\"";
  PostData = String("[{\"metric\": " + metric + ", \"value\": " + String(lightData) + ", \"timestamp\": -1" + "}]");
  int inChar;

  //Setup post request
  if (client.connect(server, serverPort)) {
  // Make a HTTP POST request:
  //Route is /publish/gardener
  client.println("POST /publish/gardener HTTP/1.0");
  client.println("Host: " + String(server));
  client.println("User-Agent: Arduino/1.0");
  client.println("Connection: close");
  //JSON message format
  client.println("Content-Type: application/json;");
  client.print("Content-Length: ");
  client.println(PostData.length());
  client.println();
  client.println(PostData);

  //Info in serail monitor too
  Serial.println("The next information about plant:");
  Serial.println(PostData);
  Serial.println("is sent to server.\n");
  } else {
    //If we didn't get a connection to the server:
    Serial.println("Connection failed");
  }

  //TODO 
  //Ide jön a response ami a majd a konstansokat küldi a tresholdhoz :D
  //Teszt fázis alatt.
  int connectLoop = 0;

  while(client.connected())
  {
    while(client.available())
    {
      inChar = client.read();
      Serial.println("Response: ");
      Serial.write(inChar);
      connectLoop = 0;
    }

    delay(1);
    connectLoop++;
    if(connectLoop > 10000)
    {
      Serial.println();
      Serial.println("Timeout");
      client.stop();
    }
  }

  Serial.println();
  Serial.println("Teszt üzenet a response-hoz");
  client.stop();

  delay(30000);
}
