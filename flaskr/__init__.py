import os
from flask import Flask,request
import json
# MARIADB DRIVER DEFINITION
class MariaDriver:
    def __init__(self,beaconid):
        import mysql.connector
        self.beaconid=beaconid
        self.mydb = mysql.connector.connect(host="localhost",user="projectQ",password="lyUTm1l9MWuxbPAu",database="projectQ")
        self.mycursor = self.mydb.cursor()
        query = "show tables;"
        self.mycursor.execute(query)
        result=self.mycursor
        flag_exists=False
        for i in result:
            if(i[0]==beaconid):
                flag_exists=True
                break
        if(flag_exists==False):
            query="CREATE TABLE `"+beaconid+"` (  `SlNo` int(11) NOT NULL,  `TIME` varchar(20) NOT NULL,  `SPEED` varchar(20) NOT NULL,  `LATITUDE` varchar(20) NOT NULL, `LONGITUDE` varchar(20) NOT NULL, `B_C` int(11) NOT NULL,  `S_ID` varchar(20) NOT NULL,  `S_C` int(11) NOT NULL,  `VALID` int(11) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;"
            self.mycursor.execute(query)
            result=self.mycursor
            for i in result:
                pass
            query="ALTER TABLE `"+beaconid+"`  ADD PRIMARY KEY (`SlNo`);"
            self.mycursor.execute(query)
            result=self.mycursor
            for i in result:
                pass
            query="ALTER TABLE `"+beaconid+"`  MODIFY `SlNo` int(11) NOT NULL AUTO_INCREMENT;"
            self.mycursor.execute(query)
            result=self.mycursor
            for i in result:
                pass
            query = "show tables;"
            self.mycursor.execute(query)
            result=self.mycursor
            flag_exists=False
            for i in result:
                if(i[0]==beaconid):
                    flag_exists=True
                    break
            if(flag_exists==False):
                raise Exception("TableCreateFailed")
    def Query(self,q):
        query = q
        self.mycursor.execute(query)
        result=self.mycursor
        return result
    def getLast(self,S_ID):
        for i in self.mycursor:
            pass
        query="SELECT * FROM "+self.beaconid+" where S_ID=\""+S_ID+"\" ORDER BY SlNo DESC LIMIT 1;"
        result=self.Query(query)
        last=None
        for i in result:
            last=i
            print(i)
        return last
    def getdirty(self,B_ID):
        for i in self.mycursor:
            pass
        query="SELECT DIRTY FROM dirty where B_ID=\""+B_ID+"\";"
        result=self.Query(query)
        data=int(result.next()[0])
        for i in self.mycursor:
            pass
        return data
    def insertValue(self,TIME,SPEED,LATITUDE,LONGITUDE,B_C,S_ID,S_C):
        dirty=self.getdirty(self.beaconid)
        last=self.getLast(S_ID)
        BeaconC,StrapC=None,None
        if(last!=None):
            BeaconC,StrapC=last[5],last[7]
        else:
            BeaconC,StrapC=0,0
        VALID=0
        if(int(BeaconC)<int(B_C) and int(StrapC)<int(S_C)):
            VALID=1
        query = "insert into "+self.beaconid+"  (TIME, SPEED, LATITUDE, LONGITUDE, B_C, S_ID, S_C, VALID) values (\""+TIME+"\",\""+SPEED+"\",\""+LATITUDE+"\",\""+LONGITUDE+"\","+str(B_C)+",\""+S_ID+"\","+str(S_C)+","+str(VALID)+");"
        #print(query)
        #                                                 Varchar       char          char               int         char       int
        for i in self.mycursor:
            pass
        print(type(TIME),TIME,type(SPEED),SPEED,type(LATITUDE),LATITUDE,type(LONGITUDE),LONGITUDE,type(B_C),B_C,type(S_ID),S_ID,type(S_C),S_C)
        result=self.Query(query)
        self.mydb.commit()
        return dirty
    def getPairing(self,B_ID):
        for i in self.mycursor:
            pass
        query="select key_value,mac_id from strap where S_ID in (select S_ID from Pairing where B_ID=\""+B_ID+"\");"
        result=self.Query(query)
        l=[[],[]]
        for i in result:
            l[0].append(i[1])
            l[1].append(i[0])
        query="update dirty set DIRTY=0 where B_ID=\""+B_ID+"\";"
        result=self.Query(query)
        for i in self.mycursor:
            pass
        self.mydb.commit()
        return l
# END MARIADB DRIVER
def create_app(test_config=None):
    # create and configure the app
    app = Flask(__name__, instance_relative_config=True)
    app.config.from_mapping(
        SECRET_KEY='dev',
        DATABASE=os.path.join(app.instance_path, 'flaskr.sqlite'),
    )

    if test_config is None:
        # load the instance config, if it exists, when not testing
        app.config.from_pyfile('config.py', silent=True)
    else:
        # load the test config if passed in
        app.config.from_mapping(test_config)

    # ensure the instance folder exists
    try:
        os.makedirs(app.instance_path)
    except OSError:
        pass

    # a simple page that says hello
    @app.route('/', methods=['GET', 'POST'])
    def hello():
        data = None
        if(request.method == "POST"):
            data = request.form
            if("test" in data.keys()):
                data = "Test worked, value we got was "+data["test"]
            else:
                z=["B_ID","TIME","SPEED","LATITUDE","LONGITUDE","B_C","S_ID","S_C"]
                flag = True
                for i in z:
                    if(i not in data):
                        flag=False
                        break
                if(flag):
                    B_ID=data["B_ID"]
                    TIME=data["TIME"]
                    SPEED=data["SPEED"]
                    LATITUDE=data["LATITUDE"]
                    LONGITUDE=data["LONGITUDE"]
                    B_C=data["B_C"]
                    S_ID=data["S_ID"]
                    S_C=data["S_C"]
                    driver = MariaDriver(B_ID)
                    if(driver.insertValue(TIME,SPEED,LATITUDE,LONGITUDE,B_C,S_ID,S_C)==0):
                        data="OK"
                    else:
                        data="DIRTY"
                elif("B_ID" in data.keys()):
                    #select key_value,mac_id from strap where S_ID in (select S_ID from Pairing where B_ID="beac1")
                    driver = MariaDriver(data["B_ID"])
                    l=driver.getPairing(data["B_ID"])
                    data=json.dumps(l)
                else:
                    data="Bad request!"
            #def insertValue(self,TIME,SPEED,LATITUDE,B_C,S_ID,S_C):
        else:
            data = request.args
        print(data)
        if(len(data)==0):
            return '''            
<form method="POST" action="/">
    Enter a value:<input type="text" name="test"></input><br>
    <input type="submit"></input>
</form>'''
        else:
            return str(data)

    return app
