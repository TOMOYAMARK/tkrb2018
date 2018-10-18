#!/usr/bin/env python
# -*- coding: utf-8 -*-
import rospy
import numpy as np
import cv2
from std_msgs.msg import String
from std_msgs.msg import Int8,Int8MultiArray,MultiArrayLayout,MultiArrayDimension

IMG_SIZE_PERCENT = 1#この倍率に画像をリサイズしてから処理

threshSet = {'Red':[[1,162,102],[17,255,255]], \
            'Green':[[66,77,45],[83,255,255]], \
            'Blue':[[85,137,86],[129,255,255]], \
            'Yellow':[[15,211,124],[41,255,255]], \
            'Purple':[[121,67,2],[180,255,255]]}

heightDiff = [45,62,73,104,145]
ballColor = []

width = 320
height = 240

camera = cv2.VideoCapture(0)
webcamOutputPub = None


def detectBalls(hsv):
    #HOUGHCirclesで検出がうまくいく引数たち
    CONST_DP = 1
    PARAM1 = 20
    PARAM2 = 12
    #HSV色空間で渡された色に応じて２値のマスクを生成

    hsv_mask = np.zeros((height,width),np.uint8)
    for th in threshSet:
        print(np.array(threshSet[th])[0])
        mask = cv2.inRange(hsv,np.array(threshSet[th][0]),np.array(threshSet[th][1]))
        hsv_mask = cv2.bitwise_or(mask,hsv_mask)
    #見やすさのため黒背景画像を利用
    hsv_mask = cv2.bitwise_not(hsv_mask)
    blackImage = np.zeros((height, width, 1), np.uint8)
    blackImage.fill(125)
    #黒背景画像の切り取り→反転でボールを灰色表示
    masked_img = cv2.bitwise_and(blackImage, hsv_mask)
    masked_img = masked_img.astype(np.uint8)
    masked_img = cv2.medianBlur(masked_img,5)#ブラーかけることで精度高
    cimg = cv2.cvtColor(masked_img,cv2.COLOR_GRAY2BGR)

    circles = cv2.HoughCircles(masked_img,cv2.HOUGH_GRADIENT,1,20,param1=PARAM1,param2=PARAM2,minRadius=0,maxRadius=height)

    if(circles is not None):
        circles = np.uint16(np.around(circles))
        #print(circles.shape)
        for i in circles[0,:]:
            # draw the outer circle
            cv2.circle(cimg,(i[0],i[1]),i[2],(0,255,0),2)
            # draw the center of the circle
            cv2.circle(cimg,(i[0],i[1]),2,(0,0,255),3)

    cv2.imshow('mask',hsv_mask)
    cv2.imshow('circles',cimg)

    return circles[0,:]


def readCamera():
    ret,frame = camera.read()
    #リサイズ
    #frame = cv2.resize(frame, (int(frame.shape[1]*IMG_SIZE_PERCENT), int(frame.shape[0]*IMG_SIZE_PERCENT)))
    return frame

def snapShot(arg):#コールバック
    #img = readCamera()
    img = cv2.imread('src/tkrb2018/scripts/field2.png')

    print(img.shape)

    hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV)
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    ballCordinates = detectBalls(hsv)
    for bc in ballCordinates:
        #print("watching {}".format(bc[0]))
        counter=0
        if(bc[1] < width*0.8 and bc[1] > width*0.2):
            for hTh in heightDiff:
                if(bc[1] > hTh):
                    counter += 1
            print("I see a ball at distant:{}".format(6-counter))

    while(1):
 
        # qを押したら終了
        k = cv2.waitKey(1)
        if k == ord('q'):
            break

def main():
    rospy.init_node('image_processing', anonymous=True)
    webcamOutputPub = rospy.Publisher('webcam_out', Int8MultiArray, queue_size=10)
    rospy.Subscriber("snapshot_req", Int8, snapShot)

    snapShot(1)
    
    rospy.spin()

if __name__ == '__main__':
    main()
