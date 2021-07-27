// 가상환경 $source venv2/bin/activate
/* $gcc -o project_mosq1602A_LED project_mosq1602A_LED.c -lmosquitto -lwiringPi -lpthread
// $./project_mosq1602A_LED
*
* 1602A 
* by Lewis Loflin www.bristolwatch.com lewis@bvu.net
* http://www.bristolwatch.com/rpi/i2clcd.htm
* Using wiringPi by Gordon Henderson
*/
// 라즈베리파이
#include <wiringPiI2C.h>
#include <wiringPi.h>
// 기본입출력
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
// mqtt
#include <mosquitto.h>
#include <unistd.h>
// sigint제어
#include <signal.h>
// 쓰레드
#include <pthread.h> 

#include <stdint.h> 

// DH11
#define MAX_TIMINGS	83
#define DHT_PIN	5	// WirPi 5 == Board(GPIO-24) 

// LED
#define PIN_NUM 1 // wiringPi넘버1

// LCD Address
#define I2C_ADDR 0x27 // I2C 디바이스 어드레스(기본0x27)

// LCD 모드 정의
#define LCD_CHR 1 // 모드 - 데이터 보내기
#define LCD_CMD 0 // 모드 - command 보내기
#define PIN_NUM 1 // LED NUM wipi 1 == board 12

#define LINE1 0x80 // 1st line
#define LINE2 0xC0 // 2nd line

#define LCD_BACKLIGHT 0x08 // On
// LCD_BACKLIGHT = 0x00  # Off
#define ENABLE 0b00000100 // Enable bit

// 0: blank, 1: me, 2: unknown
int human = 0;

// mqtt sub 
void on_connect(struct mosquitto *mosq, void *obj, int reason_code);
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos);
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

// mqtt 쓰레드 함수
void *mqtt_sub(void *data);

// lcd 데이터처리 함수
void lcd_init(void);
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);

// 단어 입력 함수
void lcdLoc(int line); // (찍는위치)커서움직이기
void ClrLcd(void);     // 초기화
void typeInt(int i);
void typeFloat(float myFloat);
void typeChar(char val); 
void typeln(const char *s); //타이핑("문장")
int fd; 

// DH11 (온도 읽어오는) 함수
float temp = 1000.0;
float humi = 1000.0;
void read_dht_data(void);

//sigint 핸들링 함수
void interruptHandler(int sig); 

// mqtt 쓰레드 함수 //
void *mqtt_sub(void *data)
{
    //MQTT_sub.c
    struct mosquitto *mosq;
	int rc;

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, true, NULL);
	if(mosq == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return (void*)0;
	}
	
	// 함수들 콜백
	mosquitto_connect_callback_set(mosq, on_connect); // 연결확인
	mosquitto_subscribe_callback_set(mosq, on_subscribe); // Sub
	mosquitto_message_callback_set(mosq, on_message); // 메세지수신

    // 연결
	rc = mosquitto_connect(mosq, "192.168.1.10", 1883, 60); // 본인아이피
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return (void*)0;
	}
    //반복
    usleep(10000);
    mosquitto_loop_forever(mosq, -1, 1);
    mosquitto_lib_cleanup();
}

