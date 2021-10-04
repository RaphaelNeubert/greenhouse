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


app = Flask(__name__)

last_val={}

def generate_graph(db, sensors, hours, filename):
    for i, s in enumerate(sensors.keys()):
        x_time = []
        y_value = []

        stamp = int(time.time())-hours*60*60
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
def generate_stats(db, sensors):
    cur = db.cursor()
    cur.execute("SELECT romcode, value FROM temperature WHERE timestamp = (SELECT MAX(timestamp) FROM temperature)")
    rows = cur.fetchall()
    for i in range(len(sensors)):
        last_val[sensors[rows[i][0]]] = rows[i][1]
    last_val['diff'] = abs(last_val['inside'] - last_val['outside'])

def prepare_data():
    sensors = {"19011432674a2528":"inside", "cf011432f0e8a028":"outside"}
    #connect to db
    try:
        db = sqlite3.connect("../tempdata.db");
    except Error as e:
        print(e)
    generate_graph(db, sensors, 12, "graph")
    generate_graph(db, sensors, 24, "graph1")
    generate_stats(db, sensors)
    db.close();

scheduler = BackgroundScheduler(daemon=True)
scheduler.add_job(func=prepare_data, trigger="interval", seconds=60)
scheduler.start()

@app.route('/')
def index():
    return render_template('index.html', outside=last_val['outside'], inside=last_val['inside'], diff=last_val['diff']);

if __name__ == '__main__':
    prepare_data()
    app.run(host='0.0.0.0')

