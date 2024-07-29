#include "WiFiS3.h"


#include "arduino_secrets.h" 


#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

const byte correnteACS712 = A0; // ACS712 connessa al pin analogico 0
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);      // initialize serial communication
  pinMode(led, OUTPUT);      // set the LED pin mode
  analogReadResolution(14); //change to 14-bit resolution
  dht.begin();
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}


void loop() {



  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
// Read current in mA from ACS712 (185mV/A)
  long I_value = (long) analogRead(correnteACS712)*(490000L/16383L)/100L;

  /*
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(" I_amp ");
  Serial.print(I_value);
  Serial.println(" mA");
*/

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out to the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println("Content-type:text/html");           
            client.println();
/*            client.print("<table>");
              client.print("<tr>");
                client.print("<td>");
                  client.print(t);          // Temp.
                client.print("</td>");
                client.print("<td>");
                  client.print(I_value);    // Corrente
                client.print("</td>");
                client.print("<td>");
                  client.print(h);          // Umidità
                client.print("</td>");      
              client.print("</tr>");
            client.print("</table>");
*/
client.print("<figure class=\"table\" style=\"width:100%;\">");
client.print(" <table class=\"ck-table-resized\">");
client.print(" <colgroup>");
client.print(" <col style=\"width:50%;\">");
client.print(" <col style=\"width:50%;\">");
client.print(" </colgroup>");
client.print(" <tbody>");
client.print(" <tr>");
client.print(" <td colspan=\"2\">");
client.print(" <p style=\"text-align:center;\"><strong>IZ0JNY 10GHz Transverter Box Monitor</strong></p>");
client.print(" <tr>");
client.print(" </tr>");
client.print(" </td>");
client.print(" <tr>");
client.print(" <td>Temperature</td>");
client.print("<td>");
client.print(t);          // Temp.
client.print("&nbsp;&degC</td>");
client.print(" </tr>");
client.print(" <tr>");
client.print(" <td>Humidity</td>"); 
client.print("<td>");
client.print(h);          // Umidità
client.print("&nbsp;%</td>");
client.print(" </tr>");
client.print(" <tr>");
client.print(" <td>SSPA Current</td>");
client.print("<td>");
client.print(I_value);    // Corrente
client.print("&nbsp;mA</td>");
client.print(" </tr>");
client.print(" </tbody>");
client.print(" </table>");
client.print("</figure>");
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

      }
      
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");


  }

 
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

 
}
