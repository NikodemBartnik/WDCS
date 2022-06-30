from machine import Pin, ADC
import time

pump1 = Pin(0, Pin.OUT)
pump2 = Pin(1, Pin.OUT)

ledR1 = Pin(2, Pin.OUT)
ledR2 = Pin(3, Pin.OUT)
ledR3 = Pin(4, Pin.OUT)
ledR4 = Pin(5, Pin.OUT)
button = Pin(6, Pin.IN, Pin.PULL_DOWN)


while(1):
    if button.value() == True:
        ledR1.toggle()
        time.sleep(1)

    