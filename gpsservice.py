#!/usr/bin/env python3
import serial
import os
import time
try:
	import pynmea2
except:
	os.system("pip3 install pynmea2")
import pynmea2
ser = serial.Serial("/dev/serial0",9600,timeout=1)
#l=[]
if(not os.path.exists("/tmp/gpsdata")):
	os.mkfifo("/tmp/gpsdata")
#count=0
while True:
	#time.sleep(0.001)
	try:
		data=ser.readline().decode()
		if("unknown msg*58" in data):
			ser.close()
			ser = serial.Serial("/dev/serial0",9600,timeout=1)
			continue
		if("$GPRMC" in data):
			f=open("/tmp/gpsdata","w")
			#f.write("Time=")
			#f.flush()
			#os.system("clear")
			#print("\n".join(l))
			#data=data.split(",")
			nmeaobj=None
			try:
				nmeaobj= pynmea2.parse(data,check=True)
			except Exception as e:
				f.write(str(e)+"\n");
				f.close
				continue
			datetime=str(nmeaobj.datetime)
			lat=str(nmeaobj.latitude)
			long=str(nmeaobj.longitude)
			speed=str(nmeaobj.spd_over_grnd)
			#print("Time="+str(time)+"\tLatitude="+str(lat)+"\tLongitude="+str(long)+"\tSpeed="+str(speed)+"\tDate="+str(date))
			f.write("Time="+str(datetime)+"\tLatitude="+str(lat)+"\tLongitude="+str(long)+"\tSpeed="+str(speed))
			f.write("\n")
			f.flush()
			f.close()
			#print(count)
			#count+=1
			#l=[]
			#time.sleep(1)
		#l.append(data)
	except KeyboardInterrupt:
		print("Bye..")
		exit(0)
	except:
		#print("Some exception occured but who cares for those things?")
		while True:
			try:
				ser.close()
				ser = serial.Serial("/dev/serial0",9600,timeout=1)
				break
			except:
				print("Exception of exception, not good!")
