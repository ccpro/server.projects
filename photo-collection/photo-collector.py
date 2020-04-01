#!/usr/local/bin/python3.6

import sys, os
import re
from fnmatch import fnmatch
from pathlib import Path
import psycopg2
from datetime import datetime

images = []
tags = ['seattle', 'от папика', 'лыжа', 'картинки', 'старъё', 'мобилография']
image_search = re.compile('<img.*src="(.*)"')

def build_tags_list():
    tags_list = []
    for tag in tags:
        tags_list.append("tags LIKE '%" + tag + "%'")

    return tags_list

def filter_image_list(image_list):
    new_list = []
    
    for i in image_list:
        if 'ccpro.no-ip.org/cgi/lji' in i:
            continue
        if 'ccpro.no-ip.org/photos' in i:
            continue

        parts = i.split('/')
        for index in range(0, 3):
            parts.pop(0)
        name = parts[len(parts) - 1]
        if '"' in name:
            dq_index = name.index('"')
            name = name[0:dq_index]

        name = os.path.splitext(name)[0]

        parts.pop(len(parts) - 1)
        new_list.append(['/'.join(parts), name])

    return new_list

def read_lj_images():

    tags_list = build_tags_list()
    found_images = []

    psql_conn = psycopg2.connect(host="10.1.1.1", database="ccpro_noip_org", user="ccpro")
    cur = psql_conn.cursor()
    
    where_tags = ' OR '.join(tags_list)
    sql = 'select coalesce(subject, \'\'), date(eventtime), tags, event from live_journal where security <> \'private\' AND (' + where_tags + ');'
    
    cur.execute(sql)
    
    records = cur.fetchall() 
    for row in records:
#        print("-----------------\n\n")
#        print("subj : ", row[0])
#        print("time : ", row[1])
#        print("tags : ", row[2])
#        print("text : ", row[3])
#        print("-----------------\n\n")

        images = image_search.findall(row[3])
#        print(images)
        images = filter_image_list(images)

#        print(images)
                
        for img in images:
            found_images.append({'album': row[0], 'date':row[1].strftime('%Y-%m-%d'), 'path':img[0], 'image':img[1]})

    cur.close()
    psql_conn.close()

    return found_images

def to_date(s):
    return datetime.strptime(s[0:8], '%Y%m%d')

def save_in_array(path, list, album):

    extra_path = ""

#    if len(path) > 2 :
#        extra_path = os.path.join(*path[2:])
    extra_path = '/'.join(path)

    for l in list:
        images.append({'date': to_date(path[1]), 'path': extra_path, 'name': l, 'album': album})

def extract_filenames(str):
    regex = r"'(.+?)'+?"
    list = re.findall(regex, str)
    return list

def extract_albumname(str):
    regex = r'\"(.*)\"'
    name = re.findall(regex, str)
    album = ""
    if name is not None and len(name) > 0:
        album = name[0].replace("'", "''")
    return album

def read_file_content(file_name, path):

#    print(file_name)

    album = ""

    found = False
    f = open(file_name, "r")
    lines = f.readlines()
    for line in lines:
        line = line.lstrip(" ")
        if line.startswith("var comment"):
            album = extract_albumname(line)
        if line.startswith("var images = Array") or line.startswith("var images = new Array"):
            list = extract_filenames(line)
            save_in_array(path, list, album)
            found = True
            break
    f.close()

#    if False == found:
#        print(file_name)

def save_data_in_db():
    insert_template = "insert into photo_images (date, path, image, album, type) values('{0}', '{1}', '{2}', '{3}', {4});"

#    psql_conn = psycopg2.connect(host="10.1.1.1", database="ccpro_noip_org", user="ccpro")
#    cur = psql_conn.cursor()
#    cur.execute("delete from photo_images")

    print("begin;")
    
    for i in images:
        print(insert_template.format(i['date'], i['path'], i['name'], i['album'], 0))

    found_images = read_lj_images()
    for i in found_images:
        print(insert_template.format(i['date'], i['path'], i['image'], i['album'].replace("'", "''"), 1))

    print("commit;")
#        try:
#            cur.execute(insert_template.format(i['date'], i['path'], i['name']))
#        except ValueError:
#            print("exception", ValueError)
#            print("i", i)
            
#    psql_conn.commit()
#    cur.close()
    
#    psql_conn.close()

def check_path(path, files) :
    for name in files:
        if name == file_name:
                
            p = Path(path, name)
#            print (p)
            lst = path.lstrip(pt)
#            print (lst)
            psp = lst.split("/")
#            print ("read_file_content", p, psp)
            read_file_content(p, psp)
    
root = ["/home/ccpro/www/photos/"]

if len(sys.argv) > 1 :
   root = []
   for i in range(1, len(sys.argv)) :
       root.append(sys.argv[i])

file_name = "index.shtml"

for pt in root:
    for path, subdirs, files in os.walk(pt):

        if subdirs is not None and len(subdirs) > 0:
             for sd in subdirs:
                for path1, subdirs1, files1 in os.walk(Path(path,sd)):
                    check_path(path1, files1)
        else:
           check_path(path, files)
    
save_data_in_db()
