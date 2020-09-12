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
            query="CREATE TABLE `"+beaconid+"` (  `SlNo` int(11) NOT NULL,  `TIME` varchar(20) NOT NULL,  `SPEED` varchar(20) NOT NULL,  `LATITUDE` varchar(20) NOT NULL,  `B_C` int(11) NOT NULL,  `S_ID` varchar(20) NOT NULL,  `S_C` int(11) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;"
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
    def insertValue(self,TIME,SPEED,LATITUDE,B_C,S_ID,S_C):
        query = "insert into "+self.beaconid+"  (TIME, SPEED, LATITUDE, B_C, S_ID, S_C) values (\""+TIME+"\",\""+SPEED+"\",\""+LATITUDE+"\","+str(B_C)+",\""+S_ID+"\","+str(S_C)+");"
        #                                                 Varchar       char          char               int         char       int
        self.Query(query)
        self.mydb.commit()