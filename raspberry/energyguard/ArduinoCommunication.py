import serial
import datetime
import time
import ast
import sqlite3

		
arduino = serial.Serial('/dev/ttyACM0',9600)
arduino.open()

# ------------------------
# First line can be crappy and must be discarded
# Find end of line
line = arduino.readline()
# ------------------------

# SELECT datetime('now', 'localtime');
# CREATE TABLE EnergyBufferTable (ID INTEGER PRIMARY KEY, TimeStamp DATE DEFAULT (datetime('now','localtime')), T1 NUMERIC, T2 NUMERIC, T3 NUMERIC, T4 NUMERIC, T5 NUMERIC, T6 NUMERIC,T7 NUMERIC,T8 NUMERIC, T9 NUMERIC, T10 NUMERIC, L1 NUMERIC, L2 NUMERIC, L3 NUMERIC, PWM_L1 NUMERIC, PWM_L2 NUMERIC, PWM_L3 NUMERIC, Import_kWh NUMERIC, Export_kWh NUMERIC, Pomp_CWU INTEGER, Pomp_500L INTEGER, SSR_3 INTEGER);
	
	# new db:
	# CREATE TABLE PompsControl (ID INTEGER PRIMARY KEY, TimeStamp DATETIME, T1 INTEGER, T2 INTEGER, T3 INTEGER, T4 INTEGER, T5 INTEGER, T6 INTEGER,T7 INTEGER, T8 INTEGER, T9 INTEGER, T10 INTEGER, Pomp_CWU INTEGER, Pomp_500L INTEGER);
	# CREATE TABLE Power (ID INTEGER PRIMARY KEY, TimeStamp DATETIME, L1 INTEGER, L2 INTEGER, L3 INTEGER, PWM_L1 INTEGER, PWM_L2 INTEGER, PWM_L3 INTEGER, SSR_3 INTEGER);

	
dbconn = sqlite3.connect('/home/pi/www/EnergyGuard.db')
dbconn.cursor().execute('PRAGMA journal_mode = WAL')   # It's required for simultaneous read and write access
c = dbconn.cursor()



while 1 :
		
		# arduino = serial.Serial("/dev/ttyAMA0", 9600, timeout=1) # open serial port
        try:
		
			line = arduino.readline()   # read whole line from Arduino
			# arduino.close()
			list = line.split ()		# convert the line into list
			
			# take each element from list and covert it to float or integer value
			t1 = ast.literal_eval(list[0]) 	# Temp. //1	28-0000052cecda   wyj. CO - Pompa Ciepla	Device Address: 28DAEC2C050000B9
			t2 = ast.literal_eval(list[1])	# Temp. //2	28-0000052cee11	  wyj. CWU - Pompa Ciepla	Device Address: 2811EE2C0500005D
			t3 = ast.literal_eval(list[2]) 	# Temp. //3	28-0000052dbd7f   buffor 300l - gora		Device Address: 287FBD2D050000D9
			t4 = ast.literal_eval(list[3])	# Temp. //4	28-0000052e0139	  wyj. Kaloryfery			Device Address: 2839012E050000F8
			t5 = ast.literal_eval(list[4])	# Temp. //5	28-0000052ce62e	  wej. Kaloryfery			Device Address: 282EE62C050000AA
			t6 = ast.literal_eval(list[5]) 	# Temp. //6	28-0000052dcfd7	  CWU czerwony 300l - gora	Device Address: 28D7CF2D050000CA
			t7 = ast.literal_eval(list[6])	# Temp. //7	28-0000052df474	  CWU czerwony 300l - dol	Device Address: 2874F42D0500003B
			t8 = ast.literal_eval(list[7]) 	# Temp. //8	28-0000052d5459	  CO 500l  - dol 			Device Address: 2859542D050000F4
			t9 = ast.literal_eval(list[8])	# Temp. //9	28-0000052ceeea	  CO 500l  - gora			Device Address: 28EAEE2C050000D7
			t10 = ast.literal_eval(list[9]) # Temp. /10 28-0000052cfe51	  temperatura zewnetrzna	Device Address: 2851FE2C05000054

			P_CWU = ast.literal_eval(list[10])	# Pomp CWU on/off 
			P_500 = ast.literal_eval(list[11])	# Pomp buffer 500 l on/off 
			
			l1 = ast.literal_eval(list[12])	# Power in Watts
			l2 = ast.literal_eval(list[13])	# Power in Watts
			l3 = ast.literal_eval(list[14])	# Power in Watts
			
			pwm_l1 = ast.literal_eval(list[15])	# PWM signal in Watts 
			pwm_l2 = ast.literal_eval(list[16])	# PWM signal in Watts
			pwm_l3 = ast.literal_eval(list[17])	# PWM signal in Watts
					
			SSR = ast.literal_eval(list[18])	# 3-phase SSR  on/off 
			
			
			
			# Insert a row of data
			# c.execute("INSERT INTO EnergyBufferTable VALUES (NULL,datetime('now', 'localtime'),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?))",  list)
			
			
			c.execute("INSERT INTO PompsControl VALUES(NULL,datetime('now', 'localtime'), (?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?),(?))", list[0:11])
			c.execute("INSERT INTO Power VALUES(NULL,datetime('now', 'localtime'), (?),(?),(?),(?),(?),(?),(?))", list[12:18])

			
			# Save (commit) the changes
			dbconn.commit()
			
					
			# Pump control 'isNoon'
			# 0 - All pumps switch off
			# 1 - swtich on CWU pump only
			# 2 - swtich on 500 pump only
			# 3 - switch on both pumps
			
			isNoon =  datetime.datetime.now().hour
			
			if isNoon == 13:				# 13 o'clock (1 p.m)
				isNoon = 1					# run CWU pump
				
			else if isNoon == 19:			# 19 o'clock (7 p.m)
				isNoon = 2					# run 500 pump
			else:
				isNoon = 0
				
			
			arduino.write(str(isNoon))
			# time.sleep(1)
        	
        except serial.serialutil.SerialException:  
			print 'Leonardo has been disconnected'
			
			dbconn.close()
			arduino.close()
			
			break							# Ignore cable disconnection
					
        except KeyboardInterrupt:		
			
			# Interrupt program execution on Ctrl-C
			# Just be sure any changes have been committed
			
			print 'Interruption signal from keyboard has been received'
			
			dbconn.close()
			arduino.close()
			
			break
			
			
# python script running in background	
# nohup python run-service.py &

  
  
