from flask import Flask, render_template
from apscheduler.schedulers.background import BackgroundScheduler
import sqlite3
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt
from matplotlib import dates as mpl_dates
import time
from datetime import datetime
import tzlocal
from pytz import timezone
import os
import sys


app = Flask(__name__)

last_val={}
stats={}

def generate_graph(db, sensors, hours, filename):
    stamp = int(time.time())-hours*60*60
    for i, s in enumerate(sensors.keys()):
        x_time = []
        y_value = []

        cur = db.cursor()
        cur.execute("SELECT timestamp, value FROM temperature WHERE romcode IS ? AND timestamp >= ?", (s, stamp))
        rows = cur.fetchall()

        for row in rows:
            local_timezone = tzlocal.get_localzone()
            x_time.append(datetime.fromtimestamp(row[0], local_timezone))
            y_value.append(row[1])
        plt.plot_date(x_time, y_value, linestyle='solid', marker='None', label=sensors[s])
        date_format = mpl_dates.DateFormatter('%H:%M', tz=tzlocal.get_localzone())
        plt.gca().xaxis.set_major_formatter(date_format)

    plt.gca().set_facecolor("#FFFADE")
    plt.legend()
    plt.savefig("static/%s.png" % filename, facecolor="#FFFADE")
    plt.close()

def get_last(db, sensors):
    cur = db.cursor()
    cur.execute("SELECT romcode, value FROM temperature WHERE timestamp = (SELECT MAX(timestamp) FROM temperature)")
    rows = cur.fetchall()
    for i in range(len(sensors)):
        last_val[sensors[rows[i][0]]] = rows[i][1]
    last_val['diff'] = abs(last_val['inside'] - last_val['outside'])

def generate_stats(db, sensors, hours):
    stamp = int(time.time())-hours*60*60
    values = [{},{},{}]
    for i, s in enumerate(sensors.keys()):
        cur = db.cursor()
        cur.execute("SELECT romcode, AVG(value), MAX(value), MIN(value) FROM temperature WHERE romcode IS ? AND timestamp >= ?", (s, stamp))
        rows = cur.fetchall()
        print(rows)
        values[0][sensors[rows[0][0]]] = round(rows[0][1], 3)
        values[1][sensors[rows[0][0]]] = round(rows[0][2], 3) 
        values[2][sensors[rows[0][0]]] = round(rows[0][3], 3)

    values[0]['diff'] = round(abs(values[0]['outside'] - values[0]['inside']), 3)
    values[1]['diff'] = round(abs(values[1]['outside'] - values[1]['inside']), 3) 
    values[2]['diff'] = round(abs(values[2]['outside'] - values[2]['inside']), 3)
    stats[hours] = values

def prepare_data():
    sensors = {"19011432674a2528":"inside", "cf011432f0e8a028":"outside"}
    try:
        db = sqlite3.connect("../tempdata.db");
    except Error as e:
        print(e)
        return
    generate_graph(db, sensors, 12, "graph")
    generate_graph(db, sensors, 24, "graph1")
    get_last(db, sensors)
    generate_stats(db, sensors, 12)
    generate_stats(db, sensors, 24)
    db.close();

scheduler = BackgroundScheduler(daemon=True)
scheduler.add_job(func=prepare_data, trigger="interval", seconds=60)
scheduler.start()

@app.after_request
def add_header(r):
    r.headers["Cache-Control"] = "no-cache, no-store, must-revalidate, public, max-age=0"
    r.headers["Pragma"] = "no-cache"
    r.headers["Expires"] = "0"
    return r

@app.route('/')
def index():
    return render_template('index.html', outside=last_val['outside'], inside=last_val['inside'], \
            diff=last_val['diff'], oavg12=stats[12][0]['outside'], iavg12=stats[12][0]['inside'], davg12=stats[12][0]['diff'], \
            omax12=stats[12][1]['outside'], imax12=stats[12][1]['inside'], dmax12=stats[12][1]['diff'],\
            omin12=stats[12][2]['outside'], imin12=stats[12][2]['inside'], dmin12=stats[12][2]['diff'],\
            oavg24=stats[24][0]['outside'], iavg24=stats[24][0]['inside'], davg24=stats[24][0]['diff'],\
            omax24=stats[24][1]['outside'], imax24=stats[24][1]['inside'], dmax24=stats[24][1]['diff'],\
            omin24=stats[24][2]['outside'], imin24=stats[24][2]['inside'], dmin24=stats[24][2]['diff']);

if __name__ == '__main__':
    if (len(sys.argv) != 2):
        print("usage: python3 main.py <dirpath>")
        exit(-1);
    if (os.chdir(sys.argv[1])):
        print("failed to change directory")
        exit(-1);
    prepare_data()
    app.run(host='0.0.0.0')

