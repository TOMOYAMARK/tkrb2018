
###clockwise###
F = 0
R = 1
B = 2
L = 3

#####psdVals####
psd_front = False
psd_left = False
psd_right = False
psd_frontHigh = False


##cameraFeedbacks##
ball_thisside = False
ball_thatside = False
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

HunknownPlan = [
    ['b',1],
    ['l',90],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['r',90],
    ['r',90],
    ["snapshot",1],
    ['f',1],
    ["snapshot",1],
    ['f',1],
    ["snapshot",1],
    ['f',1],
    ["snapshot",1],
    ['f',1],
    ['l',90],
    ['f',1],
    ["shoot",0],
    ['f',1],
    ['f',1],
    ['l',90],
    ['l',90],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['l',90],
    ['f',1],
    ['f',1],
    ['f',1],
    ["snapshot",1],
    ['f',1],
    ["snapshot",1],
    ['f',1],
    ["snapshot",1],
    ['f',1],
    ["snapshot",1],
    ["shoot",1]
]

unknownPlan = [
    ['l',90],
    ['f',1],
    ['f',1],
    ['r',90],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['r',90],
    ['f',1],
    ['r',90],
    ['f',1],
    ["shoot",0],
]


map = [[1,1,1,1,1,1,1,1,1],
       [1,0,0,0,0,0,0,0,1],
       [1,0,0,0,0,1,0,0,1],
       [1,1,0,0,0,0,0,0,1],
       [1,0,0,0,0,0,0,0,1],
       [1,0,0,0,1,0,0,0,1],
       [1,0,0,0,0,0,0,0,1],
       [1,0,0,0,0,0,0,0,1],
       [1,0,0,0,0,0,0,0,1],
       [1,0,0,0,0,0,0,0,1],
       [1,1,1,0,0,0,1,1,1]]

mask = [[1,1,1,1,1,1,1,1,1],
        [1,0,0,0,0,0,0,0,1],
        [1,0,0,0,0,0,0,0,1],
        [1,0,0,0,0,0,0,0,1],
        [1,0,0,0,0,0,0,0,1],
        [1,1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1,1]]

moveMask = [[1,1,1,1,1,1,1,1,1],
        [1,0,0,0,0,0,0,0,1],
        [1,0,0,0,0,0,0,0,1],
        [1,0,0,0,0,0,0,0,1],
        [1,0,0,0,0,0,0,0,1],
        [1,1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1,1],
        [1,1,1,1,1,1,1,1,1]]

### machine state when starting to search unknown area ###
pos = [1,5]
heading = F
goal = [4,5]

task = 'o'
param = 0

def move(dir):
    if(dir == F):
        if(heading == F):
            pos[1] -= 1
        elif(heading == R):
            pos[0] += 1
        elif(heading == B):
            pos[1] += 1
        elif(heading == L):
            pos[0] -= 1
    elif(dir == B):
        if(heading == F):
            pos[1] += 1
        elif(heading == R):
            pos[0] -= 1
        elif(heading == B):
            pos[1] -= 1
        elif(heading == L):
            pos[0] += 1

def rotate(dir):
    global heading
    if(dir == R):
        heading += 1
        if(heading == 4):heading = 0
    if(dir == L):
        heading -= 1
        if(heading == -1):heading = 3

def cvtHeading(dir):
    if(dir == F):return 'f'
    if(dir == R):return 'r'
    if(dir == B):return 'b'
    if(dir == L):return 'l'

def look():
    if(heading == F):
        mask[pos[1]-1][pos[0]] = 1
        return map[pos[1]-1][pos[0]]
    elif(heading == R):
        mask[pos[1]][pos[0]+1] = 1
        return map[pos[1]][pos[0]+1]
    elif(heading == B):
        mask[pos[1]+1][pos[0]] = 1
        return map[pos[1]+1][pos[0]]
    elif(heading == L):
        mask[pos[1]][pos[0]-1] = 1
        return map[pos[1]][pos[0]-1]

def lookMask():
    if(heading == F):
        return mask[pos[1]-1][pos[0]]
    elif(heading == R):
        return mask[pos[1]][pos[0]+1]
    elif(heading == B):
        return mask[pos[1]+1][pos[0]]
    elif(heading == L):
        return mask[pos[1]][pos[0]-1]


def searchedAll():
    for i in range(11):
        for j in range(9):
            if(mask[i][j] == 0):return 0
    return 1


def searchAlgorithm():
    global unknownPlan,task,param
    if(unknownPlan == []):return 0
    task = unknownPlan[0][0]
    param = unknownPlan[0][1]
    unknownPlan.pop(0)
    if(task == 'f'):
        if(look() == 1):
            if(heading == R):
                if(pos[0] == 6):
                    unknownPlan.pop(0)
                    unknownPlan.pop(0)
                    unknownPlan = [['l',90],['f',1],['r',90],['f',1],['l',90]] + unknownPlan
                else:
                    unknownPlan.pop(0)
                    unknownPlan = [['l',90],['f',1],['r',90],['f',1],['f',1],['r',90],['f',1],['l',90]] + unknownPlan
            elif(heading == L):
                if(pos[0] == 2):
                    unknownPlan.pop(0)
                    unknownPlan.pop(0)
                    unknownPlan = [['r',90],['f',1],['l',90],['f',1],['r',90]] + unknownPlan
                else:
                    unknownPlan.pop(0)
                    unknownPlan = [['r',90],['f',1],['l',90],['f',1],['f',1],['l',90],['f',1],['r',90]] + unknownPlan
        else:
            move(F)
            
    elif(task == 'r'):
        rotate(R)
    elif(task == 'b'):
        move(B)
    elif(task == 'l'):
        rotate(L)
    elif(task == "shoot"):
        print("I have to shoot")
        unknownPlan = shoot + unknownPlan

def searchAlgorithmEx():
    global unknownPlan,task,param,ball_thisside,ball_thatside,psd_front
    task = unknownPlan[0][0]
    param = unknownPlan[0][1]
    unknownPlan.pop(0)
    if(task == 'f'):
        if(ball_thisside or ball_thatside):
            print("going get the balls!")
            ball_thiside = ball_thatside = 0
            unknownPlan = [['l',90],['f',1],['f',1],['f',1],['f',1],['b',1],['b',1],['b',1],['b',1],['r',90]] + unknownPlan
        else:
            print("entered section")
            print(psd_front)
            if(psd_front == True):
                unknownPlan.insert(0,['f',1])
                unknownPlan = collect + unknownPlan
                task = 'p'
            if(psd_left == True):
                unknownPlan.pop(0)
                unknownPlan = [['r',10],['f',1]]
            elif(psd_right == True):
                unknownPlan.pop(0)
                unknownPlan = [['l',10],['f',1]]
            else:move(F)
            psd_front = False
    elif(task == 'r'):
        rotate(R)
    elif(task == 'b'):
        move(B)
    elif(task == 'l'):
        rotate(L)


def spin():
    searchAlgorithmEx()
    return task,param
