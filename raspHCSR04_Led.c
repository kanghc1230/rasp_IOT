#include <stdio.h>
#include <wiringPi.h>

#define LED 29 // WP GPIO 29

int main(void) {
    float dist, s, e;
 
    wiringPiSetup();
 
    pinMode(0, OUTPUT);
    pinMode(1, INPUT);

    if(wiringPiSetup() == -1)
        return -1;
    pinMode(LED, OUTPUT);
 
    while (1) {
        digitalWrite(0, 0);
        digitalWrite(0, 1);
        delayMicroseconds(10);
        digitalWrite(0, 0);
        while(digitalRead(1) == 0)
            s = micros();
        while(digitalRead(1) == 1)
            e = micros();
        dist = (e - s) / 58;
        if (dist < 10){
            printf("distance(cm) : LED ON %f\n", dist);
            digitalWrite(LED, HIGH);
        }
        else if (dist >= 10){
            printf("distance(cm) : LED OFF %f\n", dist);
            digitalWrite(LED, LOW);
        }
        delay(100);
    }
    return 0;
}
