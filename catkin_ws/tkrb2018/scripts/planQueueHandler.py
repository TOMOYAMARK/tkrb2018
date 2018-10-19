#!/usr/bin/env python 
# coding:utf-8
from tkrb2018.srv import *
import rospy

first = 0


###planに ["shoot"]追加⇛決まった以下のシュート動作をplanに挿入
shoot = [
    ['z',-120],
    ['p',5],
    ['n',60],
    ['c',90],
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
#'n'⇛首サーボ⇛ 0->upmax 180->downmax 30->initialPosition
#'c'⇛回収サーボ⇛ 0->open 90->close
#'a'⇛エアー出力
#["shoot"]⇛シュート動作

mplan = [
    ['c',90],
    ['n',30],
    ['p',1],
    ['f',1],
    ['r',90],
    ['f',1],
    ['l',90],
    ['f',1],
    ['r',90],
    ['f',2],
    ['l',90],
    ['f',1],
    ["shoot"]
]

plan = [
    ['n',25],
    ['p',0.5],
    ['c',0],
    ['p',1],
    ['f',1],
    ['c',90],
    ['f',1],
    ['c',0],
    ['p',1],
    ['c',90],
    ['f',1]
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
	if(resp.task == 'f' or resp.task == 'b'):resp.param += 0.2
    	plan.pop(first)
    
    return resp


def planQueueServer():
    rospy.init_node('plan_queue_server')
    s = rospy.Service('plan_queue', PlanQueue, planQueueHandler)        

    print "plan_server ready"
    rospy.spin()

if __name__ == "__main__":
    planQueueServer()
    
