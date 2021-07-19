#include <stdio.h>
#include <wiringPi.h>
#define PIN_NUM 4 // wiringPi넘버5는. python에서18 

int main(void){
    if(wiringPiSetup() == -1){
        return -1;
    }
    pinMode(PIN_NUM,OUTPUT);

    while(1){
        digitalWrite(PIN_NUM, 1);
        delay(500);
        digitalWrite(PIN_NUM, 0);
        delay(500);
    }
    return 0;    
}
// C 컴파일방법
// $sudo gcc -o 파일명 파일명.c -lwiringPi