#!/usr/local/bin/python3.6

import urllib.request
import json
import pprint
import calendar
import psycopg2
import os, sys
import syslog
import random
import time

from datetime import datetime, timedelta

class Time:
    def __init__(self, mixedTime):
        self.__timeInMinutes = self.parseMixedTime(mixedTime)

    def parseMixedTime(self, mixedTime):
        hours = int(mixedTime / 100)
        mins = mixedTime - hours * 100
        timeInMinutes = hours * 60 + mins
        return timeInMinutes
        
    def __lt__(self, other):
        if isinstance(other, Time):
            return self.__timeInMinutes < other.__timeInMinutes
        return NotImplemented

    def __gt__(self, other):
        if isinstance(other, Time):
            return self.__timeInMinutes > other.__timeInMinutes
        return NotImplemented

    def __gt__(self, min):
        return self.__timeInMinutes > min

    def __str__(self):
        hrs = int(self.__timeInMinutes / 60)
        mns = self.__timeInMinutes % 60
        s = str(hrs) + ":" + str(mns)
        return s

    def __sub__(self, other) :
        if isinstance(other, Time):
            return Time(self.__timeInMinutes - other.__timeInMinutes)
        return NotImplemented

    def __add__(self, mins):
        res = self.__timeInMinutes + mins
        hrs = int(res / 60)
        mns = res % 60
        res = hrs * 100 + mns
        return Time(res)

    def getMinutes(self):
        return self.__timeInMinutes

    def addMinutes(self, mins):
        self.__timeInMinutes = self.__timeInMinutes + mins

    def get(self):
        hrs = int(self.__timeInMinutes / 60)
        mns = self.__timeInMinutes % 60
        s = hrs * 100 + mns
        return s

def add_record_to_db(month, day, sunrise, sunset):
    insert = insert_template.format(local_zip, now.year, month, day, sunrise, sunset)
    syslog.syslog(syslog.LOG_PID, 'going to add new sunrise-sunset time to db {0} : {1}/{2}'.format(today), sunrise, sunset)
    cur = psql_conn.cursor()
    cur.execute(insert)
    psql_conn.commit()
    cur.close()

def get_cloud_index():
    url = 'https://api.openweathermap.org/data/2.5/weather?zip=98027,us&APPID=' + api_key
    content = urllib.request.urlopen(url).read()
    data = json.loads(content)

    return int(data['clouds']['all'])

def utc_to_local(utc_dt):
    # get integer timestamp to avoid precision lost
    timestamp = calendar.timegm(utc_dt.timetuple())
    local_dt = datetime.fromtimestamp(timestamp)
    assert utc_dt.resolution >= timedelta(microseconds=1)
    return local_dt.replace(microsecond=utc_dt.microsecond)

def get_srss_online(month, day):
    date = '{0}-{1:02d}-{2:02d}'.format(now.year, month, day)

    url = 'https://api.sunrise-sunset.org/json?lat=47.49638&lng=-121.99090&formatted=0&date=' + date

    content = urllib.request.urlopen(url).read()

    data = json.loads(content)

    if data['status'] == 'OK':
        sunrise = datetime.strptime(data['results']['sunrise'], '%Y-%m-%dT%H:%M:%S+00:00')
        sunrise = utc_to_local(sunrise)
        sunrise_hrs = int(sunrise.strftime('%H%M'))

        sunset = datetime.strptime(data['results']['sunset'], '%Y-%m-%dT%H:%M:%S+00:00')
        sunset = utc_to_local(sunset)
        sunset_hrs = int(sunset.strftime('%H%M'))

        add_record_to_db(month, day, sunrise_hrs, sunset_hrs)
        return {'sunrise':sunrise_hrs, 'sunset':sunset_hrs}
    else:
        raise ValueError('wrong date : ' + date)

