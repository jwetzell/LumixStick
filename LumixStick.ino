#include <M5StickC.h>
#include <WiFi.h>

#include <WebServer.h>

#include <FS.h>
#include <SPIFFS.h>
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

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

// the setup routine runs once when M5StickC starts up
void setup(){
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  Serial.begin(115200);
  
  WiFi.begin(ssid,password);
  Serial.print("WiFi Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi Connected - " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.onNotFound(handleNotFound);

  server.begin();
  
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
  updateCameraStatusIndicator();
  
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
