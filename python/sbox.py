key=[0x62143c09,
     0x5715e8a8,
     0x9673c160,
     0xa1bc4299,
     0x258fffb6,
     0x6db3217c,
     0xe9941e03,
     0xd921b034,
     0x7d0b6575,
     0x1977ead0] # 320bit key can encrypt 40 data char min
sbox=[[[11,0],[31,1]], # 1
      [[9,1],[26,0]],  # 2
      [[10,1],[2,1]],  # 3
      [[4,1],[7,0]],   # 4
      [[14,0],[29,1]], # 5
      [[27,1],[11,0]], # 6
      [[8,0],[21,1]],  # 7
      [[12,1],[8,0]],  # 8
      [[6,1],[22,0]],  # 9
      [[32,1],[6,1]],  # 10
      [[7,1],[30,0]],  # 11
      [[25,0],[18,0]], # 12
      [[15,1],[28,0]], # 13
      [[3,0],[16,1]],  # 14
      [[22,0],[19,0]], # 15
      [[18,1],[17,0]], # 16
      [[2,1],[32,0]],  # 17
      [[1,0],[24,1]],  # 18 
      [[24,0],[14,1]], # 19
      [[28,0],[10,0]], # 20
      [[26,0],[9,1]],  # 21
      [[13,1],[13,0]], # 22
      [[30,1],[1,1]],  # 23
      [[17,0],[20,1]], # 24
      [[19,1],[12,0]], # 25
      [[5,1],[5,0]],   # 26
      [[21,0],[23,1]], # 27
      [[23,1],[25,0]], # 28
      [[31,0],[15,1]], # 29
      [[29,0],[3,0]],  # 30
      [[20,0],[27,1]], # 31
      [[16,0],[4,0]]]  # 32
# above is a list [ [ [falseIndex,setval], [trueIndex,setval] ] ]
swapbox=[[[23,1],[20,32]],  #1
         [[7,8],[6,5]],     #2
         [[8,29],[19,22]],  #3
         [[29,11],[24,27]], #4
         [[9,10],[18,2]],   #5
         [[25,26],[17,26]], #6
         [[4,17],[9,23]],   #7
         [[6,19],[3,29]],   #8
         [[2,24],[2,18]],   #9
         [[16,30],[10,11]], #10
         [[27,28],[13,20]], #11
         [[18,32],[4,12]],  #12
         [[15,20],[15,16]], #13
         [[3,22],[14,21]],  #14
         [[5,6],[1,30]],    #15
         [[13,25],[7,31]],  #16
         [[14,31],[26,3]],  #17
         [[21,12],[11,17]], #18
         [[31,2],[12,24]],  #19
         [[11,23],[5,28]],  #20
         [[32,3],[8,14]],   #21
         [[10,13],[25,4]],  #22
         [[12,14],[22,6]],  #22
         [[19,21],[27,19]], #23
         [[20,4],[23,25]],  #24
         [[17,15],[16,13]], #25
         [[22,5],[21,7]],   #26
         [[24,7],[28,8]],   #27
         [[26,27],[32,1]],  #28
         [[28,9],[31,10]],  #29
         [[1,18],[30,15]],  #30
         [[30,16],[29,9]],  #31
         [[16,4],[8,12]]]   #32
# above is a list [ [ [falseSwap1,falseSwap2], [trueSwap1,trueSwap2] ] ]
def findrepeat(resolve,resolve1=0):
    l=[i for  i in range(1,33)]
    for i in range(32):
        try:
            l.remove(swapbox[i][resolve][resolve1])
        except ValueError:
            print("Conflict",swapbox[i][resolve][resolve1],"pos",i)
    l1=[i for  i in range(1,33)]
    for i in range(32):
        try:
            l1.remove(swapbox[i][resolve][resolve1])
        except ValueError:
            print("Conflict",swapbox[i][resolve][resolve1],"pos",i)
            print(swapbox[i])
            print("Replacable Values",l)
            z=int(input("Enter index"))
            swapbox[i][resolve][resolve1]=l[z]
            l.pop(z)
    print(l)
def setbit(val,pos,setval):
    if(setval==0):
        val = val & (~(1<<pos))
    else:
        val = val | (1<<pos)
    return val
def getbit(val,pos):
    if((val&(1<<pos))==(1<<pos)):
        return 1
    else:
        return 0
def swapbits(val,index1,index2):
    bit1=0
    bit2=0
    temp=1<<index1
    if(temp == (val & temp)):
        bit1=1
    temp=1<<index2
    if(temp == (val & temp)):
        bit2=1
    val=setbit(val,index1,bit2)
    val=setbit(val,index2,bit1)
    return val
def seedKey(seed,Key):
    key=Key
    for i in range(32):
        if(getbit(seed,i)==True):
            if(key%8==0):
                key=setbit(key,sbox[i][1][0]-1,sbox[i][1][1])
            key=swapbits(key,swapbox[i][1][0]-1,swapbox[i][1][1]-1)
        else:
            if(key%8==0):
                key=setbit(key,sbox[i][0][0]-1,sbox[i][0][1])
            key=swapbits(key,swapbox[i][0][0]-1,swapbox[i][0][1]-1)
    return key
for i in key:
    print("seed=",100,"key=",i,seedKey(100,i))
for i in range(100):
    for j in range(10):
        print(seedKey(2100280,key[j]),end="\t")
        key[j]=seedKey(6782354,key[j])
    print()