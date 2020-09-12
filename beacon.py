#!/usr/bin/env python3
from threading import Thread
import gc
import requests
import time
from datetime import datetime
import json
url="https://abhiramshibu.tuxforums.com:5000"
beacon_id="beac1"
recvBuff=["",[]]
messageCount=0
#Global functions definitions  
def incrementCounter():
    counter=-1
    try:
        f=open("/home/user/.counter","r")
        counter=int(f.read())
        f.close()
        f=open("/homee/user/.counter","w")
    except:
        f=open("/home/user/.counter","w")
    counter+=1
    f.write(str(counter))
    f.close()
    return counter
def buffInit():
    global recvBuff
    recvBuff=["",[]]
def readData(handle, value):
    global buffInit
    global messageCount
    recvBuff[0]+=value.decode()
    recvBuff[1].append(messageCount)
    #print(messageCount,value)
    messageCount+=1
class encdata:
    def __init__(self,recvBuff):
        self.bytedata=None
        self.recvBuff=recvBuff
        self.message=None
        buffInit()
    def parseVersion(self):
        return self.recvBuff[0],self.recvBuff[1]
    def parse(self):
        recvBuff=self.recvBuff
        if(self.bytedata!=None):
            return self.bytedata,recvBuff[1]
        else:
            data=recvBuff[0].split("-")
            data="".join([chr(int(i,16)) for i in data])
            self.bytedata=data.encode()
            return self.bytedata,recvBuff[1]
    def dec(self,key):
        pos=0
        if(self.message!=None):
            return self.message
        self.message=b""
        for i in self.bytedata:
            self.message+=chr(i ^ key[pos]).encode()
            pos+=1
            pos=pos%len(key)
        return self.message
class Beacon_BT:
    def __init__(self,mac,uuid="0000ffe1-0000-1000-8000-00805f9b34fb"):
        import pygatt
        import time
        self.time=time
        self.pygatt = pygatt
        self.adapter = pygatt.GATTToolBackend()
        self.adapter.start()
        self.uuid = uuid         #UUID of the service
        self.mac = mac                            #MAC of the strap
        self.device = self.adapter.connect(mac)
        self.device.subscribe(uuid,callback=readData)
    def getVersion(self):
        global recvBuff
        self.device.char_write(self.uuid,b'VERSION')
        self.time.sleep(0.1)
        a=encdata(recvBuff=recvBuff)
        x,y=a.parseVersion()
        return x
    def Encrypt(self,data):
        global recvBuff
        data=("ENC"+data).encode()
        self.device.char_write(self.uuid,data)
        self.time.sleep(0.2)
        dataunit=encdata(recvBuff=recvBuff)
        dataunit.parse()
        self.dataunit=dataunit
        return dataunit.bytedata
    def reset(self):
        self.__init__(self.mac,self.uuid)
    def disconnect(self):
        self.device.disconnect()
    def reconnect(self):
        self.device = self.adapter.connect(mac)
        self.device.subscribe(uuid,callback=readData)
class GPSPipe:
    def __init__(self,file="/tmp/gpsdata"):
        self.f=file
        self.t = Thread(target=self.run)
        self.active=True
        self.t.start()
    def run(self):
        while self.active:
            self.file=open(self.f)
            datain=self.file.readline()
            self.file.close()
            if(datain=="" or (not "Time" in datain)):
                continue
            self.data=datain.split("\t")
            d = {}
            bypass_flag=False
            for i in self.data:
                try:
                    z=i.split("=")
                    d[z[0]]=z[1].strip()
                except:
                    pass
            if(bypass_flag):
                #print("Bypassing, invalid ->",d)
                continue
            self.data=d
            #print(d)
    def stop(self):
        self.active=False
        self.t.join()
    def __del__(self):
        self.active=False
def refreshDB():
    global strapmacs
    global strapkeys
    r = requests.post(url, data = {'B_ID':beacon_id})
    data=json.loads(r.content.decode())
    strapmacs=data[0]
    strapkeys=data[1]
    #print(strapmacs,strapkeys)
    #l[[mac,mac,mac],[key,key,key]]
def run(strap,key):
    try:
        latitude=gpsPipe.data["Latitude"]
        longitude=gpsPipe.data["Longitude"]
        speed=gpsPipe.data["Speed"]
        _time= gpsPipe.data["Time"]
        data=strap.Encrypt(latitude+","+longitude+","+_time)
        data=strap.dataunit.dec(key.encode()).decode()
        data=data.split(",")
        lat = data[0]
        lng = data[1]
        l=[]
        for i in range(len(data[2])-1,-1,-1):
            if(data[2][i]==':'):
                l.append(i)
            if(len(l)==2):
                break
        datetime = data[2][:l[1]]
        ctrstrp = data[2][l[0]+1:]
        idstrp = data[2][l[1]+1:l[0]]
        valid=False
        if(_time==datetime.strip() and lat == latitude and lng == longitude):
            valid=True
        r = requests.post(url, data = {'B_ID':beacon_id,"TIME":datetime,"SPEED":speed,"LATITUDE":lat,"LONGITUDE":lng,"B_C":incrementCounter(),"S_ID":idstrp,"S_C":int(ctrstrp)})
        result=r.content.decode()
        # if(result=="DIRTY"):
        #     refreshDB()
        #     print("Refreshed")
        # else:
        #     print("No need to refresh")
        print(lat,lng,datetime,ctrstrp,idstrp)
    except Exception as e:
        print(e)
        pass
strapkeys=[]
strapmacs=[]
while True:
    try:
        refreshDB()
        break
    except Exception as e:
        print(e)
gpsPipe = GPSPipe()
straps = [Beacon_BT(i) for i in strapmacs]
try:
    while(True):
        timenow=datetime.now()
        for i in range(len(straps)):
            run(straps[i],strapkeys[i])
        td=datetime.now()-timenow
        if(td.total_seconds()<60):
            time.sleep(60-td.total_seconds())
except KeyboardInterrupt:
    print("Bye..")
except Exception as e:
    print(e)
gpsPipe.stop()
print("Terminated")
