// https://hoho325.tistory.com/216
// 암암 4개 필요

#include <stdio.h>
#include <wiringPi.h>
 
#define LED 1
#define SWITCH 4
 
int main(){
    if(wiringPiSetup() == -1)
        return -1;
 
    // Mode 설정
    pinMode(LED, OUTPUT);
    pinMode(SWITCH, INPUT);
 
    while(1){
        if(digitalRead(SWITCH) == HIGH){
            printf("Button was pressed\n");
            digitalWrite(LED, HIGH);
        }
        else{
            printf("Button was NOT pressed\n");
            digitalWrite(LED, LOW);
        }
 
        delay(1000);
    }
    return 0;
}
