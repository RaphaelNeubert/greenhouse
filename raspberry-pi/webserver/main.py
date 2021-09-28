#!/bin/python
from flask import Flask, render_template
from apscheduler.schedulers.background import BackgroundScheduler
import sqlite3
from matplotlib import pyplot as plt


app = Flask(__name__)

def generate_graph(db):
    x_time = []
    y_value = []
    cur = db.cursor()
    cur.execute("SELECT timestamp, value FROM temperature WHERE romcode IS 'cf011432f0e8a028'")
    #cur.execute("SELECT timestamp, value FROM temperature WHERE romcode IS '19011432674a2528'")
    rows = cur.fetchall()

    for row in rows:
        x_time.append(row[0])
        y_value.append(row[1])
    plt.plot(x_time, y_value, label='Sensor 1')
    plt.savefig('graph.png')

def prepare_data():
    #connect to db
    try:
        db = sqlite3.connect("../tempdata.db");
    except Error as e:
        print(e)
    generate_graph(db)
    db.close();

scheduler = BackgroundScheduler(daemon=True)
scheduler.add_job(func=prepare_data, trigger="interval", seconds=2)
scheduler.start()

@app.route('/')
def index():
    return render_template('index.html');

if __name__ == '__main__':
    app.run()
