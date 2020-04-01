import psycopg2
import re

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

insert_template = "insert into photo_images (date, path, image, album) values('{0}', '{1}', '{2}', '{3}');"
images = read_images()

for img in images:
    sql = insert_template.format(img['date'], img['path'], img['image'], img['album'].replace("'", "''"))
    print(sql)
