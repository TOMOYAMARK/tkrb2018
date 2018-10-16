#!/usr/bin/env python 
# coding:utf-8
from tkrb2018.srv import *
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


def planQueueHandler(req):
    global plan,frontier
    resp = PlanQueueResponse()

    if(plan[0][0] == 'x' or frontier == True):#未知領域探索に突入！
        frontier = True
        resp.task,resp.param = rules.spin()
    elif(len(plan) == 0):
        resp.task = 's'
        resp.param = 0
    else:
        resp.task = plan[0][0]
        resp.param = plan[0][1]
    	plan.pop(FIRST)
    
    return resp


def planQueueServer():
    rospy.init_node('plan_queue_server')
    s = rospy.Service('plan_queue', PlanQueue, planQueueHandler)        

    print "plan_server ready"
    rospy.spin()

if __name__ == "__main__":
    planQueueServer()
    
