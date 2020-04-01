#!/usr/local/bin/python3.6

import psycopg2

psql_conn = psycopg2.connect(host="10.1.1.1", database="ccpro_noip_org", user="ccpro")

cur = psql_conn.cursor()
cur.execute("SELECT reference FROM movie_review")
rows = cur.fetchall()
cur.close();
data = []

if rows is not None and len(rows) > 0:
    for row in rows:
        data.append({'ref': row[0]})

cur = psql_conn.cursor()
for d in data:
    cur.execute("select date(eventtime) from live_journal where event like '%" + d['ref']+ "%'")
    rows = cur.fetchall()

    if rows is not None and len(rows) > 0:
        d['date'] = str(rows[0][0])

cur.close();

for d in data:
    if 'date' in d:
        cur = psql_conn.cursor()

        sql = "update movie_review set date = '"+ d['date'] +"' where reference = '"+ d['ref']+"'"
        print(sql)
    
        cur.execute(sql)
        psql_conn.commit()
        cur.close()

#print(data)