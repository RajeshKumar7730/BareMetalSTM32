import json

with open('version.json') as f:
    d = json.load(f)
    print(d)
    print(type(d["major"]))