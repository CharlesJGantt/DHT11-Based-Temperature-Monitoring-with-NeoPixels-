//This code uses a DHT11 Digital Temperature and Humidity sensor and reports back the temperature of the room as well as the humidity to a website. At the same time it utilizes a strip of NeoPixel LED's to indicate whether or not the room is warm or cool. 
// Portions of this code were written by Charles Gantt, Curtis Gauger, and the original (unknown) of the basic DHT code example. 
// This code is released under the Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0) License (http://creativecommons.org/licenses/by-sa/4.0/)

#include <SPI.h>  //We will use the SPI library to allow our program to communicate over ethernet via the serial bus.
#include <Ethernet.h>  //The Ethernet library will allow us to easily use the Ethernet shield
#include <FastLED.h>  //The FastLED library will allow the Arduino to send SPI commands to the NeoPixel strip (Download at http://www.http://fastled.io/)
#include <DHT.h>  //The DHT library will allow us to easily access the data that the DHT11 sensor is outputting (Download at https://github.com/adafruit/DHT-sensor-library)

// Setup the DHT Sensor
#define DHTPIN 2     // what pin we're connected to

#define DHTTYPE DHT11   // Defines what model of DHT Sensor we are using, in this case a DHT11

DHT dht(DHTPIN, DHTTYPE);

//Setup the NeoPixel Strip
#define NUM_LEDS 7 // How many leds in your strip?
#define DATA_PIN 6 // What pin is the NeoPixel's data line connected to?

CRGB leds[NUM_LEDS]; // Define the array of leds

//Setup the Ethernet Shield
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  0x90, 0xA2, 0xDA, 0x00, 0x23, 0x36 }; //MAC address found on the back of your ethernet shield (Use this default MAC if your shield does not specify one
IPAddress ip(10,10,1,177); // IP address dependent upon your network addresses. (Typically this will look like 192.168.1.177)

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
// Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
     ; // wait for serial port to connect. Needed for Leonardo only
  }

  dht.begin(); //Initalize the DHT11 sensor. 

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}


void loop() {
 
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C");
  }
 
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
   client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
        
          // output the value of the DHT-11
         client.println("<H2>");
            client.print("Humidity: ");
            client.println("</H2>");
            client.println("<p />");
            client.println("<H1>");
            client.print(h);
            client.print(" %\t");
            client.println("</H1>");
            client.println("<p />"); 
            client.println("<H2>");
            client.print("Temperature: ");
            client.println("</H2>");
            client.println("<H1>");
            client.print(t*1.8+32);
            client.println(" &#176;");
            client.println("F");
            client.println("</H1>");
           
                
         
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
  
  //This sets the NeoPixel's color based on the temperature of the room. Note that the temperature is in degrees celcius and not ferienheight. 
  // This if statement sets the NeoPixel's color to Blue if the temperature is less than or equal to 23.66C (74.5F)
 if (t <= 23.66) {fill_solid( &(leds[0]), NUM_LEDS /*number of leds*/, CRGB::Blue);
     FastLED.show();
  }
  //This else statement sets the NeoPixel's color to red if the temperature is above 23.66C (74.5fF)
  else {
    fill_solid( &(leds[0]), NUM_LEDS /*number of leds*/, CRGB::Red);
    FastLED.show();
  }
    
}
