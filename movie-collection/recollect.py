#!/usr/local/bin/python3.6

import psycopg2
from psycopg2 import sql
import re
import json

ref_re = re.compile(r'<b>(.*?)</b>')
clean_re = re.compile('<.*?>')
split_re = re.compile(r'[\r\n]+')
subsplit_re = re.compile(r' - ')

def search_in_db(title):
    found = False
    id = 0

    title = title.replace("'", "''")

    c = psql_conn.cursor()
    c.execute("select id from movie_review where name = '{}'".format(title))
    records = c.fetchall()
    c.close()

    if records is not None and len(records) > 0:
        found = True
        id = int(records[0][0])

    return found, id

#    count = records[0]
#
#    cur.close()
#
#    if count > 0:
#        found = True
#
#    return found

def parse_data(line,htmlid):
    d1 = split_re.split(line)

    data = []

    for d in d1:
        m = ref_re.search(d)
        
        if m is not None:
            title = re.sub(clean_re, '', m[0])
            print(title)
            
            found, id = search_in_db(title)
            
            if not found:
                data.append({'name':title, 'id':htmlid})
            else:
#                print('found')
#                print(id)
        
#        sd = subsplit_re.split(d)
#        if 2 == len(sd):
#            dd = ref_re.match(sd[0])
#            if dd and 2 == len(dd.groups()):
#                data.append({'reference':dd.groups()[0], 'name':dd.groups()[1], 'review':sd[1]})            
#            else:
#                data.append({'reference':'', 'name':sd[1], 'review':sd[1]})            
    return data

ins = """INSERT INTO movie_review (checked, rate, date, reference, name, review) VALUES (1,0,'{}','{}','{}','{}');"""

psql_conn = psycopg2.connect(host="10.1.1.1", database="ccpro_noip_org", user="ccpro")
cur = psql_conn.cursor()

sql = "select eventtime, event, htmlid from live_journal where tags LIKE '%киновости%'"

cur.execute(sql)

records = cur.fetchall()

all_data = []

for row in records:
   data = parse_data(row[1], row[2])

   all_data.append(data)

#   fields = []
#   for d in data:
#       fields = (row[0], d['reference'], d['name'], d['review'])
#       ins_sql = ins.format(row[0], d['reference'], d['name'].replace("'", "''"), d['review'].replace("'", "''"))
#       print(ins_sql)
    
cur.close()
psql_conn.close()


with open ('movie.data', 'w') as outfile:
    json.dump(all_data, outfile)
