#!/usr/bin/env python 
# coding:utf-8
from tkrb2018.srv import *
import rospy

first = 0


###planに ["shoot"]追加⇛決まった以下のシュート動作をplanに挿入
shoot = [
    ['z',-120],
    ['p',5],
    ['n',45],
    ['c',180],
    ['p',1],
    ['a',0],
    ['p',2],
    ['c',0],
    ['n',60],
    ['z',120],
    ['p',5]
    ]

#'s'⇛スタートボタン待ち。コントローラの入力を想定。ボタン1 or □ボタン入力までWORKING
#'f','r','b','l'
#'z'⇛回収部持ち上げステピ
#'n'⇛首サーボ
#'c'⇛回収サーボ
#'a'⇛エアー出力
#["shoot"]⇛シュート動作

plan = [
    ['p',3],
    ["shoot"]
]


def planQueueHandler(req):
    global plan
    resp = PlanQueueResponse()

    if(plan[0][0] == "shoot"):
        print("I have to shoot")
        plan.pop(first)
        plan = shoot + plan
    pass

    if(len(plan) == 0):
        resp.task = 's'
	resp.param = 0.0
    
    else:
        resp.task = plan[0][0]
        resp.param = plan[0][1]
    	plan.pop(first)
    
    return resp


def planQueueServer():
    rospy.init_node('plan_queue_server')
    s = rospy.Service('plan_queue', PlanQueue, planQueueHandler)        

    print "plan_server ready"
    rospy.spin()

if __name__ == "__main__":
    planQueueServer()
    
