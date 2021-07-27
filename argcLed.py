#https://jinisbonusbook.tistory.com/52
import sys
import RPi.GPIO as GPIO
import time

LED_pin = 12 # board 12 == wp 1

def led_on(pin):
    GPIO.setmode(GPIO.BOARD) 
    GPIO.setwarnings(False)
    GPIO.setup(pin, GPIO.OUT) 
    GPIO.output(pin, True)
    print ("led on")

#turn off
def led_off(pin):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setwarnings(False) # RuntimeWarning: This channel is already in use, continuing anyway.  Use GPIO.setwarnings(False) to disable warnings.
    GPIO.setup(pin, GPIO.OUT) 
    GPIO.cleanup(pin) #GPIO.LOW 초기화는 한번하면끝
    print ("led off")


if len( sys.argv) != 2:
    print( "command : python3 {0} <1or0>".format(sys.argv[0]) )
    sys.exit()

if sys.argv[1] == "1":
    led_on(LED_pin)
elif sys.argv[1] == "0":
    led_off(LED_pin)