// 메인 함수 //
int main()
{
    //종료시그인트 cntl+c
    signal(SIGINT, interruptHandler);

    //mqtt처리쓰레드 작동
    pthread_t p_thread; 
    int status;
    pthread_create(&p_thread, NULL, mqtt_sub, (void*)0); // 쓰레드생성 함수실행
    pthread_detach(p_thread); // 쓰레드 자동 join
    sleep(1); //쓰레드실행대기

    //IOT LED,LCD 작동
    //setup
    if (wiringPiSetup() == -1)
        exit(1);

    fd = wiringPiI2CSetup(I2C_ADDR);

    //led
    pinMode(PIN_NUM, OUTPUT); //LED output
    //lcd
    lcd_init(); // setup LCD

    char array1[] = "airconditional on";

    int home_led = 0;

    while (1)
    {
        // 0 입력받을시 불끄기
        if (human == 0 && home_led == 1){
            digitalWrite(PIN_NUM, 0);
            home_led = 0;
            ClrLcd();
        }

        else if (human == 1)
        {
            // 전부지우기
            ClrLcd();
            // 기본입력방법
            lcdLoc(LINE1);  // 커서 첫번째줄
            typeln("ME,");  // 스트링입력
            lcdLoc(LINE2);  // 커서 두번째줄
            typeln("Welcome!"); // 스트링입력
            delay(5000);    // 표시중 딜레이

            read_dht_data(); // temp, humi에 한번읽기

            if (home_led == 0) //홈 led(iot들이)가 꺼져있는가
            {
                ClrLcd();
                home_led = 1; //홈 led 키기
                lcdLoc(LINE1);
                typeln("LED ON.");
                digitalWrite(PIN_NUM, 1); // LED 작동
                delay(3000);
            }

            // 라인에 int value 입력방법 typeInt(int);
            // 라인에 float value 입력방법 typeFloat(float);
            ClrLcd();
            lcdLoc(LINE1);
            typeln("temp : ");
            typeFloat(temp);
            typeln(" C");
            delay(2000);

            lcdLoc(LINE2);
            typeln("humi : ");
            typeFloat(humi);
            typeln(" %");
            delay(4000);

            // 라인에 배열입력 방법
            // ClrLcd();
            // lcdLoc(LINE1);
            // typeln(array1); //"airconditional on!"
            // delay(2000);
        }
        else if (human == 2)
        {
            ClrLcd();
            lcdLoc(LINE1);
            typeln("You're not allowed");
            lcdLoc(LINE2);
            typeln("Please get out");
            delay(5000);

            ClrLcd();
            lcdLoc(LINE1);
            typeln("Your face is");
            lcdLoc(LINE2);
            typeln("sent to the host");
            delay(5000);
            ClrLcd();

            ClrLcd();
            lcdLoc(LINE1);
            typeln("Calling police...");
            delay(3000);
        }
                    
    }
    
    return 0;
}


// 함수부분 mqtt sub //
/* Callback called connect 클라이언트 sub로 "토픽"을 이어주고, 에러를 클라로부터 브로커가 받을때 */
void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	int rc;
    // 연결성공시
	printf("recived from : face_recog : %s\n", mosquitto_connack_string(reason_code));
	if(reason_code != 0){
		mosquitto_disconnect(mosq);
	}

	rc = mosquitto_subscribe(mosq, NULL, "myhome/LED", 1); // "토픽"
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
		mosquitto_disconnect(mosq);
	}
}

/* Callback called SUBSCRIBE, 클라로 부터  에러를 브로커가 메세지 전달될때  */
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
	int i;
	bool have_subscription = false;
    //구독 성공
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
	printf("%s %s\n", msg->topic ,(char *)msg->payload); // 프로세스 시스템에 출력
	if(!strcmp( ((char *)msg->payload),"0")) 
		human = 0; //종료
    if(!strcmp( ((char *)msg->payload),"1"))
		human = 1; //me
    if(!strcmp( ((char *)msg->payload),"2"))
		human = 2; //허가되지않은 사용자
}


// LCD 함수 //
// float형 출력
void typeFloat(float myFloat)
{
    char buffer[20];
    sprintf(buffer, "%4.2f", myFloat);
    typeln(buffer);
}

// int형 출력
void typeInt(int i)
{
    char array1[20];
    sprintf(array1, "%d", i);
    typeln(array1);
}

// 초기화 loc 0x80 아무것도없는상태
void ClrLcd(void)
{
    lcd_byte(0x01, LCD_CMD);
    lcd_byte(0x02, LCD_CMD);
}

// 커서이동 LINE1:1번째줄 LINE2:2번째줄
void lcdLoc(int line)
{
    lcd_byte(line, LCD_CMD);
}

// 컈릭터형 출력
void typeChar(char val)
{

    lcd_byte(val, LCD_CHR);
}

