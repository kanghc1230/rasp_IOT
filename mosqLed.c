#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#define PIN_NUM 1 // wiringPi넘버1

/* Callback called when the client receives a CONNACK message from the broker. */
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


/* Callback called when the broker sends a SUBACK in response to a SUBSCRIBE. */
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


/* Callback called when the client receives a message. */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
	if(!strcmp( ((char *)msg->payload),"1"))
		digitalWrite(PIN_NUM, 1);
	if(!strcmp( ((char *)msg->payload),"0"))
		digitalWrite(PIN_NUM, 0);
}


int main(int argc, char *argv[])
{
	if(wiringPiSetup() == -1){
        return -1;
    }
	pinMode(PIN_NUM,OUTPUT);
	
	struct mosquitto *mosq;
	int rc;

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, true, NULL);
	if(mosq == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}
	
	/* Configure callbacks. This should be done before connecting ideally. */
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_subscribe_callback_set(mosq, on_subscribe);
	mosquitto_message_callback_set(mosq, on_message);

	rc = mosquitto_connect(mosq, "192.168.1.10", 1883, 60);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

	mosquitto_loop_forever(mosq, -1, 1);

	mosquitto_lib_cleanup();
	return 0;
}

