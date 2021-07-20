import RPi.GPIO as GPIO
import time
 
LED = 12 # wp 1
SWITCH = 16 #wp 4
 
def led_on(pin):
    GPIO.setmode(GPIO.BOARD) #모드를 psy no
    GPIO.setup(pin, GPIO.OUT) #out로셋업

    GPIO.output(pin, True)

def led_off(pin):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(pin, GPIO.OUT) 

    GPIO.cleanup(pin)

# 버튼 등을 누를때 함수를 호출하는 형태로 알맞게 변경하여 사용하면 된다.
while True:
    if GPIO.input(SWITCH) == 0: # input은 눌렸을때 0값을 받아옴
        led_on(LED) # led 점등
    else:
        led_off(LED) # led 소등
