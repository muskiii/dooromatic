#include <Wire.h>
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "nachohub";
const char* password = "shashumbero22";

// Set web server port number to 80
WiFiServer server(9891);

// Variable to store the HTTP request
String header;

// Assign output variables to GPIO pins
const int output_D0 = D0;
const int output_D2 = D2;
const int output_LED = D4;

void setup() {
  Serial.begin(9600);
  pinMode(output_D0, OUTPUT); 
  digitalWrite(output_D0, LOW);
  pinMode(output_D2, OUTPUT); 
  digitalWrite(output_D2, LOW);
  pinMode(output_LED, OUTPUT); 
  digitalWrite(output_LED, LOW);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting- to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}
void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
           
            if (header.indexOf("GET /bell") >= 0) {
              Serial.println("open-door");
              digitalWrite(output_D2, HIGH);
              digitalWrite(output_LED, HIGH);
              delay(2500);
              digitalWrite(output_D2, LOW);
              digitalWrite(output_LED, LOW);
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
            } 

            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.flush();
    client.stop();
  }
}
