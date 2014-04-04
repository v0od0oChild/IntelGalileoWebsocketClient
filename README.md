IntelGalileoWebsocketClient
==============================

Websocket client ported for [INTEL GALILEO](http://arduino.cc/en/ArduinoCertified/IntelGalileo) boards.

Install the library to "libraries" folder in your Sketchbook folder. 

This library is general purpose but was made to support the [Muzzley Galileo Arduino Library](https://github.com/muzzley/muzzleyConnectorArduinoGalileo). Check [Muzzley](http://www.muzzley.com) for more details.


### Usage Example
``` arduino
#include <Ethernet.h>
#include <WSClient.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
WSClient client;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac);
  client.connect("echo.websocket.org", 80, "/");
  addEventListener("on_message", new Delegate<void, char*>(&onMessage));
  addEventListener("on_close", new Delegate<void, char*>(&onConnectionClose));
  client.send("Hello world");
}

void loop() {
  delay(50);
  client.listen();
}

void onConnectionClose(char* msg){
  Serial.println("Connection closed");
  Serial.println("");
}

void onMessage(char* msg) {
  Serial.println("--- Got message ----");
  Serial.println(msg);
}

```


### Credits
Special thanks to:
  - krohling [ArduinoWebsocketClient](https://github.com/krohling/ArduinoWebsocketClient)
  - djsb [arduino-websocketclient](https://github.com/djsb/arduino-websocketclient)