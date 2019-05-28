from flask import Flask, render_template, flash
#from flask_bootstrap import Bootstrap
import sqlite3
 
app = Flask(__name__)


@app.route('/')
def index():
    return 'Hello Adku!!'



@app.route('/today')
def today(chartID = 'chart_ID', chart_type = 'line', chart_height = 500):

	poststime = []
	postsL1 = []
	postsL2 = []
	postsL3 = []
	
	try:
		g = sqlite3.connect('/home/pi/www/EnergyBuffer.db')
		g.cursor().execute('PRAGMA journal_mode = WAL') # It's required for simultaneous read and write access
		cur = g.cursor()
		cur.execute("select TimeStamp, L1, L2, L3 from EnergyBufferTable where TimeStamp > date('now');")
		for row in cur.fetchall():
			poststime.append(str(row[0])[11:-3])  	# Cut string to display only time in format hh:mm
			postsL1.append(row[1])
			postsL2.append(row[2])
			postsL3.append(row[3])
	
		g.close()	
	
	except sqlite3.OperationalError:
		flash("There is no database connected")
	
	series = [{"name": 'L1', "data": postsL1}, {"name": 'L2', "data": postsL2}, {"name": 'L3', "data": postsL3}]
		
	xAxis = {"categories": poststime}
	yAxis = {"title": {"text": 'Power [W]'}}
	
	return render_template('index2.html', series=series, xAxis=xAxis)

@app.route('/now')
def now():

	poststime = []
	postsL1 = []
	postsL2 = []
	postsL3 = []
	
	try:
		g = sqlite3.connect('/home/pi/www/EnergyBuffer.db')
		g.cursor().execute('PRAGMA journal_mode = WAL') # It's required for simultaneous read and write access
		cur = g.cursor()
		cur.execute("select TimeStamp, L1, L2, L3 from EnergyBufferTable where TimeStamp > datetime('now', '-1 hour');")
		for row in cur.fetchall():
			poststime.append(str(row[0])[11:-3])  	# Cut string to display only time in format hh:mm
			postsL1.append(row[1])
			postsL2.append(row[2])
			postsL3.append(row[3])
	
		g.close()	
	
	except sqlite3.OperationalError:
		flash("There is no database connected")
	
	series = [{"name": 'L1', "data": postsL1}, {"name": 'L2', "data": postsL2}, {"name": 'L3', "data": postsL3}]
		
	xAxis = {"categories": poststime}
	yAxis = {"title": {"text": 'Power [W]'}}
	
	return render_template('index2.html', series=series, xAxis=xAxis)
 

	
@app.route('/<temp1>-<temp2>')
def temperature(temp1, temp2):

	poststime = []
	postsL1 = []
	postsL2 = []
	postsL3 = []
	
	try:
		g = sqlite3.connect('/home/pi/www/EnergyBuffer.db')
		g.cursor().execute('PRAGMA journal_mode = WAL') # It's required for simultaneous read and write access
		cur = g.cursor()
		
		cur.execute("select TimeStamp, %s, %s from EnergyBufferTable where TimeStamp > date('now');" % (temp1, temp2))
		for row in cur.fetchall():
			poststime.append(str(row[0])[11:-3])  	# Cut string to display only time in format hh:mm
			postsL1.append(row[1])
			postsL2.append(row[2])
			#postsL3.append(row[3])
	
		g.close()	
	
	except sqlite3.OperationalError:
		flash("There is no database connected")
	
	#series = [{"name": 'L1', "data": postsL1}, {"name": 'L2', "data": postsL2}, {"name": 'L3', "data": postsL3}]
	
		
	series = [{"name": 'T1', "data": postsL1}, {"name": 'T2', "data": postsL2} ]
	xAxis = {"categories": poststime}
	yAxis = {"title": {"text": 'Power [W]'}}
	
	return render_template('index2.html', series=series, xAxis=xAxis)
        
		
		
 
@app.route('/yesterday')
def yesterday(chartID = 'chart_ID', chart_type = 'line', chart_height = 500):

	poststime = []
	postsL1 = []
	postsL2 = []
	postsL3 = []
	
	try:
		g = sqlite3.connect('/home/pi/www/EnergyBuffer.db')
		g.cursor().execute('PRAGMA journal_mode = WAL') # It's required for simultaneous read and write access
		cur = g.cursor()
		cur.execute("select TimeStamp, L1, L2, L3 from EnergyBufferTable where TimeStamp > date('now', '-1 day') and TimeStamp < date('now');")
		for row in cur.fetchall():
			poststime.append(str(row[0])[11:-3])  	# Cut string to display only time in format hh:mm
			postsL1.append(row[1])
			postsL2.append(row[2])
			postsL3.append(row[3])
	
		g.close()	
	
	except sqlite3.OperationalError:
		flash("There is no database connected")
	
	chart = {"renderTo": chartID, "type": chart_type, "height": chart_height,}
	series = [{"name": 'L1', "data": postsL1}, {"name": 'L2', "data": postsL2}, {"name": 'L3', "data": postsL3}]
	
	title = {"text": 'Power Lines'}
	xAxis = {"categories": poststime}
	yAxis = {"title": {"text": 'Power [W]'}}
	return render_template('index2.html', chartID=chartID, chart=chart, series=series, title=title, xAxis=xAxis, yAxis=yAxis)
	
if __name__ == '__main__':
    app.debug = True
    app.run('192.168.202.102')

