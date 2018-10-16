
###clockwise###
F = 0
R = 1
B = 2
L = 3

BALLS = -1

unknownPlan = [
    ['f',1],
    ['r',90],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['l',90],
    ['f',1],
    ['l',90],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['r',90],
    ['f',1],
    ['r',90],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['l',90],
    ['f',1],
    ['l',90],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
    ['f',1],
]

limit = 100

map = [[1,1,1,1,1,1,1,1,1],
       [1,-1,0,0,1,0,-2,0,1],
       [1,0,0,0,0,0,0,0,1],
       [1,0,1,0,-1,-1,0,0,1],
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

### machine state ###
pos = [1,5]
heading = [F]

### virtual machine state ###
posIm = []
headingIm = []

goal = [4,5]
ballHandling = 0
capacity = 2
haveToShoot = False

##シュート時のマシンのステータス
shootingPoint = [[3,5],[5,5]]
shootingDir = [R,L]

task = 's'
param = 0

##communication variables##
def move(dir,posbuf,hbuf):
    global ballHandling,haveToShoot
    if(dir == F):
        if(hbuf[0] == F):
            posbuf[1] -= 1
        elif(hbuf[0] == R):
            posbuf[0] += 1
        elif(hbuf[0] == B):
            posbuf[1] += 1
        elif(hbuf[0] == L):
            posbuf[0] -= 1
    elif(dir == B):
        if(hbuf[0] == F):
            posbuf[1] += 1
        elif(hbuf[0] == R):
            posbuf[0] -= 1
        elif(hbuf[0] == B):
            posbuf[1] -= 1
        elif(hbuf[0] == L):
            posbuf[0] += 1
    if(map[pos[1]][pos[0]] < 0):
        ballHandling += map[pos[1]][pos[0]] * BALLS
        map[pos[1]][pos[0]] = 0
        if(ballHandling >= capacity):
            print("I have to shoot")
            haveToShoot = True

def rotate(dir,h):
    if(dir == R):
        h[0] += 1
        if(h[0] == 4):h[0] = 0
    if(dir == L):
        h[0] -= 1
        if(h[0] == -1):h[0] = 3

def cvtHeading(dir):
    if(dir == F):return '↑'
    if(dir == R):return '→'
    if(dir == B):return '↓'
    if(dir == L):return '←'

def look():#ここにSnapShotの結果（1マス先の情報を当てはめる）
    if(heading[0] == F):
        mask[pos[1]-1][pos[0]] = 1
        return map[pos[1]-1][pos[0]]
    elif(heading[0] == R):
        mask[pos[1]][pos[0]+1] = 1
        return map[pos[1]][pos[0]+1]
    elif(heading[0] == B):
        mask[pos[1]+1][pos[0]] = 1
        return map[pos[1]+1][pos[0]]
    elif(heading[0] == L):
        mask[pos[1]][pos[0]-1] = 1
        return map[pos[1]][pos[0]-1]

def lookMask():
    if(heading[0] == F):
        return mask[pos[1]-1][pos[0]]
    elif(heading[0] == R):
        return mask[pos[1]][pos[0]+1]
    elif(heading[0] == B):
        return mask[pos[1]+1][pos[0]]
    elif(heading[0] == L):
        return mask[pos[1]][pos[0]-1]


def searchedAll():
    for i in range(11):
        for j in range(9):
            if(mask[i][j] == 0):return 0
    return 1

def reversePlan(plbuf):
    pl = plbuf.copy()
    if(pl[0] == 'f'):
        pl[0] = 'b'
        return pl
    elif(pl[0] == 'b'):
        pl[0] = 'f'
        return pl
    elif(pl[0] == 'r'):
        pl[0] = 'l'
        return pl
    elif(pl[0] == 'l'):
        pl[0] = 'r'
        return pl

def reversePlay(arr):
    revArr = []
    for n in arr:
        revArr.insert(0,reversePlan(n))
    return revArr

def roundTrip(pos_a,head_a,pos_b,head_b):
    global unknownPlan,ballHandling,haveToShoot
    posIm = pos_a.copy()
    headingIm = head_a.copy()

    tripPlan = []
    if(pos_b[0]-posIm[0] < 0):#b------a
        while(headingIm[0] != L):
            rotate(L,headingIm)
            tripPlan.append(['l',90])
    elif(pos_b[0]-posIm[0] > 0):#a------b
        while(headingIm[0] != R):
            rotate(R,headingIm)
            tripPlan.append(['r',90])
    elif(pos_b[0]-posIm[0] == 0):
        while(headingIm[0] != B):
            rotate(R,headingIm)
            tripPlan.append(['r',90])
    for x in range(abs(pos_b[0]-posIm[0])):
        move(F,posIm,headingIm)
        tripPlan.append(['f',1])
    while(headingIm[0] != B):
        rotate(R,headingIm)
        tripPlan.append(['r',90])
    for y in range(abs(pos_b[1] - posIm[1])):
        move(F,posIm,headingIm)
        tripPlan.append(['f',1])
    while(headingIm[0] != head_b):
        rotate(L,headingIm)
        tripPlan.append(['l',90])

    if(headingIm[0] == head_b and posIm == pos_b):
        print("shooted!! virtually")
        haveToShoot = False
    tripPlan = tripPlan + reversePlay(tripPlan)

    unknownPlan = tripPlan + unknownPlan
    ballHandling = 0
    print(tripPlan)

def pivotTurning(pos_a,heading_a,pos_b,heading_b,pivot,cw):
    print("Im pivot turning pivot:{}".format(pivot))
    pivotPlan = []
    headingIm = heading_a.copy()
    posIm = pos_a.copy()
    if(cw == True):
        rotate(L,headingIm)
        pivotPlan.append(['l',90])

        move(F,posIm,headingIm)
        pivotPlan.append(['f',1])
        rotate(R,headingIm)
        pivotPlan.append(['r',90])
        while(posIm != pos_b):
            print(posIm,pos_b)
            move(F,posIm,headingIm)
            pivotPlan.append(['f',1])
            if(posIm == pos_b):
                break
            move(F,posIm,headingIm)
            pivotPlan.append(['f',1])
            rotate(R,headingIm)
            pivotPlan.append(['r',90])
        while(headingIm != heading_b):
            rotate(L,headingIm)
            pivotPlan.append(['l',90])

    if(cw == False):
        rotate(R,headingIm)
        pivotPlan.append(['r',90])

        move(F,posIm,headingIm)
        pivotPlan.append(['f',1])
        rotate(L,headingIm)
        pivotPlan.append(['l',90])
        while(posIm != pos_b):
            move(F,posIm,headingIm)
            pivotPlan.append(['f',1])
            if(posIm == pos_b):break
            move(F,posIm,headingIm)
            pivotPlan.append(['f',1])
            rotate(L,headingIm)
            pivotPlan.append(['l',90])
        while(headingIm != heading_b):
            rotate(R,headingIm)
            pivotPlan.append(['r',90])

    print(pivotPlan)
    return pivotPlan

def detour(ps,head,pivot):
    print("I7m taking a detour")
    global unknownPlan
    headingIm = head.copy()
    posIm = ps.copy()
    move(F,posIm,headingIm)
    while(unknownPlan[0][0] != 'f'):
        if(unknownPlan[0][0] == 'r'):
            rotate(R,headingIm)
        elif(unknownPlan[0][0] == 'l'):
            rotate(L,headingIm)
        unknownPlan.pop(0)
    move(F,posIm,headingIm)
    unknownPlan.pop(0)

    if(heading[0] == L):
        if(pos[1] != 1):
            unknownPlan = pivotTurning(pos,heading,posIm,headingIm,pivot,False) + unknownPlan
        else:
            unknownPlan = pivotTurning(pos,heading,posIm,headingIm,pivot,True) + unknownPlan
    elif(heading[0] == R):
        if(pos[1] != 1):
            unknownPlan = pivotTurning(pos,heading,posIm,headingIm,pivot,True) + unknownPlan
        else:
            unknownPlan = pivotTurning(pos,heading,posIm,headingIm,pivot,False) + unknownPlan
    else:
        if(pos[0] >= 4):
            if(heading[0] == F):
                unknownPlan = pivotTurning(pos,heading,posIm,headingIm,pivot,True) + unknownPlan
            elif(heading[0] == B):
                unknownPlan = pivotTurning(pos,heading,posIm,headingIm,pivot,False) + unknownPlan
        else:
            if(heading[0] == F):
                unknownPlan = pivotTurning(pos,heading,posIm,headingIm,pivot,False) + unknownPlan
            elif(heading[0] == B):
                unknownPlan = pivotTurning(pos,heading,posIm,headingIm,pivot,True) + unknownPlan


def searchAlgorithmEx():
    global unknownPlan,pos,heading,task,param
    if(unknownPlan == []):return 0
    task = unknownPlan[0][0]
    param = unknownPlan[0][1]
    if(haveToShoot == False):
        unknownPlan.pop(0)
        if(task == 'f'):
            if(look() == 1):#PSDの情報をみて１マス先に障害物があるとき
                posIm = pos.copy()
                headingIm = heading.copy()
                move(F,posIm,headingIm)
                detour(pos,heading,posIm)
            else:
                move(F,pos,heading)
        elif(task == 'r'):rotate(R,heading)
        elif(task == 'b'):move(B,pos,heading)
        elif(task == 'l'):rotate(L,heading)
    elif(haveToShoot == True):
        roundTrip(pos,heading,shootingPoint[0],shootingDir[0])




def spin():
    searchAlgorithmEx()
    return task,param
