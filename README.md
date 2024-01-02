# Blauberg_S14

Blauberg S14 replacement controller with ESPHome

With this library you can replace the S14 controller of your air recuperator (mine is KOMFORT ERV EC DB 350) and control it via ESPHome.

![](/images/home_assistant.png)
![](/images/1695192425987.jpg)


## Connection

```
 Wire from                  D1 Mini
 original controller

+24V (Yellow)   ----------> 
RX (Green)      ----------> D1
TX (Brown)      ----------> D0
GND (White)     ----------> GND
```
I used DC/DC Buck Converter LM2596 to step down +24V to +5V and power up D1 mini. 
If you want, you can instead use additional power supply, but dont forget to connect (**!!! ENSURE THAT THERE IS NO VOLTAGE BETEWEEN POWER SUPPLY GND AND RECUPERATOR GND !!!**) GND (Wire from original controller) -> GND (D1 Mini)

**Keep in mind, that your pinout can be different, so double check.**

![](/images/1695192425993.jpg)

I cut the wire and soldered USB-A socked, to be able to swap between D1 Mini and original S14 easily

![](/images/1695192425977.jpg)

## TODO
* indication about required filter replacement
* filter reset button
