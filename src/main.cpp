// This demo does web requests via DHCP and DNS lookup.
// 2011-07-05 <jc@wippler.nl>
//
// License: GPLv2

#include <SPI.h>
#include <EtherCard.h>
#include <PubSubClient.h>

#define REQUEST_RATE 5000 // milliseconds

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// remote website name
char mqtt[30] = "mqtt.muebaschu.org";

byte Ethernet::buffer[700];
static long timer;

EtherCard ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void sub_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void DHCPOption(uint8_t option, const byte* data, uint8_t len) {
  if (option == 251) {
    strcpy(mqtt, (char*)data);
    mqtt[len] = 0;
  }
}

void setup () {
  Serial.begin(57600);
  Serial.println("\n[getDHCPandDNS]");

  ether.dhcpAddOptionCallback(251,DHCPOption);

  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    Serial.println( "Failed to access Ethernet controller");

  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");

  ether.printIp("My IP: ", ether.myip);
  // ether.printIp("Netmask: ", ether.mymask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);

  Serial.println(mqtt);

  if (!ether.dnsLookup(mqtt, true))
    Serial.println("DNS failed");
  ether.printIp("Server: ", ether.hisip);

  client.setServer(ether.hisip, 1883);
  client.setCallback(sub_callback);





  timer = - REQUEST_RATE; // start timing out right away
}


void loop()
{
 if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

/*
void loop () {

  ether.packetLoop(ether.packetReceive());

  if (millis() > timer + REQUEST_RATE) {
    timer = millis();
    Serial.println("\n>>> REQ");
  }
}


 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