// 문장 출력
void typeln(const char *s)
{

    while (*s)
        lcd_byte(*(s++), LCD_CHR);
}

// 바이트결정
void lcd_byte(int bits, int mode)
{

    //Send byte to data pins
    // bits = the data
    // mode = 1 for data, 0 for command
    int bits_high;
    int bits_low;
    // uses the two half byte writes to LCD
    bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
    bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;

    // High bits
    wiringPiI2CReadReg8(fd, bits_high);
    lcd_toggle_enable(bits_high);

    // Low bits
    wiringPiI2CReadReg8(fd, bits_low);
    lcd_toggle_enable(bits_low);
}

void lcd_toggle_enable(int bits)
{
    // Toggle enable pin on LCD display
    delayMicroseconds(500);
    wiringPiI2CReadReg8(fd, (bits | ENABLE));
    delayMicroseconds(500);
    wiringPiI2CReadReg8(fd, (bits & ~ENABLE));
    delayMicroseconds(500);
}

void lcd_init()
{
    // Initialise display
    lcd_byte(0x33, LCD_CMD); // Initialise
    lcd_byte(0x32, LCD_CMD); // Initialise
    lcd_byte(0x06, LCD_CMD); // Cursor move direction
    lcd_byte(0x0C, LCD_CMD); // 0x0F On, Blink Off
    lcd_byte(0x28, LCD_CMD); // Data length, number of lines, font size
    lcd_byte(0x01, LCD_CMD); // Clear display
    delayMicroseconds(500);
}

// DH11 함수 //
void read_dht_data()
{
    int data[5] = { 0, 0, 0, 0, 0 };
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j			= 0, i;
	// data[0] = data[1] = data[2] = data[3] = data[4] = 0; //전역배열변수 초기화

    // dh11
    // LOW를 18ms동안, 그리고 20 ~ 40us동안 HIGH신호를 주면 start 
    pinMode( DHT_PIN, OUTPUT );
	digitalWrite( DHT_PIN, LOW );
	delay(18);
    //digitalWrite(DHT_PIN, HIGH) ;
    //delayMicroseconds(30) ;

	pinMode( DHT_PIN, INPUT ); // pin 연결
	// 데이터 읽는부분
	for ( i = 0; i < MAX_TIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHT_PIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHT_PIN );
		if ( counter == 255 )
			break;
		/* ignore first 3 transitions */
		if ( (i >= 4) && (i % 2 == 0) )
		{
			/* shove each bit into the storage bytes */
			data[j / 8] <<= 1;
			if ( counter > 16 )
				data[j / 8] |= 1;
			j++;
            //printf("data[4] = %d ,data[0]= %d ,data[1]= %d ,data[2]= %d ,data[3]= %d \n",j,data[4],data[0],data[1],data[2],data[3]);
		}
	}
	//data[4] 부호, humi습도 = data[0].data[1] % , temp온도 = data[2].data[3] 'C

	/* check 40 bits (8bit x 5 ) + verify checksum in the last byte*/
	if ( (j >= 40) &&
	    (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
	{
		float h = (float)((data[0] << 8) + data[1]) / 10;
		if ( h > 100 )
		{
			h = data[0];	// humi DHT11
		}
		float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
		if ( c > 125 )
		{
			c = data[2];	// temp DHT11
		}
		if ( data[2] & 0x80 )
		{
			c = -c;
		}
		float f = c * 1.8f + 32; // 'F

		printf( "Humidity = %.1f %% Temperature = %.1f *C (%.1f *F)\n", h, c, f );
        humi = h;
        temp = c;
	}else  {
		printf( "DH11 Data get failed\n" );
        
	}
}

// sigint 핸들링함수. 종료콜 //
void interruptHandler(int sig) 
{
    printf("테스트 프로세스 끝내기.\n"); 
    lcd_init(); //lcd초기화
    digitalWrite(PIN_NUM, 0); //불끄기
    sleep(1);
    printf("Main Done");
    exit(0);
}
