#define door 8
#define class405 9
#define class406 11
/*
  WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi module (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 9.

 If the IP address of your board is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and UNO WiFi Rev.2)
 * LED attached to pin 9

 created 25 Nov 2012
 by Tom Igoe
 */
#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)와이파이 이름
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP) 와이파이 비번
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiClient client;//내 와이파이에 접근한 고객(아마도 웹사이트가 client가 되는 것 같음)
WiFiServer server(80);

void setup() {
  Serial.begin(9600);      // initialize serial communication
  pinMode(class405, OUTPUT);      // set the LED pin mode
  pinMode(class406,OUTPUT);
  pinMode(door,OUTPUT);
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

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);//와이파이를 연다.
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80 웹서버를 연다.
  printWifiStatus();                        // you're connected now, so print out the status
}


void loop() {
   client = server.available();   // listen for incoming clients 서버에 오는 client를 받는다.

  if (client) {                             // if you get a client,클라이언트는 EZLock사이트가 된다. 
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then EZLock 사이트에서 이벤트가 발생할시 전달하는 정보를 읽는다.

        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
             //and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            readstatus();
            // the content of the HTTP response follows the header:

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // "/n"말고 다른 어떤 char이면 currentLine에 추가되도록 
        }

        // 클라이언트가 전송한 정보가 동일할시 전구와,문이 제어 된다.
        if(currentLine.endsWith("GET /?pin=405HIGH")){
          digitalWrite(class405,HIGH);
          
        }
        if(currentLine.endsWith("GET /?pin=405LOW")){
          digitalWrite(class405,LOW);
         
        }
         if(currentLine.endsWith("GET /?pin=406DHIGH")){
          digitalWrite(door,HIGH); //문잠금
   
        }
         if(currentLine.endsWith("GET /?pin=406DLOW")){
          digitalWrite(door,LOW);//문열림
          
        }
            if(currentLine.endsWith("GET /?pin=405DHIGH")){
          digitalWrite(door,HIGH); //문잠금
   
        }
         if(currentLine.endsWith("GET /?pin=405DLOW")){
          digitalWrite(door,LOW);//문열림
          
        }
        if(currentLine.endsWith("GET /?pin=406HIGH")){//406호 불켜기
          digitalWrite(class406,HIGH);
         
        }
        if(currentLine.endsWith("GET /?pin=406LOW")){//406호 불끄기
          digitalWrite(class406,LOW);
       
        }
          if(currentLine.endsWith("GET /?pin=all")){//shut down 누르면
          digitalWrite(class405,LOW);
          digitalWrite(class406,LOW);
          digitalWrite(door,HIGH);//잠금
          readstatus();
        }
        
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
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
void readstatus(){//각 호실의 전구와 문의 상태가 어떤지 알려주는 함수
            boolean D=digitalRead(door);
            boolean LED405 =digitalRead(class405);
            boolean LED406 = digitalRead(class406);
            if(D){
              client.print("<p>Door : Lock</p>");
            }else if(!D){
              client.print("<p>Door : UnLock</p>");
            }
            if(LED405){
              client.print("<p>405 light : ON</p>");
            }else if(!LED405){
             // client.print("<p>405 light : OFF</p>");
            }if(LED406){
              client.print("<p>406 light : ON</p>");
            }else if(!LED406){
              //client.print("<p>406 light : OFF</p>");
            }
}
