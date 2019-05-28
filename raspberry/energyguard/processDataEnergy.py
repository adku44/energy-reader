import datetime
import time
import sqlite3




# SELECT datetime('now', 'localtime');
# CREATE TABLE EnergyBufferTable (ID INTEGER PRIMARY KEY, TimeStamp DATE DEFAULT (datetime('now','localtime')), T1 NUMERIC, T2 NUMERIC, T3 NUMERIC, T4 NUMERIC, T5 NUMERIC, T6 NUMERIC,T7 NUMERIC,T8 NUMERIC, T9 NUMERIC, T10 NUMERIC, L1 NUMERIC, L2 NUMERIC, L3 NUMERIC, PWM_L1 NUMERIC, PWM_L2 NUMERIC, PWM_L3 NUMERIC, Import_kWh NUMERIC, Export_kWh NUMERIC, Pomp_CWU INTEGER, Pomp_500L INTEGER, SSR_3 INTEGER);
	
	
# CREATE TABLE EnergyDaily (ID INTEGER PRIMARY KEY, TimeStamp DATE , DeltaImport INTEGER, DeltaExport INTEGER);
# CREATE TABLE EnergyMonthly (ID INTEGER PRIMARY KEY, TimeStamp DATE , DeltaImport INTEGER, DeltaExport INTEGER);
	
dbconn_newDB = sqlite3.connect('/home/pi/www/Data-EB.db')
#dbconn_newDB.cursor().execute()  
c_newDB = dbconn_newDB.cursor()



try:
		g = sqlite3.connect('/home/pi/www/EnergyBuffer.db')
		g.cursor().execute('PRAGMA journal_mode = WAL') # It's required for simultaneous read and write access
		cur = g.cursor()
	
	
except sqlite3.OperationalError:
		flash("There is no database connected")
		# break

	
	
	


# cur.execute("select TimeStamp, %s, %s from EnergyBufferTable where TimeStamp > date('now');" % (temp1, temp2))



# poststime = []
# posts1 = 0.0
# posts2 = 0.0


		
datefrom = datetime.datetime(2016,1,1)    # start date

# days

# for i in range(4): 
#		dateto = datefrom + datetime.timedelta(day=1)
		
#		cur.execute("select date(TimeStamp), max(Import_kWh) - min(Import_kWh), max(Export_kWh) - min(Export_kWh) from EnergyBufferTable where TimeStamp BETWEEN '%s' AND '%s';" % (datefrom, dateto))

#		row = cur.fetchone()
		
#		if  not (row[0] is None):
				# Insert a row of data
#				print(i)
#				print(row[0])  
#				print(row[1])  
#				print(row[2])  
#				c_newDB.execute("INSERT INTO EnergyDaily VALUES (NULL,(?),(?),(?))",  (row[0], int(row[1]), int(row[2]) ))
		
#		datefrom = dateto


# months

for i in range(2): 
	
		if datefrom.month + 1 > 12:
				dateto =  datetime.date(datefrom.year + 1, 1, datefrom.day)
		else:
				dateto =  datetime.date(datefrom.year, datefrom.month+1, datefrom.day)
		
	
		cur.execute("select date(TimeStamp), max(Import_kWh) - min(Import_kWh), max(Export_kWh) - min(Export_kWh) from EnergyBufferTable where TimeStamp BETWEEN '%s' AND '%s';" % (datefrom, dateto))

		row = cur.fetchone()
		
		if  not (row[0] is None):
				# Insert a row of data
				print(i)
				print(row[0])  
				print(row[1])  
				print(row[2])  
				c_newDB.execute("INSERT INTO EnergyMonthly VALUES (NULL,(?),(?),(?))",  (row[0], int(row[1]), int(row[2]) ))
		
		datefrom = dateto

		
# select date(TimeStamp), max(Import_kWh) - min(Import_kWh), max(Export_kWh) - min(Export_kWh) from EnergyBufferTable where TimeStamp BETWEEN '2016-02-01' AND '2016-02-02';



# Save (commit) the changes
dbconn_newDB.commit()

g.close()
dbconn_newDB.close()


