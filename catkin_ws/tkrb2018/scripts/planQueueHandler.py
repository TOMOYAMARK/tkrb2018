#!/usr/bin/env python 
# coding:utf-8
from tkrb2018.srv import *
import rules
import rospy
from std_msgs.msg import Int8
from std_msgs.msg import String

first = 0
frontier = False


#初期位置までの操作
#⇛　

###planに ["shoot"]追加⇛決まった以下のシュート動作をplanに挿入
shootpre = [
    ['z',-20],
    ['p',3],
    ['n',120],
    ['p',2],
    ['c',0],
    ['p',1],
    ['a',0],
    ['p',2],
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
    ['z',30],
    ['p',3],
    ['z',-30],
    ['n',50],
    ['p',1],
    ['n',170],
    ['p',1.5]
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
    ['f',1],
    ['f',1],
    ["shoot"]
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
    ["shoot"],
    ['b',1],
    ['l',90],
    ['x',0],
    ['p',2]

]

def planQueueHandler(req):
    global plan,frontier
    resp = PlanQueueResponse()
    print(plan[0][0])

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
    elif(plan[0][0] == "snapshot"):
        rospy.wait_for_service('Snapshot')
        print("accessed service")
        try:
            snapshot= rospy.ServiceProxy('Snapshot', Snapshot)
            resp = snapshot("look_side")
            if(resp.balls > 0):
                resp.balls = 0
                rules.ball_thisside = True
            print("saw a ball! {}".format(resp.balls))
        except rospy.ServiceException, e:
            print "Service call failed: %s"%e
        plan.pop(first)



    if(plan[0][0] == 'x' or frontier == True):
        print("getting rules...")
        #未知領域探索に突入！
        frontier = True
        
        #'x'というプランを渡したあとは、rules.pyに記述された
        #アルゴリズムにしたがってプランが小出しに装填
        resp.task,resp.param = rules.spin()
        print("got {}".format(resp.task))
        if(resp.task == "snapshot"):
            plan.insert(0,["snapshot"])
            resp.task = 'p'

    elif(len(plan) == 0):
        print("nothing")
        resp.task = 's'
	resp.param = 0.0
    
    else:
        resp.task = plan[0][0]
        resp.param = plan[0][1]
        plan.pop(first)
    
    return resp

def PSDFCallback(val):
    #受け取るデータは3bit [L][R][F]0 or 1
    #arduino側でしきい値処理をさせてから受信する。
    psdVals = val.data

    #正面に反応
    rules.psd_front = val.data

def PSDSCallback(val):
    #受け取るデータは3bit [L][R][F]0 or 1
    #arduino側でしきい値処理をさせてから受信する。
    psdVals = val.data

    #側面に反応
    rules.psd_right = ((psdVals & 2) == 2)
    rules.psd_left = ((psdVals & 4) == 4)
    rules.psd_frontHigh = ((psdVals & 8) == 8)

def planQueueServer():
    rospy.init_node('plan_queue_server')
    rospy.Subscriber("psdfront",Int8, PSDFCallback)
    rospy.Subscriber("psdside",Int8, PSDSCallback)
    s = rospy.Service('plan_queue', PlanQueue, planQueueHandler)       

    print "plan_server ready"
    rospy.spin()

if __name__ == "__main__":
    planQueueServer()
    
