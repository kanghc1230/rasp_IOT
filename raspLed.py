# $pip3 install RPi.GPIO  비주얼스튜디오
# phy) GPIO 18, GND 14
import RPi.GPIO as GPIO
import time

def led(pin, t):
    GPIO.setmode(GPIO.BCM) #BCM번호로 하겠다.
    GPIO.setwarnings (False) #BCM False
    
    GPIO.setup(pin, GPIO.OUT)
    time.sleep(t) 
    GPIO.output(pin, True)
    #turn off
    GPIO.output(pin, GPIO.LOW) #GPIO.LOW 초기화는 한번하면끝
    #GPIO.cleanup(pin)

#func call
for i in range(0,3):
    led(24, 3) 
    time.sleep(1)

"""
# $pip3 install RPi.GPIO  비주얼스튜디오시
import RPi.GPIO as GPIO
import time

def led(pin, t):
    GPIO.setmode(GPIO.BOARD) #board번호로 하겠다. #(GPIO.BCM)이면 18,5가아니라 BCM 24
    GPIO.setup(pin, GPIO.OUT)

    GPIO.output(pin, True)
    time.sleep(t) 

    #turn off
    GPIO.cleanup(pin)

#func call
for i in range(0,5):
    led(18, 5) # BOARD 18번 핀에 끼운 LED를 5초동안 점등
    time.sleep(1)

"""
