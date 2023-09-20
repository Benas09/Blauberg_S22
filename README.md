# Blauberg_S22

Blauberg S22 replacement controller with ESPHome

With this library you can replace the S22 controller of your air recuperator (mine is KOMFORT ERV EC DB 350) and controll it via ESPHome.

```
 Wire from                  D1 Mini
 original controller

+24V (Yellow)   ----------> 
RX (Green)      ----------> D1
TX (Bworn)      ----------> D0
GND (White)     ----------> GND
```

I used DC/DC Buck Converter LM2596 to step down +24V to +5V and power up D1 mini. 
If you want, you can instead use additional power supply, but dont forget to connect GND (Wire from original controller) -> GND (D1 Mini)
