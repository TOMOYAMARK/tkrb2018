#!/usr/bin/env python
# license removed for brevity

import rospy
import cv2
from std_msgs.msg import String

def snapShot():
    pub = rospy.Publisher('chatter', String, queue_size=10)
    rospy.init_node('talker', anonymous=True)
    r = rospy.Rate(10) # 10hz
    while not rospy.is_shutdown():
        str = "hello world %s"%rospy.get_time()
        pub.publish(str)
        r.sleep()

if __name__ == '__main__':
    try:
        snapShot()
    except rospy.ROSInterruptException: pass
