#!/usr/bin/env python 
# coding:utf-8
from tkrb2018.srv import *
import rospy

first = 0

#"Next"に対応
#キューの処理、例外は's'(STOP)にする
#arduino側の調整

plan = [
    ['n',0],
    ['p',1],
    ['n',40],
    ['p',1],
    ['n',0],
    ['p',1],
    ['n',180]
    ##
    ['p',0.5],
    ['c',0],
    ['p',1],
    ['f',0.5],
    ['c',180],
    ['f',0.5],
    ['p',1],
    ['b',1]
]


def planQueueHandler(req):
    resp = PlanQueueResponse()

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
    
