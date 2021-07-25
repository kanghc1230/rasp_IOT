#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>

#define PIN_NUM 1 // wiringPi넘버1 12(boaad)

/* Callback called connect 클라이언트 sub로 "토픽"을 이어주고, 에러를 클라로부터 브로커가 받을때 */
void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	int rc;

	printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
	if(reason_code != 0){
		mosquitto_disconnect(mosq);
	}

	rc = mosquitto_subscribe(mosq, NULL, "mysecondMq/LED", 1);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
		mosquitto_disconnect(mosq);
	}
}

/* Callback called SUBSCRIBER로 부터 에러를 받고 브로커에게 메세지 전달때  */
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
	int i;
	bool have_subscription = false;

	for(i=0; i<qos_count; i++){
		printf("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
		if(granted_qos[i] <= 2){
			have_subscription = true;
		}
	}
	if(have_subscription == false){
		fprintf(stderr, "Error: All subscriptions rejected.\n");
		mosquitto_disconnect(mosq);
	}
}

/* Callback called 클라이언트로부터 메세지를 받을떄 */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload); // 프로세스 시스템에 출력
	if(!strcmp( ((char *)msg->payload),"1")) // 온메세지가 1이라면
		digitalWrite(PIN_NUM, 1); // led 켜기
	if(!strcmp( ((char *)msg->payload),"0")) // 온메세지가 0이라면
		digitalWrite(PIN_NUM, 0); // led 끄기
}


int main(int argc, char *argv[])
{
	//셋업
	if(wiringPiSetup() == -1){
        return -1;
    }
	//led output
	pinMode(PIN_NUM,OUTPUT);
	
	struct mosquitto *mosq; // mqtt데이터가 들어갈 구조체 선언 
	int rc; //에러처리변수

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, true, NULL);
	if(mosq == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}
	
	/* 함수들이 작동, 연결할때마다 콜백해주는 함수*/
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_subscribe_callback_set(mosq, on_subscribe);
	mosquitto_message_callback_set(mosq, on_message);

	rc = mosquitto_connect(mosq, "192.168.1.10", 1883, 60); //아이피입력
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

	//루프
	mosquitto_loop_forever(mosq, -1, 1);

	mosquitto_lib_cleanup();
	return 0;
}

