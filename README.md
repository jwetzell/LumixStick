# LumixStick
Lumix Camera Control for M5StickC

## Use
* Edit sketch with access point information and camera IP (can be changed later)
* If you are using an M5StickC-Plus then change the first line to `#include <M5StickCPlus.h>`
* Flash sketch and restart M5StickC
* Once M5StickC is connected to WiFi the circular icon will indicate camera connection status.
  * Red = Disabled
  * Yellow = Connecting
  * Green = Conencted
* The B button (side button) will enable/disable remote
* Once connected the A button will send the capture command to the camera
* Navigate to the IP Address of the M5StickC in a browser to change the IP of the camera

## TODO
* Make IP Address changes persistant
* More UI things
  * Camera Info
  * Settings (Cam IP, Stick IP)
