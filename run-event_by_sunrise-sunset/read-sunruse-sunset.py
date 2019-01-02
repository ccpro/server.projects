import urllib.request
import json
import pprint
import calendar
from datetime import datetime, date, time, timedelta
import time
import os

pp = pprint.PrettyPrinter(indent=4)
today = datetime.today().strftime('%Y-%m-%d')

def utc_to_local(utc_dt):
    # get integer timestamp to avoid precision lost
    timestamp = calendar.timegm(utc_dt.timetuple())
    local_dt = datetime.fromtimestamp(timestamp)
    assert utc_dt.resolution >= timedelta(microseconds=1)
    return local_dt.replace(microsecond=utc_dt.microsecond)

#url = 'https://api.sunrise-sunset.org/json?lat=47.49638&lng=-121.99090&formatted=0&date=' + today

#content = urllib.request.urlopen(url).read()
content = b'{"results":{"sunrise":"2019-01-02T15:55:32+00:00","sunset":"2019-01-03T00:28:59+00:00","solar_noon":"2019-01-02T20:12:15+00:00","day_length":30807,"civil_twilight_begin":"2019-01-02T15:20:04+00:00","civil_twilight_end":"2019-01-3T01:04:26+00:00","nautical_twilight_begin":"2019-01-02T14:41:07+00:00","nautical_twilight_end":"2019-01-03T01:43:24+00:00","astronomical_twilight_begin":"2019-01-02T14:03:49+00:00","astronomical_twilight_end":"2019-01-03T02:20:42+00:00"},"status":"OK"}'

data = json.loads(content)

start_time = datetime.strptime(today +' 6:55', '%Y-%m-%d %H:%M')
stop_time = datetime.strptime(today +' 23:45', '%Y-%m-%d %H:%M')

if data['status'] == 'OK':
    sunrise = datetime.strptime(data['results']['sunrise'], '%Y-%m-%dT%H:%M:%S+00:00')
    sunrise = utc_to_local(sunrise)

    sunset = datetime.strptime(data['results']['sunset'], '%Y-%m-%dT%H:%M:%S+00:00')
    sunset = utc_to_local(sunset)

    if start_time < sunrise:
        sunrise = sunrise + timedelta(minutes = 10)

        print('------------------- sunrise -------------------')
        time_on = start_time.strftime('%H%M')
        time_off = sunrise.strftime('%H%M')
        print('going to light up @ ' + time_on)
        print('going to light down @ ' + time_off)

#       os.system('at {0} runlight'.format(time_on))
#       os.system('at {0} stoplight'.format(time_off))
        print('at {0} runlight'.format(time_on))
        print('at {0} stoplight'.format(time_off))

    if stop_time > sunset:
        sunset = sunset + timedelta(minutes = -10)

        print('------------------- sunset -------------------')
        time_on = sunset.strftime('%H%M')
        time_off = stop_time.strftime('%H%M')
        print('going to light up @ ' + sunset.strftime('%H:%M'))
        print('going to light down @ ' + stop_time.strftime('%H:%M'))

        print('at {0} runlight'.format(time_on))
        print('at {0} stoplight'.format(time_off))
