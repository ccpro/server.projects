
import requests

url = "https://movie-database-imdb-alternative.p.rapidapi.com"

payload = "{}"
headers = {
    'x-rapidapi-host': 'movie-database-imdb-alternative.p.rapidapi.com',
    'x-rapidapi-key': "7b93da3e3fmsh7194e9fc6f6d879p113e84jsn0f8fb5a57240",
}
params = {
    'i': 'tt4154796',
    'r': 'json'
}

response = requests.request("GET", url, data=payload, headers=headers, params=params)

print(response.text)