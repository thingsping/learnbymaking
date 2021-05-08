from machine import Pin, ADC
from WifiModCloud import WifiModCloud
import utime

ir = Pin(7, Pin.IN)
led = Pin(25, Pin.OUT)
relay1 = Pin(11, Pin.OUT)
relay2 = Pin(12, Pin.OUT)
relay1.high()
relay2.high()
buzz = Pin(14, Pin.OUT)
ldr = ADC(26)
wmc = WifiModCloud(True)
ip = wmc.connect_wifi(ssid="myssid", password="myssidpassword")
print("Connected. IP={}".format(ip))
wmc.setdb_to_firebase(host="courseinabox-b1d6f.firebaseio.com", auth="SjhN2wfl35lFxIhgZLRPLduZuVJ3w6GQoZR9lRyL",
                tree="touchless_entry")
print("Succesfully set DB to Firebase")

bzstarttime = 0
fbreported = True
mode = None

def irhandler(pin):
    global bzstarttime, buzz, fbreported
    if (pin.value() == 0 and bzstarttime == 0):
        buzz.on()
        fbreported = False
        bzstarttime = utime.ticks_ms() 
    
    
ir.irq(handler=irhandler, trigger=Pin.IRQ_FALLING|Pin.IRQ_RISING)

def set_light_auto():
    val = ldr.read_u16()
    if val <= 32000 :
        relay1.low()
        set_db_helper("lightstate", "ON")
    else :
        relay1.high()
        set_db_helper("lightstate", "OFF")

def set_db_helper(key, value):
    try:
        wmc.set_value(key , value)
    except Exception as e:
        print("Exception - {} while communicating with remote database".format(e))

while True :
    if bzstarttime != 0 and (utime.ticks_ms() - bzstarttime) > 5000 :    
        bzstarttime = 0
        buzz.off()
        
    if not fbreported :        
        fbreported = True
        set_db_helper("doorbellstate", "ON")
        
    try :
        mode = wmc.get_value("mode", str)
        print("Mode={}".format(mode))
    except Exception as e:
        print("Exception - {} while communicating with remote database".format(e))
        continue
    
    if mode == "force_on":
        relay1.low()
        set_db_helper("lightstate", "ON")
    elif mode == "force_off" : 
        relay1.high()
        set_db_helper("lightstate", "OFF")
    elif mode == "auto" :
        set_light_auto()
    else : 
        print("Unknown value for mode - {}".format(mode))
        led.toggle()
    utime.sleep(5) #
