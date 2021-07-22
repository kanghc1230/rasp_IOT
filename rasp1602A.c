//SCL(Serial Clock) : 시리얼 클럭으로 마스터와 슬레이브 간의 클럭을 맞춰 데이터를 송수신 할 수 있게 해준다. 기본 값 HIGH일 때만 SDA가 HIGH -> LOW 로 가면 START, LOW -> HIGH로 가면 END를 뜻한다. 데이터 송수신은 SCL이 LOW일 때만 가능하다.
//SDA(Serial Data) : 시리얼 데이터로 통신하고자 하는 데이터를 의미하며 위의 설명처럼 SCL이 LOW일 때 데이터를 주고받을 수 있으며 각각의 전송에는 ACK가 포함되며 데이터를 읽을 것일지 쓸 것일지에 따라 규격이 다르다.
//git clone https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library.git
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//0x27 : 0 0 0
LiquidCrystal_I2C lcd(0x27, 16, 2); //lcd address, 16chars, 2line

void setup()
{
    lcd.begin();
    lcd.backlight();
}

void loop()
{
    lcd.clear();
    lcd.pinrt("hi");
    lcd.setCursor (0,1); //go 2line
    lcd.print("Hello Wrold!");
    delay(500)
}