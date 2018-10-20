#!/usr/bin/env python 
# coding:utf-8
from tkrb2018.srv import *
import rospy

first = 0


#初期位置までの操作
#⇛　

###planに ["shoot"]追加⇛決まった以下のシュート動作をplanに挿入
shootpre = [
    ['z',-70],
    ['p',3],
    ['n',80],
    ['p',2],
    ['c',0],
    ['p',1],
    ['a',0],
    ['p',2],
    ['c',110],
    ['p',1],
    ['n',150],
    ['p',2],
    ['n',80],
    ['p',1],
    ['c',0],
    ['p',1],
    ['a',0],
    ['p',2],
    ['c',110],
    ['n',170],
    ['p',1],
    ['z',70],
    ['p',4],
]

shoot = [
    ['z',-93],
    ['p',5],
    ['n',100],
    ['p',2],
    ['n',160],
    ['p',1.3],
    ['n',100],
    ['c',0],
    ['f',1],
    ['p',0.5],
    ['a',0],
    ['p',2],
    ['c',110],
    ['b',1],
    ['n',160],
    ['p',1.5],
    ['n',100],
    ['p',1],
    ['c',0],
    ['f',1],
    ['p',0.5],
    ['a',0],
    ['p',2],
    ['b',1],
    ['c',110],
    ['z',93],
    ['p',2],
    ['n',170],
    ['p',2],
    ['n',170],
    ['p',2]
]




start = [
    ['c',110],
    ['n',30],
    ['z',-25],
    ['p',1],
    ['n',170],
]

collect = [
    ['n',150],
    ['p',1],
    ['p',0.5],
    ['z',25],
    ['p',3],
    ['z',-25],
    ['n',50],
    ['p',1],
    ['n',170],
    ['p',1]
]

#'s'⇛スタートボタン待ち。コントローラの入力を想定。ボタン1 or □ボタン入力までWORKING
#'f','r','b','l'
#'z'⇛回収部持ち上げステピ  ->25 and -25 collect action
#'n'⇛首サーボ⇛ 0->upmax 180->downmax 170->initialPosition
#'c'⇛回収サーボ⇛ 0->open 90->close
#'a'⇛エアー出力
#["shoot"]⇛シュート動作
#["start"]


nplan = [#どうかくよう
    ['p',1],
    ['f',1],
    ['f',1],
    ['b',1],
    ['b',1],
    ['l',90],
    ['r',90],
    ['f',1],
    ['f',1],
    ['b',1],
    ['b',1],
    ['f',1],
    ['b',1],
    ['f',1],
    ['l',90],
    ['l',90],
    ['p',3]
]


plan = [#既知領域用
    ['p',1],
    ['f',1],
    ["collect"],
    ['f',1],
    ["shoot"],
    ['b',1],
    ['r',90],
    ['f',1],
    ["collect"],
    ['l',90],
    ['f',1],
    ["collect"],
    ['b',1],
    ['l',90],
    ['f',1],
    ['f',1],
    ['f',1],
    ['r',90],
    ["collect"],
    ['r',90],
    ['f',1],
    ['f',1],
    ['l',90],
    ['f',1],
    ["shoot"],
    ['l',90],
    ['f',1],
    ['f',1],
    ['r',90],
    ['f',1],
    ["collect"],
    ['f',1],
    ['r',90],
    ["shoot"]

]


def planQueueHandler(req):
    global plan
    resp = PlanQueueResponse()

    if(plan[0][0] == "shoot"):
        print("I have to shoot")
        plan.pop(first)
        plan = shoot + plan
    elif(plan[0][0] == "collect"):
        plan.pop(first)
        plan = collect + plan
    elif(plan[0][0] == "start"):
        plan.pop(first)
        plan = start + plan


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
    
