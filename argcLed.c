// c commandline

#include <stdio.h>
#include <wiringPi.h>
 
#define LED 1
 
int main(int argc, char *argv[]){
    if(wiringPiSetup() == -1)
        return -1; 
 
    // Mode 설정
    pinMode(LED, OUTPUT);
 
    if (argc != 2)
    {
        printf("Command type Error\n%s <1 or 0>\n",argv[1],argv[0]);
        return 0;
    }

    if(*argv[1]==49){
            printf("Turn on Command \n");
            digitalWrite(LED, HIGH);
    }
    else if(*argv[1]==48){
        printf("Turn off Command \n");
        digitalWrite(LED, LOW);
    } 
    return 0;
}
