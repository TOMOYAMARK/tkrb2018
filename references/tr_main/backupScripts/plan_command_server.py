#!/usr/bin/env python 
from tr_main.srv import *
import rospy

ST = 40
ST_UP = 550
ST_ARMS = 20
ST_PULL = 400
TUNE = 0.6
F_TUNE = 0.1
ALPHA = 0.1

plan = [
#for testing  ['f',5],
    ['b',F_TUNE],
    ['c', 1],
    ['s', ST_ARMS],
    ['e', 1.4 + F_TUNE],
    ['c', 2],
    ['s', ST_ARMS*2],
    ['l', 90],
    ['s',10],

    ['b', TUNE],
    ['c', 1],
    ['s', ST_ARMS],
    ['e', 2.0+TUNE],
    ['c', 2],
    ['s', ST_ARMS*2],

    ['r', 90],  
    ['s',10],
    ['b', TUNE],
    ['c', 1],
    ['s', ST_ARMS],
    ['e', 5.4+TUNE],
    ['c', 2],
    ['s', ST_ARMS*2],

    ['r', 90],
    ['s',10],
    ['b', TUNE],
    ['c', 1],
    ['s', ST_ARMS],
    ['e', 4.4+TUNE],
    ['c', 2],
    ['s', ST_ARMS*2],   

    ['r', 90],
    ['s',10],
    ['b',TUNE],
    ['c',1],
    ['s',ST_ARMS],
    ['e', 4.3+TUNE],
    ['c', 2], 
    ['s', ST_ARMS*2],  
    ['r', 90],

    ['s',10],
    ['e', 1.95],
    ['s',10],
    ['r', 92],
    ['s',10],
    ['b',0.2],
    ['u', 1],
    ['s', ST_UP],
    ['e',0.2],  
    ['b',0.1],
    ['e',0.1],
    ['s',100],
    ['e', 0.7],
    ['p', 2],   
    ['s', ST_PULL],
    ['b', 0.1],
    ['e', 0.1],
    ['b', 0.1],
    ['e', 0.1],
    ['b', 0.1],
    ['e', 0.1],
    ['b', 0.1],
    ['e', 0.1],
    ['b', 0.1],
    ['e', 0.1],   
    ['z', 0],
    ['s', 50],
    ['b', 3],
]


def handle_plan_command(req):
    if req.i > len(plan)-1: #len=2 -> 0, 1,
        result = 's'
    else:
        result = plan[req.i][0]
    return PlanCommandResponse(result)

def handle_plan_param(req):
    if req.i > len(plan)-1: #len=2 -> 0, 1,
        result = 1
    else:
        result = plan[req.i][1]    
    return PlanParamResponse(result)

def plan_command_server():
    rospy.init_node('plan_server')
    s2 = rospy.Service('plan_param', PlanParam, handle_plan_param)        
    s1 = rospy.Service('plan_command', PlanCommand, handle_plan_command)

    print "plan_server ready"
    rospy.spin()

if __name__ == "__main__":
    plan_command_server()
    
