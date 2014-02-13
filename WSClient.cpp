#include <WSClient.h>
#include <string.h>
#include <Base64.h>


// Private methods

/** Reads the next line from the websocket  */
String WSClient::readLine(){
  String line = "";
  char character;

  while(socket.available() > 0 && (character = socket.read()) != '\n') {
        if (character != '\r' && character != -1) {
            line += character;
        }
  }
 return line;
}


/** Handshake Process */
bool WSClient::handshake(const char host[], const int port, const char path[]){
  
  char key[]="------------------------";
  char keyStart[17];
  char b64Key[25];
  
  for (int i=0; i<16; ++i){
    key[i] = (char) 1+(random()%(256-1));
  }

  base64_encode(b64Key, keyStart, 16);

  for (int i=0; i<24; ++i) {
    key[i] = b64Key[i];
  }
  
  socket.print("GET ");
  socket.print(path);
  socket.print(" HTTP/1.1");
  socket.println("");
  socket.print("Host: ");
  socket.print(host);
  socket.print(":");
  socket.print(port);
  socket.println("");
  socket.println("Upgrade: websocket");
  socket.println("Connection: Upgrade");
  socket.println("Origin: Galileo");
  socket.print("Sec-WebSocket-Key: ");
  socket.print(key);
  socket.println("");
  socket.println("Sec-WebSocket-Version: 13");
  socket.println("");

  bool result = false;
  char character;
  String handshake = "", line, server_key;
  int maxAttempts = 300, attempts = 0;
    
  while(socket.available() == 0 && attempts < maxAttempts){ 
    delay(100); 
    attempts++;
  }
    
  while((line = readLine()) != "") {
            
    if(line.indexOf("Sec-WebSocket-Accept:") >= 0){
      server_key = line.substring(22);
      result = true;
    }
    handshake += line;
    handshake += '\n';
  }
  return result;

  //TODO
  //Check Server key
}



// Public methods

/**  Connects and handshakes with remote server */
void WSClient::connect(const char host[], const int port, const char path[]){
  ready_status = CONNECTING; 
  if (socket.connect(host, port)){
    if(handshake(host, port, path)){
      ready_status = OPEN;
    }else{
      socket.stop();
      ready_status = CLOSED;
    }
  }
}

/** Gracefully disconnect from server */
void WSClient::disconnect(){
  // Should send 0x8700 to server to tell it I'm quitting here.
  socket.write((uint8_t) 0x87);
  socket.write((uint8_t) 0x00);
  socket.flush();
  delay(20);
  socket.stop();
}

/** Checks if the connection is open  */
bool WSClient::connected(){
  bool connected = false;
  if(ready_status == OPEN) connected = true;
  return connected;
}


/** Sends a message to the websocket server */
void WSClient::send(char data[]){   
  int size = strlen(data);
   
  // string type
  socket.write(0x81);

  // NOTE: no support for > 16-bit sized messages
  if (size > 125){
    socket.write(127);
    socket.write((uint8_t) (size >> 56) & 255);
    socket.write((uint8_t) (size >> 48) & 255);
    socket.write((uint8_t) (size >> 40) & 255);
    socket.write((uint8_t) (size >> 32) & 255);
    socket.write((uint8_t) (size >> 24) & 255);
    socket.write((uint8_t) (size >> 16) & 255);
    socket.write((uint8_t) (size >> 8) & 255);
    socket.write((uint8_t) (size ) & 255);
  } else {
    socket.write((uint8_t) size);
  }

  for (int i=0; i<size; ++i){
    socket.write(data[i]);
  }
}


/** Set the function to handle the received messages  */
void WSClient::setMessageHandler(DataArrivedDelegate dataArrivedDelegate){
  _dataArrivedDelegate = dataArrivedDelegate;
}

/** listen to incoming messages on the websocket  */
void WSClient::listen(){
  String data;
  int int_readed;

  if(socket.available() && (int_readed = socket.read()) != 129){
    while(socket.available() && (int_readed = socket.read()) != 129){
      data += char(int_readed);
    }
    if (_dataArrivedDelegate != NULL) {
      _dataArrivedDelegate(*this, data);
    }
  }
}