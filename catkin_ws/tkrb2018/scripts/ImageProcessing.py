#!/usr/bin/env python
# -*- coding: utf-8 -*-
import rospy
import numpy as np
import cv2
from std_msgs.msg import String
from std_msgs.msg import Int8,Int8MultiArray,MultiArrayLayout,MultiArrayDimension

IMG_SIZE_PERCENT = 0.25#このサイズに画像をリサイズしてから処理

#######snapshot_reqからされるリクエストモード########
DITECTION = -1#とりあえずボールの色認識⇛座標を記憶（テスト用）

################ボールの色しきい値(HSV)################
PURPLE_L,PURPLE_H = np.array([100,90,10]),np.array([200,150,255])
GREEN_L,GREEN_H = np.array([100,90,10]),np.array([200,150,255])
YELLOW_L,YELLOW_H = np.array([100,90,10]),np.array([200,150,255])

camera = cv2.VideoCapture(0)
webcamOutputPub = None

def readCamera():
    ret,frame = camera.read()
    # スクリーンショットを撮りたい関係で1/4サイズに縮小
    frame = cv2.resize(frame, (int(frame.shape[1]*IMG_SIZE_PERCENT), int(frame.shape[0]*IMG_SIZE_PERCENT)))
    return frame

def snapShot(arg):#コールバック
    image = readCamera()
    #image = cv2.imread('field2.png')

    imageHSV = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
    imageGray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
    # 指定した色に基づいたマスク画像の生成
    imageMask = cv2.inRange(imageHSV, PURPLE_L, PURPLE_H)

    while(1):
        cv2.imshow("SHOW MASK IMAGE", imageMask)#マスク画像を表示
 
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
