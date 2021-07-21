#가능하면 공유환경 source venv2/bin/activate
#헤더파일gpio
#pip install rpi.gpio 
#헤더파일paho
#pip install paho-mqtt
#sudo git clone https://github.com/eclipse/paho.mqtt.python.git
#cd paho.mqtt.python
#sudo python setup.py install

import RPi.GPIO as GPIO
import time
import paho.mqtt.client as mqtt

LED = 12 # wp 1 

#LED 제어부분
def led_on(pin):
    GPIO.setmode(GPIO.BOARD) 
    GPIO.setup(pin, GPIO.OUT) 
    GPIO.output(pin, True)

def led_off(pin):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(pin, GPIO.OUT) 
    GPIO.cleanup(pin)

#연결 메세지 제어부분
def on_connect(client, userdata, flags, rc):  
    print("Connected with result code {0}".format(str(rc)))  
    client.subscribe("mysecondMq/LED")  #TOPIC

def on_message(client, userdata, msg):  
    print(str(msg.payload))
    if (str(msg.payload).find('1')>0):
        led_on(LED)
    if (str(msg.payload).find('0')>0):
        led_off(LED)


while True: #반복
    client = mqtt.Client("test1")  #아무거나
    client.on_connect = on_connect # 연결
    client.on_message = on_message # 메세지 수신
    client.connect('192.168.1.10', 1883,60) #connect(Hostip, port(1883), time 60)
    client.loop_forever() #클라이언트 유지  