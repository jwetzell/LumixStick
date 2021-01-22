#define CAMERA_DISABLED 0
#define CAMERA_CONNECTING 1
#define CAMERA_CONNECTED 2

#define LUMIX_KEEPALIVE_INTERVAL 2000

#include <Arduino.h>
#include <HTTPClient.h>

class Lumix {
  private:
    String ip;
    int connectionStatus;
    String deviceName;
    unsigned long lastKeepAlive;

  public:
    Lumix(String ip);

    void updateDeviceName(String deviceName);
    void updateConnectionStatus(int connectionStatus);
    void updateIp(String ip);

    bool isConnected();
    bool isConnecting();
    bool isDisabled();

    void update();
    
    int getConnectionStatus();
    String getDeviceName();

    String getBaseUrl();
    String getCaptureUrl();

    void sendCommand(String paramString);


    String getSplitStringIndex(String data, char separator, int index);
};
