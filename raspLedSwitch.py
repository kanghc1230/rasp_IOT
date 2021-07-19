import RPi.GPIO as GPIO
import time

def led_on(pin):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(pin, GPIO.OUT)

    GPIO.output(pin, True)

def led_off(pin):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(pin, GPIO.OUT)

    GPIO.cleanup(pin)

# 버튼 등을 누를때 함수를 호출하는 형태로 알맞게 변경하여 사용하면 된다.
led_on(18) # led 점등
time.sleep(2)
led_off(18) # led 소등