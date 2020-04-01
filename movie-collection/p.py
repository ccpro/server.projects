
import re

ref = 'http://www.imdb.com/title/tt0790724/'
re_imdb = re.compile(r'.*/title/(tt.*?)/.*?')

imdb = re_imdb.match(ref)
print(imdb)
