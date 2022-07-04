from machine import Pin, ADC, Timer
import time, os

#constants
PIN_PUMP_1 = 15
PIN_PUMP_2 = 14
PIN_LED_R_1 = 2
PIN_LED_R_2 = 3
PIN_LED_R_3 = 4
PIN_LED_R_4 = 5
PIN_LED_G_1 = 18
PIN_LED_G_2 = 19
PIN_LED_G_3 = 20
PIN_LED_G_4 = 21

PIN_BUTTON_1 = 6
PIN_BUTTON_2 = 17

WATERING_CYCLE_TIMES_ARRAY = [2, 4, 6, 8] #time in minutes

#global variables
selectionActive = True
dailyWateringCycles = 3 
wateringCycleTime = 1 #choose from WATERING_CYCLE_TIMES
selectionTimer = Timer()

pump1 = Pin(PIN_PUMP_1, Pin.OUT)
pump2 = Pin(PIN_PUMP_2, Pin.OUT)

ledR1 = Pin(PIN_LED_R_1, Pin.OUT)
ledR2 = Pin(PIN_LED_R_2, Pin.OUT)
ledR3 = Pin(PIN_LED_R_3, Pin.OUT)
ledR4 = Pin(PIN_LED_R_4, Pin.OUT)
ledG1 = Pin(PIN_LED_G_1, Pin.OUT)
ledG2 = Pin(PIN_LED_G_2, Pin.OUT)
ledG3 = Pin(PIN_LED_G_3, Pin.OUT)
ledG4 = Pin(PIN_LED_G_4, Pin.OUT)

button1 = Pin(PIN_BUTTON_1, Pin.IN, Pin.PULL_DOWN)
button2 = Pin(PIN_BUTTON_2, Pin.IN, Pin.PULL_DOWN)


def checkKeys():
    global selectionActive
    if(button1.value() or button2.value()):
        selectionActive = True
        selectionTimer.init(mode=Timer.ONE_SHOT, period=10000, callback=disableSelection)
        
def disableSelection(t):
    global selectionActive
    selectionActive = False
    ledR1.value(0)
    ledR2.value(0)
    ledR3.value(0)
    ledR4.value(0)
    ledG1.value(0)
    ledG2.value(0)
    ledG3.value(0)
    ledG4.value(0)
    saveSettings()
    
    
def saveSettings():
    #saving settings
    try:
        os.remove('t.txt')
    except OSError:
        print("t.txt doesn't exists")
    try:
        os.remove('c.txt')
    except OSError:
        print("t.txt doesn't exists")
        
    f = open('t.txt', 'w+')
    f.write(str(wateringCycleTime))
    f.close()
    f = open('c.txt', 'w+')
    f.write(str(dailyWateringCycles))
    f.close()
    
def loadSettings():
    global wateringCycleTime, dailyWateringCycles
    try:
        f = open('t.txt')
        wateringCycleTime = int(f.read())
        print('wateringCycleTime', wateringCycleTime)
        f.close()
    except OSError:
        print("t.txt doesn't exists")
    try: 
        f = open('c.txt')
        dailyWateringCycles = int(f.read())
        print('dailyWateringCycles', dailyWateringCycles)
        f.close()
    except OSError:
        print("c.txt doesn't exists")

    
def displayWateringTimes():
    if(selectionActive):
        ledR4.value(1) if wateringCycleTime >= 0 else ledR4.value(0)
        ledR3.value(1) if wateringCycleTime >= 1 else ledR3.value(0)
        ledR2.value(1) if wateringCycleTime >= 2 else ledR2.value(0)
        ledR1.value(1) if wateringCycleTime >= 3 else ledR1.value(0)
        
        
def displayWateringCycles():
    if(selectionActive):
        ledG1.value(1) if dailyWateringCycles >= 0 else ledG1.value(0)
        ledG2.value(1) if dailyWateringCycles >= 1 else ledG2.value(0)
        ledG3.value(1) if dailyWateringCycles >= 2 else ledG3.value(0)
        ledG4.value(1) if dailyWateringCycles >= 3 else ledG4.value(0)
    

def checkAndModifySettings():
    global wateringCycleTime, dailyWateringCycles
    if(button1.value()):
        time.sleep_ms(50)
        if(button1.value()):
            wateringCycleTime = (wateringCycleTime+1) if wateringCycleTime < 3 else 0
            displayWateringTimes()
            time.sleep_ms(200)
    
    if(button2.value()):
        time.sleep_ms(50)
        if(button2.value()):
            dailyWateringCycles = (dailyWateringCycles+1) if dailyWateringCycles < 3 else 0
            displayWateringCycles()
            time.sleep_ms(200)
            
            

def checkWatering():
    #watering
    pass
    

#loading the settings from flash before entering the loop
loadSettings()
#disable LEDs after 10 seconds
selectionTimer.init(mode=Timer.ONE_SHOT, period=10000, callback=disableSelection)

while(1):
    checkKeys()
    checkWatering()
    if(selectionActive):
        checkAndModifySettings()
        displayWateringTimes()
        displayWateringCycles()
    time.sleep_ms(50)
    print(selectionActive)
