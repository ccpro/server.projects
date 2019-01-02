import urllib.request
import json
import pprint

api_key = 'e172f5276171db59194c0d462804653e'
#url = 'http://api.openweathermap.org/data/2.5/weather?lat=47.49638&lon=-121.99090&APPID=' + api_key
url = 'https://api.openweathermap.org/data/2.5/weather?zip=98027,us&APPID=' + api_key

print(url)

pp = pprint.PrettyPrinter(indent=4)

#content = urllib.request.urlopen(url).read()
content = b"""{"coord":{"lon":-122.09,"lat":37.39},
"sys":{"type":3,"id":168940,"message":0.0297,"country":"US","sunrise":1427723751,"sunset":1427768967},
"weather":[{"id":800,"main":"Clear","description":"Sky is Clear","icon":"01n"}],
"base":"stations",
"main":{"temp":285.68,"humidity":74,"pressure":1016.8,"temp_min":284.82,"temp_max":286.48},
"wind":{"speed":0.96,"deg":285.001},
"clouds":{"all":42},
"dt":1427700245,
"id":0,
"name":"Mountain View",
"cod":200}"""

#print(content)

data = json.loads(content)

index = int((data['clouds']['all'] + 1) / 10 / 3 + 1)
#print(index)

for i in range(1, 99):
    index = int((i + 1) / 40 + 1)
    print("{0} :: {1}".format(i, index))