def find_srss_for_date(month, day):
    sql = "SELECT sunrise, sunset FROM sunrise_sunset_times WHERE zip = {0} and month = {1} and day = {2} order by year desc".format(local_zip, month, day)
    cur = psql_conn.cursor()
    cur.execute(sql)
    rows = cur.fetchall()
    data = {}

    if len(rows) > 0:
        row = rows[0]
        data = {'sunrise':int(row[0]), 'sunset':int(row[1])}
        cur.close()
    else:
        data = get_srss_online(month, day)
        
    return data

debug = 0
if __name__=='__main__':
    for i in range(1, len(sys.argv)):
        if sys.argv[i][:2] == "-d": 
            debug = 1
            print(f'debug mode on')

psql_conn = psycopg2.connect(host="10.1.1.1",database="ccpro_noip_org", user="ccpro")

insert_template = "insert into sunrise_sunset_times (zip, year, month, day, sunrise, sunset) values({0}, {1}, {2}, {3}, {4}, {5})"
now = datetime.now()
local_zip = 98027
api_key = 'e172f5276171db59194c0d462804653e'

pp = pprint.PrettyPrinter(indent=4)

srss = find_srss_for_date(now.month, now.day)

psql_conn.close()

start_time = Time(655)
stop_time = Time(2345)

weekday = now.weekday()
if (weekday in [5,6,2]):
    r = random.randint(0, 15) - 5
    start_time = Time(730 + r)
    stop_time = Time(2330)

cloud_index = get_cloud_index()
delay_coef = lambda x: int((x + 1) / 30 + 1) # 50 gives index from 1 to 2, 40 gives from 1 to 3
dc = delay_coef(cloud_index)

syslog.syslog(syslog.LOG_PID, 'cloud index is {0}, delay coef is {1}'.format(cloud_index, dc))

time_offset = 5 * dc

if 1 == debug:
    print(f' sunrise: {srss["sunrise"]}, sunset: {srss["sunset"]}')

tm = time.localtime()
dst_offset = 0
if (tm.tm_isdst) :
    dst_offset = 60

if 1 == debug:
    print(f'daylight savings time offset {dst_offset}')

sunrise = Time(srss['sunrise'])
sunrise.addMinutes(time_offset + dst_offset)

total_off = Time(srss['sunrise'])
total_off.addMinutes(60)

if sunrise - start_time > 10:
    cmd = 'echo "/usr/bin/perl /usr/home/ccpro/projects/wifi-on-off/tplink_hs110_cmd.pl -command=on -ip=10.1.1.64" | at 0{0}'.format(start_time.get())
    print(cmd)
    if 0 == debug:
        syslog.syslog(syslog.LOG_PID, cmd)
        os.system(cmd)
    cmd = 'echo "/usr/bin/perl /usr/home/ccpro/projects/wifi-on-off/tplink_hs110_cmd.pl -command=off -ip=10.1.1.64" | at 0{0}'.format(sunrise.get())
    print(cmd)
    if 0 == debug:
        syslog.syslog(syslog.LOG_PID, cmd)
        os.system(cmd)

cmd = 'echo "/usr/bin/perl /usr/home/ccpro/projects/wifi-on-off/tplink_hs110_cmd.pl -command=off -ip=10.1.1.64" | at 0{0}'.format(total_off.get())
print(cmd)
if 0 == debug:
    syslog.syslog(syslog.LOG_PID, cmd)
    os.system(cmd)

sunset = Time(srss['sunset'])
sunset.addMinutes(-time_offset + dst_offset)

if stop_time - sunset > 10:
    cmd = 'echo "/usr/bin/perl /usr/home/ccpro/projects/wifi-on-off/tplink_hs110_cmd.pl -command=on -ip=10.1.1.64" | at {0}'.format(sunset.get())
    print(cmd)
    if 0 == debug:
        syslog.syslog(syslog.LOG_PID, cmd)
        os.system(cmd)
    cmd = 'echo "/usr/bin/perl /usr/home/ccpro/projects/wifi-on-off/tplink_hs110_cmd.pl -command=off -ip=10.1.1.64" | at {0}'.format(stop_time.get())
    print(cmd)
    if 0 == debug:
        syslog.syslog(syslog.LOG_PID, cmd)
        os.system(cmd)
