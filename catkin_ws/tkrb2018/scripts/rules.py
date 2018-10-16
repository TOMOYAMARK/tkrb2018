
###clockwise###
F = 0
R = 1
B = 2
L = 3

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

task = 's'
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


def spin():
    searchAlgorithm()
    return task,param
