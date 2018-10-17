#!/usr/bin/env python 
# coding:utf-8
from tkrb2018.srv import *
from std_msgs.msg import Int32
import rospy
import rules

FIRST = 0
frontier = False #未知領域探索モード　⇛　True

#"Next"に対応
#キューの処理、例外は's'(STOP)にする
#arduino側の調整

plan = [
    ['c',0],
    ['x',0]
]

psdVal = 1000 #測距センサーの値を入れていくよ
psdThresh = 200

def planQueueHandler(req):
    global plan,frontier
    resp = PlanQueueResponse()

    if(plan[0][0] == 'x' or frontier == True):
        #未知領域探索に突入！
        frontier = True

        #このへんでカメラノードにサービス要請する予定
        #１マス、あわよくば2マス先を見てボールがいくつあるか返す⇛マッピング

        #'x'というプランを渡したあとは、rules.pyに記述された
        #アルゴリズムにしたがってプランが小出しに装填されていきます
        resp.task,resp.param = rules.spin()
    elif(len(plan) == 0):
        resp.task = 's'
        resp.param = 0
    else:
        resp.task = plan[0][0]
        resp.param = plan[0][1]
    	plan.pop(FIRST)
    
    return resp


def PSDCallback(data):
    psdVal = data.data
    if(psdVal < psdThresh):rules.psd = 1
    else:rules.psd = 0
    #障害物の存在をrules.pyに反映させる処理。
    #rules.look()のときにpsdVal < threshが満たされていれば障害物の
    #存在を反映

def planQueueServer():
    rospy.init_node('plan_queue_server')
    rospy.Subscribe("psd_request",Int32, PSDCallback)
    s = rospy.Service('plan_queue', PlanQueue, planQueueHandler)        

    print "plan_server ready"
    rospy.spin()

if __name__ == "__main__":
    planQueueServer()
    
