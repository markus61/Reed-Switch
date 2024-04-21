// This demo does web requests via DHCP and DNS lookup.
// 2011-07-05 <jc@wippler.nl>
//
// License: GPLv2

#include <EtherCard.h>

#define REQUEST_RATE 5000 // milliseconds

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// remote website name
char mqtt[30] = "mqtt.muebaschu.org";

byte Ethernet::buffer[700];
static long timer;

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

  timer = - REQUEST_RATE; // start timing out right away
}

void loop () {

  ether.packetLoop(ether.packetReceive());

  if (millis() > timer + REQUEST_RATE) {
    timer = millis();
    Serial.println("\n>>> REQ");
  }
}