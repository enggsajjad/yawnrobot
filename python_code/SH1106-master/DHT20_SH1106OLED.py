# Read DHT20 and
# show on i2c-OLED 1,3" with SH1106
# Autor:   Joern Weise
# License: GNU GPl 3.0
# Created: 23. Oct 2021
# Modified: 26. Oct 2022 by Bernd Albrecht
import sh1106
#from dht20 import DHT20
from machine import Pin, I2C
import utime
sda=Pin(16)
scl=Pin(17)
i2c=I2C(0, sda=sda, scl=scl, freq=400000)
WIDTH = 128
HIGHT = 64
oled = sh1106.SH1106_I2C(128, 64, i2c, None, 0x3c,rotate=180)
#dht20 = DHT20(0x38, i2c)           # 0x38 ist die i2c-Adresse
i = 0
while True:
    #measurements = dht20.measurements
    tempC = 23#measurements['t']
    humRH = 45#measurements['rh']
    print("Temperature: " + str(tempC))
    print("Humidty: " + str(humRH))
    print("Counter: " + str(i))
    print(">-----------<")
    #Write data to display
    oled.fill(0)
    oled.text("Pico W, DHT20 ",6,0)
    oled.text("Temp:" + str(tempC),6,14)
    oled.text("Humi:" + str(humRH),6,42)
    oled.text("Counter:" + str(i),6,56)
    oled.show()
    i+=1
    utime.sleep(2) #Sleep for two seconds
