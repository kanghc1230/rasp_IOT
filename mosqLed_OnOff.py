import RPi.GPIO as GPIO
import time
import paho.mqtt.client as mqtt

LED = 12 # wp 1

def led_on(pin):
    GPIO.setmode(GPIO.BOARD) #모드를 psy no
    GPIO.setup(pin, GPIO.OUT) #out로셋업
    GPIO.output(pin, True)

def led_off(pin):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(pin, GPIO.OUT) 
    GPIO.cleanup(pin)


def on_connect(client, userdata, flags, rc):  
    print("Connected with result code {0}".format(str(rc)))  
    client.subscribe("mysecondMq")  

def on_message(client, userdata, msg):  
    print(str(msg.payload))
    if (str(msg.payload).find('LED ON')>0):
        led_on(LED)
    if (str(msg.payload).find('LED OFF')>0):
        led_off(LED)
while True:
    client = mqtt.Client("test1")  
    client.on_connect = on_connect  
    client.on_message = on_message 
    client.connect('192.168.1.10', 1883,60)
    client.loop_forever()  