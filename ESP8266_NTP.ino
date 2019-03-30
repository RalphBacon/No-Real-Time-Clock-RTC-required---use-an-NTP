/* A simple UDP example which connects to an NTP (Network Time Protocol) ideally in a pool (so we get
 * a random NTP server given to us from that pool).
 *
 * The reply (seconds since midnight 1970) is then converted into the date and time for use in emails.
 *
 *	FYI millis() rolls over after 71.6 minutes on an ESP8266, 49.7 days on an Arduino ATMega328P
 */
#include <arduino.h>

// My secret stuff (eg WiFi password)
#include "config.h"

// How we connect to your local wifi
#include <ESP8266WiFi.h>

// UDP library which is how we communicate with Time Server
#include <WiFiUdp.h>

// See Arduino Playground for details of this useful time synchronisation library
#include <TimeLib.h>

// WiFi specific defines
#define WifiTimeOutSeconds 10
//#define _ssid 			MY SSID
//#define _password 		MY WIFI PASSWORD

// Just an open port we can use for the UDP packets coming back in
unsigned int localPort = 8888;

// this is the "pool" name for any number of NTP servers in the pool.
// If you're not in the UK, use "time.nist.gov"
// Elsewhere: USA us.pool.ntp.org
// Read more here: http://www.pool.ntp.org/en/use.html
char timeServer[] = "time.nist.gov"; //"uk.pool.ntp.org";

// NTP time stamp is in the first 48 bytes of the message
const int NTP_PACKET_SIZE = 48;

//buffer to hold incoming and outgoing packets
byte packetBuffer[NTP_PACKET_SIZE];

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

// Your time zone relative to GMT / UTC
// Not used (yet)
const int timeZone = 1;

// Days of week. Day 1 = Sunday
String DoW[] = {"Sun", "Mon","Tue","Wed","Thu","Fri","Sat"};

// How often to resync the time (under normal and error conditions)
#define _resyncSeconds 60
#define _resyncErrorSeconds 15
#define _millisMinute 60000

// forward declarations
void connectToWifi();
void printDigits(int digits);
void digitalClockDisplay();
time_t getNTPTime();

//-----------------------------------------------------------------------------
// SETUP     SETUP     SETUP     SETUP     SETUP     SETUP     SETUP     SETUP
//-----------------------------------------------------------------------------
void setup() {
	// Open serial communications and wait for port to open:
	Serial.begin(76800);

	// Connect to your local wifi (one time operation)
	connectToWifi();

	// What port will the UDP/NTP packet respond on?
	Udp.begin(localPort);

	// What is the function that gets the time (in ms since 01/01/1900)?
	setSyncProvider(getNTPTime);

	// How often should we synchronise the time on this machine (in seconds)?
	// Use 300 for 5 minutes but once an hour (3600) is more than enough usually
	setSyncInterval(_resyncSeconds); // just for demo purposes!

	// Built in LED
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
	delay(500);

	// turn off LED
	digitalWrite(LED_BUILTIN,HIGH);
}

//-----------------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
//-----------------------------------------------------------------------------
void loop() {

	// This just prints the "system time"
	digitalClockDisplay();

	// Do other stuff etc, delay here just emulates that
	delay(0.5 * _millisMinute); // mins * millis in 1 min
}

//-----------------------------------------------------------------------------
// Prints a nice time display
//-----------------------------------------------------------------------------
void digitalClockDisplay() {
	// We'll grab the time so it doesn't change whilst we're printing it
	time_t t=now();

	//Now print all the elements of the time secure that it won't change under our feet
	printDigits(hour(t));
	Serial.print(":");
	printDigits(minute(t));
	Serial.print(":");
	printDigits(second(t));
	Serial.print("    ");
	Serial.print(DoW[weekday(t)-1]);
	Serial.print(" ");
	printDigits(day(t));
	Serial.print("/");
	printDigits(month(t));
	Serial.print("/");
	printDigits(year(t));
	Serial.println();
}

void printDigits(int digits) {
	// utility for digital clock display: prints leading 0
	if (digits < 10) Serial.print('0');
	Serial.print(digits);
}

//-----------------------------------------------------------------------------
// This is the function to contact the NTP pool and retrieve the time
//-----------------------------------------------------------------------------
time_t getNTPTime() {
	// turn off LED
	digitalWrite(LED_BUILTIN,HIGH);

	// Send a UDP packet to the NTP pool address
	Serial.print("\nSending NTP packet to ");Serial.println(timeServer);
	sendNTPpacket(timeServer);

	// Wait to see if a reply is available - timeout after X seconds. At least
	// this way we exit the 'delay' as soon as we have a UDP packet to process
	#define UDPtimeoutSecs 5
	int timeOutCnt = 0;
	while (Udp.parsePacket() == 0 && ++timeOutCnt < (UDPtimeoutSecs * 10)){
		delay(100);
		yield();
	}

	// Is there UDP data present to be processed? Sneak a peek!
	if (Udp.peek() != -1) {
		// We've received a packet, read the data from it
		Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

		// The time-stamp starts at byte 40 of the received packet and is four bytes,
		// or two words, long. First, extract the two words:
		unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
		unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

		// combine the four bytes (two words) into a long integer
		// this is NTP time (seconds since Jan 1 1900)
		unsigned long secsSince1900 = highWord << 16 | lowWord;
		Serial.print("Seconds since Jan 1 1900 = ");
		Serial.println(secsSince1900);

		// now convert NTP time into everyday time:
		//Serial.print("Unix time = ");

		// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
		const unsigned long seventyYears = 2208988800UL;

		// subtract seventy years:
		unsigned long epoch = secsSince1900 - seventyYears;

		// Reset the interval to get the time from NTP server in case we previously changed it
		setSyncInterval(_resyncSeconds);

		// LED indication that all is well
		digitalWrite(LED_BUILTIN,LOW);

		return epoch;
	}

	// Failed to get an NTP/UDP response
	Serial.println("No response");
	setSyncInterval(_resyncErrorSeconds);

	return 0;
}

//-----------------------------------------------------------------------------
// send an NTP request to the time server at the given address
//-----------------------------------------------------------------------------
void sendNTPpacket(const char* address) {
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;

	// all NTP fields have been given values, now you can send a packet requesting a timestamp:
	// Note that Udp.begin will request automatic translation (via a DNS server) from a
	// name (eg pool.ntp.org) to an IP address. Never use a specific IP address yourself,
	// let the DNS give back a random server IP address
	Udp.beginPacket(address, 123); //NTP requests are to port 123

	// Get the data back
	Udp.write(packetBuffer, NTP_PACKET_SIZE);

	// All done, the underlying buffer is now updated
	Udp.endPacket();
}

// -----------------------------------------------------------------------
// Establish a WiFi connection with your router
// -----------------------------------------------------------------------
void connectToWifi() {
	Serial.print("Connecting to: ");Serial.println(_ssid);
	WiFi.begin(_ssid, _password);

	// Try to connect 4 times a second for X seconds before timing out
	int timeout = WifiTimeOutSeconds * 4;
	while (WiFi.status() != WL_CONNECTED && (timeout-- > 0)) {
		delay(250);
		Serial.print(".");
	}

	// Successful connection?
	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("\nFailed to connect, exiting");
		// Set some LED failure here, for example
		delay(1000);
		return;
	}

	Serial.print("\nWiFi connected with (local) IP address of: ");
	Serial.println(WiFi.localIP());
}
