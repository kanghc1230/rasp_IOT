#가상환경 source venv2/bin/activate
#face_recog.py   실행방식: $python3 ./project_face_recog.py 
#MQTT https://proni.tistory.com/entry/%F0%9F%90%8D-Python-MQTT-module-paho%EB%A5%BC-%EC%9D%B4%EC%9A%A9%ED%95%9C-MQTT-%ED%86%B5%EC%8B%A0
from os import wait
import numpy as np
import face_recognition as fr
import cv2
import threading, time
import paho.mqtt.client as mqtt

#들어온 사람 (0:아무도없음, 1:나 2:외부인)
human = 0

#전역변수 mqtt쓰레드 상태 조정
mqttThread_status = 1
#mqtt쓰레드 실행부분
def mqttThread ():
    print("mqtt on")
    #집안에 사람수체크할 변수
    FamilyInHome = 0
    global human
    global led
    mqttc = mqtt.Client("recog") #pub이름 아무거나
    mqttc.connect("192.168.1.10", 1883,60) #보낼아이피
    while True:
        #me와 집안에 아무도없을때
        #접근한사람 human이 me일때
        if human == 1:
            #집주인에게 알림
            mqttc.publish("myhome/VIDEO", "Me가 집에 들어왔습니다") #"토픽", "msg"
            #IOT LED작동
            mqttc.publish("myhome/LED", "1")
            #mqttc.loop(10)
            human = 0
            
        #낮선사람
        if human == 2:
            #집주인에게 알림
            mqttc.publish("myhome/VIDEO", "낮선사람이 집을 서성입니다.") #"토픽", "msg"
            #IOT 경고음
            mqttc.publish("myhome/LED", "2")  #"토픽", "msg"
            #사진파일 보내기
            time.sleep(5)
            fd = open("/home/pi/work/unknown.jpg", "rb")  #저장된 사진파일열기
            Unknown_image = fd.read() #파일 읽어들여서
            mqttc.publish("myhome/VIDEO/Unknown", Unknown_image, 0)  #"토픽", "image"
            fd.close() #닫기

            #mqttc.loop(10)
            human = 0

        print("mqtt is running...")
        time.sleep(5)

        #종료 콜 받으면
        if mqttThread_status == 0:
            print("mqtt off")
            break;
        

#메인 쓰레드부분
#mqtt쓰레드 시작.
t1 = threading.Thread(target=mqttThread, args=())
t1.start()
time.sleep(1) #쓰레드시작대기

#캡쳐 카메라 연결
video_capture = cv2.VideoCapture(0)

#STEP 1. 사진부분
#raspistill -o /home/pi/work/me320x240.jpg -w 320 -h 240
#사진한장 찍어서 파일하나 넣기
#me_image = fr.load_image_file("/home/pi/work/me.jpg")
me_image = fr.load_image_file("/home/pi/work/me320x240.jpg")
#얼굴사진 인코딩해서 [-1212414,...] 128개의 리스트로 변화
me_face_encoding = fr.face_encodings(me_image)[0]

known_face_encondings = [me_face_encoding]
known_face_names = ["Me"] #your name

#비디오 촬영 사이즈
video_capture.set(cv2.CAP_PROP_FRAME_WIDTH, 320) #cv2.CAP_PROP_FRAME_WIDTH == 3
video_capture.set(cv2.CAP_PROP_FRAME_HEIGHT, 240) #cv2.CAP_PROP_FRAME_HEIGHT == 4
video_capture.set(cv2.CAP_PROP_FPS, 20)
#STEP 2. 비디오 촬영부분
while True: 
    ret, frame = video_capture.read()

    #STEP 3. 촬영인코딩
    rgb_frame = frame[:, :, ::-1]

    #로케이션, 인코딩
    face_locations = fr.face_locations(rgb_frame)
    face_encodings = fr.face_encodings(rgb_frame, face_locations)
    
    #얼굴비교부분
    for (top, right, bottom, left), face_encoding in zip(face_locations, face_encodings):
        #촬영에서 얼굴인코딩해 비교
        matches = fr.compare_faces(known_face_encondings, face_encoding)

        name = "Unknown"

        face_distances = fr.face_distance(known_face_encondings, face_encoding)

        best_match_index = np.argmin(face_distances)
        if matches[best_match_index]: #촬영대상이 등록된 사진과일치하면
            name = known_face_names[best_match_index] #name에 unknown대신 me 
            human = 1
        else: #촬영대상이 알수없는 사용자라면
            human = 2
            cv2.imwrite("/home/pi/work/unknown.jpg", frame) #프레임을 사진으로저장
            
        
        #사각형 그리기
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)
        cv2.rectangle(frame, (left, bottom -35), (right, bottom), (0, 0, 255), cv2.FILLED)
        font = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)

    #화면에 띄우기
    cv2.imshow('Webcam_facerecognition', frame)

    if (cv2.waitKey(1) & 0xFF == ord('q')):
        break

#쓰레드 종료
mqttThread_status = 0 #스레드,종료
time.sleep(1)
#카메라 off
video_capture.release()
cv2.destroyAllWindows()
print("Main Done")


