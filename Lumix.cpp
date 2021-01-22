#include "Lumix.h"

Lumix::Lumix(String ip){
  this->ip = ip;
  connectionStatus = 0;
  deviceName = "";
  lastKeepAlive = 0;
}

void Lumix::updateDeviceName(String deviceName){
  this->deviceName = deviceName;
}

void Lumix::updateConnectionStatus(int connectionStatus){
  this->connectionStatus = connectionStatus;
}

void Lumix::updateIp(String ip){
  this->ip = ip;
}

bool Lumix::isConnected(){
  return connectionStatus == CAMERA_CONNECTED;
}

bool Lumix::isConnecting(){
  return connectionStatus == CAMERA_CONNECTING;
}

bool Lumix::isDisabled(){
  return connectionStatus == CAMERA_DISABLED;
}

void Lumix::update(){
  if((millis()-lastKeepAlive) > LUMIX_KEEPALIVE_INTERVAL && !isDisabled()){
    Serial.println("Sending Lumix Keep Alive");
    
    if(!isConnected()){
      sendCommand("?mode=accctrl&type=req_acc&value=0&value2=LumixStick");
    }
    
    sendCommand("?mode=camcmd&value=recmode");
    lastKeepAlive = millis();
  }
}

int Lumix::getConnectionStatus(){
  return connectionStatus;
}

String Lumix::getDeviceName(){
  return deviceName;
}

String Lumix::getBaseUrl(){
  return "http://" + ip + "/cam.cgi";
}

String Lumix::getCaptureUrl(){
  return "http://" + ip + "/cam.cgi?mode=camcmd&value=capture";
}

//this assumed what you are passing is the bit after cam.cgi ex. "?mode=camcmd&value=recmode"
void Lumix::sendCommand(String paramString){
  String commandUrl = "http://" + ip + "/cam.cgi" + paramString;
  Serial.println("Sending request: " + commandUrl);
  HTTPClient http;
  http.setConnectTimeout(100);
  http.begin(commandUrl.c_str());
  
  int httpResponseCode = http.GET();

  if(httpResponseCode == 200){
        String payload = http.getString();
        if(payload.indexOf("ok") >= 0){
          if(paramString.indexOf("req_acc") >= 0 && isConnecting()){
            updateDeviceName(getSplitStringIndex(payload,',',1));
            updateConnectionStatus(CAMERA_CONNECTED);
          }
        }else{
          if(isConnected()){
            updateConnectionStatus(CAMERA_CONNECTING);
          }
        }
    }
    else {
      //Some other error either way we didn't get an "ok" back from the camera
      if(isConnected()){
        updateConnectionStatus(CAMERA_CONNECTING);
      }
    }
}

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String Lumix::getSplitStringIndex(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
