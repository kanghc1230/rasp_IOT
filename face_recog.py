#face_recog.py
import numpy as np
import face_recognition as fr
import cv2

video_capture = cv2.VideoCapture(0)

#STEP 1. 사진부분
#raspistill -o ~/work/me.jpg
#사진한장 찍어서 파일하나 넣기
me_image = fr.load_image_file("/home/pi/work/me.jpg")
#print(fr.face_encodings(me_image)) 
#얼굴사진 인코딩해서 [-1212414,...] 128개의 리스트로 변화
me_face_encoding = fr.face_encodings(me_image)[0]

known_face_encondings = [me_face_encoding]
known_face_names = ["Me"] #your name

#STEP 2. 촬영부분
while True: 
    ret, frame = video_capture.read()

    #STEP 3. 촬영인코딩
    rgb_frame = frame[:, :, ::-1]

    #로케이션, 인코딩
    face_locations = fr.face_locations(rgb_frame)
    face_encodings = fr.face_encodings(rgb_frame, face_locations)
    
    
    for (top, right, bottom, left), face_encoding in zip(face_locations, face_encodings):
        #촬영에서 얼굴인코딩해 비교
        matches = fr.compare_faces(known_face_encondings, face_encoding)

        name = "Unknown"

        face_distances = fr.face_distance(known_face_encondings, face_encoding)

        best_match_index = np.argmin(face_distances)
        if matches[best_match_index]:
            name = known_face_names[best_match_index]
        
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)

        cv2.rectangle(frame, (left, bottom -35), (right, bottom), (0, 0, 255), cv2.FILLED)
        font = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)

    cv2.imshow('Webcam_facerecognition', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

video_capture.release()
cv2.destroyAllWindows()


