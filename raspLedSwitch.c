//https://hoho325.tistory.com/216
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
        if(digitalRead(SWITCH) == HIGH){ //스위치가 눌리면
            //printf("Button was pressed\n");
            digitalWrite(LED, HIGH); //led밝기조절
        }
        else{
            //printf("Button was NOT pressed\n");
            digitalWrite(LED, LOW);
        }
        //delay(1000);
    }
    return 0;
}
