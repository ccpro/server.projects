#!/usr/local/bin/python3.6

import urllib.request
import json
import pprint
import calendar
import psycopg2
import os

from datetime import datetime, timedelta

def add_record_to_db(month, day, sunrise, sunset):
    insert = insert_template.format(local_zip, now.year, month, day, sunrise, sunset)
    cur = psql_conn.cursor()
    cur.execute(insert)
    psql_conn.commit()
    cur.close()

def get_cloud_index():
    url = 'https://api.openweathermap.org/data/2.5/weather?zip=98027,us&APPID=' + api_key
    content = urllib.request.urlopen(url).read()
    data = json.loads(content)

    return int((data['clouds']['all'] + 1) / 40 + 1)

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
    sql = "SELECT sunrise, sunset FROM sunrise_sunset_times WHERE zip = {0} and month = {1} and day = {1} order by year desc".format(local_zip, month, day)
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

psql_conn = psycopg2.connect(host="10.1.1.1",database="ccpro_noip_org", user="ccpro")

insert_template = "insert into sunrise_sunset_times (zip, year, month, day, sunrise, sunset) values({0}, {1}, {2}, {3}, {4}, {5})"
now = datetime.now()
local_zip = 98027
api_key = 'e172f5276171db59194c0d462804653e'

pp = pprint.PrettyPrinter(indent=4)

srss = find_srss_for_date(now.month, now.day)

psql_conn.close()

start_time = 655
stop_time = 2345

cloud_index = get_cloud_index()
time_offset = 10 * cloud_index

if start_time < srss['sunrise']:
    cmd = 'echo "/usr/bin/perl /usr/home/ccpro/projects/wifi-on-off/tplink_hs110_cmd.pl -command=off -ip=10.1.1.64" | at 0{0}'.format(start_time)
    os.system(cmd)
    cmd = 'echo "/usr/bin/perl /usr/home/ccpro/projects/wifi-on-off/tplink_hs110_cmd.pl -command=on -ip=10.1.1.64" | at 0{0} +{1} minutes'.format(srss['sunrise'], time_offset)
    os.system(cmd)

if stop_time > srss['sunset']:
    cmd = 'echo "/usr/bin/perl /usr/home/ccpro/projects/wifi-on-off/tplink_hs110_cmd.pl -command=on -ip=10.1.1.64" | at {0} -{1} minutes'.format(srss['sunset'], time_offset)
    os.system(cmd)
    cmd = 'echo "/usr/bin/perl /usr/home/ccpro/projects/wifi-on-off/tplink_hs110_cmd.pl -command=off -ip=10.1.1.64" | at {0}'.format(stop_time)
    os.system(cmd)
