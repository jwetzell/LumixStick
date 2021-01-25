#include <M5StickC.h> //change this line to #include <M5StickCPlus.h> if you are using the M5Stick-C Plus
#include <WiFi.h>
#include <WebServer.h>
#include "Lumix.h"

// Put your wifi SSID and password here
const char* ssid = "ssid";
const char* password = "password";

WebServer server(80);

Lumix camera = Lumix("192.168.54.1");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>LumixStick</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/save">
    Camera IP: <input type="text" name="ip">
    <input type="submit" value="Save">
  </form>
</body></html>)rawliteral";


void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleSave(){
  if(server.arg("ip") == ""){
    server.send(200,"text/plain", "No IP Param....no update made");
  }else{
    String response = "Camera IP Updated to " + server.arg("ip");
    camera.updateIp(server.arg("ip"));
    server.send(200, "text/plain", response);
  }
}

// the setup routine runs once when M5StickC starts up
void setup(){
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  updateCameraStatusIndicator();
  
  Serial.begin(115200);
  
  WiFi.begin(ssid,password);
  Serial.print("WiFi Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
    drawNetworkIcon(TFT_BLACK);
    delay(250);
    drawNetworkIcon(TFT_WHITE);
  }
  Serial.println();
  Serial.println("WiFi Connected - " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/save", handleSave);

  server.begin();
  
}

void drawNetworkIcon(uint32_t color){
  M5.Lcd.drawLine(4, 16, 4, 14, color);
  M5.Lcd.drawLine(7, 16, 7, 12, color);
  M5.Lcd.drawLine(10, 16, 10, 10, color);
  M5.Lcd.drawLine(13, 16, 13, 8, color);
  M5.Lcd.drawLine(16, 16, 16, 6, color);
}

void updateCameraStatusIndicator(){
  
  int circleRadius = 7;
  int padding = 4;

  switch (camera.getConnectionStatus()) {
    case CAMERA_DISABLED:
      M5.Lcd.fillCircle(M5.Lcd.width() - circleRadius - padding, circleRadius + padding, circleRadius, TFT_RED);
      break;
    case CAMERA_CONNECTING:
      M5.Lcd.fillCircle(M5.Lcd.width() - circleRadius - padding, circleRadius + padding, circleRadius, TFT_YELLOW);
      break;
    case CAMERA_CONNECTED:
      M5.Lcd.fillCircle(M5.Lcd.width() - circleRadius - padding, circleRadius + padding, circleRadius, TFT_GREEN);
      break;
    default:
      // statements
      break;
  }
}

void loop() {
  M5.update();
  server.handleClient();
  camera.update();
  
  if(camera.infoUpdated()){
    Serial.println("Camera Info Changed");
    updateCameraStatusIndicator();

    //blank out name area
    M5.Lcd.fillRect(0, (M5.Lcd.height() / 4) - (M5.Lcd.height() / 8), M5.Lcd.width(), M5.Lcd.height()/4, TFT_BLACK);
    int datumPrevious = M5.Lcd.getTextDatum();
    M5.Lcd.setTextDatum(MC_DATUM);
    //draw name or ip if no name is set
    M5.Lcd.drawString(camera.getFriendlyName(), M5.Lcd.width() / 2, M5.Lcd.height() / 4);
    M5.Lcd.setTextDatum(datumPrevious);
  }
  
  if(M5.BtnB.wasPressed()){
    if(!camera.isDisabled()){
      camera.updateConnectionStatus(CAMERA_DISABLED);
    }else{
      camera.updateConnectionStatus(CAMERA_CONNECTING);
    }
  }

  if(M5.BtnA.wasPressed() && camera.isConnected()){
    Serial.println("Sending Lumix Capture Command");
    camera.sendCommand("?mode=camcmd&value=capture");
  }
}
