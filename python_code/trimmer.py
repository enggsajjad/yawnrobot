from machine import ADC
import time
adc = ADC(28) 
while True:
    digital_value = adc.read_u16()     
    print("ADC value=",digital_value)
    voltage_value=3.3*(digital_value/65535)
    print("Voltage: {}V ".format(voltage_value))
    time.sleep_ms(2000)