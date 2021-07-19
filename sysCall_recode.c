#include <stdlib.h>
#include <stdio.h>
int main()
{   
    char buff[100];
    for (int i=0;i <5; i++){
	    sprintf (buff, "raspivid -o ~/work/video%d.h264",i+1); //1,2,3,4,5
	    system (buff); //system call
    }
    //system ("raspivid -o ~/work/video5.h264 -t 5000");
    system ("ls -al"); //system call
    
    return 0;
}
