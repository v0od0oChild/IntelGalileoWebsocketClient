#ifndef WSCLIENT_H_
#define WSCLIENT_H_

#include <Ethernet.h>
#include <string.h>

typedef enum {CLOSED, CONNECTING, OPEN} Status;

class WSClient{
  public:
    void connect(const char host[], const int port = 80, const char path[] = "/");
    typedef void (*DataArrivedDelegate)(WSClient client, String data);
    void setMessageHandler(DataArrivedDelegate dataArrivedDelegate);
    void disconnect();
    bool connected();
    void send(char data[]);
    void listen();
  private:
    bool handshake(const char host[], const int port, const char path[]);
    String readLine();
    EthernetClient socket;
    Status ready_status = CLOSED;
    DataArrivedDelegate _dataArrivedDelegate;
};
#endif
