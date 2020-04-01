#!/usr/local/bin/python3.6

import psycopg2
import re
import json

re_imdb = re.compile(r'.*/title/(tt.*?)/.*?')

with open('movie.data') as json_file:
    data = json.load(json_file)

resave = []

psql_conn = psycopg2.connect(host="10.1.1.1", database="ccpro_noip_org", user="ccpro")

for dd in data:
    for d in dd:

#        print(d)

        imdb = re_imdb.match(d['reference'])
        sql = ''
        param = ''
        if imdb is not None and len(imdb.groups()) > 1:
            sql = "SELECT * FROM movie_review where reference LIKE '%{0}%'"
            param = imdb[0]
        else:
            sql = "SELECT * FROM movie_review WHERE name = '{0}'"
            param = d['name'].replace("'", "''")

        cur = psql_conn.cursor()
        cur.execute(sql.format(param))
        rows = cur.fetchall()
        data = {}

        if 0 == len(rows):
            resave.append(d)
            print(d)
        cur.close()

with open ('resave.data', 'w') as outfile:
    json.dump(resave, outfile)
