import os
from collections import namedtuple
import yaml
import json
import pylast


secrets_file = "LastFMPython\\secrets.yaml"
listening_history_file = "LastFMPython\\history.json"

#get secrets from a file *not* on github
def load_secrets():
    
    if os.path.isfile(os.getcwd() + '\\' + secrets_file):
        with open(os.getcwd() + '\\' + secrets_file, "r") as f: 
            return yaml.load(f)
    else:
        print("No secrets file found")
        quit()

#class which makes the storing of my last.fm data a little safer and intuitive
SmallTrack = namedtuple("SmallTrack", "title artist album timestamp_array")

#get array of previous listening history from file
def load_history():
    history = []
    with open(os.getcwd() + "\\" + listening_history_file, "r+") as f:
        history = json.load(f)
    for i, track in enumerate(history):
        history[i] = SmallTrack(title = history[i][0], artist = history[i][1], album = history[i][2], timestamp_array = history[i][3])
    return history

#make history (no pun intendid)
def write_history(history):
    with open(os.getcwd() + "\\" + listening_history_file, "w+") as f:
        json.dump(history, f, separators=(',', ': '), indent = True, sort_keys=True)

secrets = load_secrets()

#fire up API
print("Logging in with: " + secrets["username"])
network = pylast.LastFMNetwork(api_key = secrets["api_key"], api_secret = secrets["api_secret"], username = secrets["username"])

#get the user you want to shuffle
user = network.get_user(secrets["username"])
print("Got user: " + user.get_name())

#get the recent tracks of the user
history = user.get_recent_tracks(limit = 0)

#put them into a friendly array, and dedupe while we're at it
strippedHistory = []

print("Start parsing")

for track in history:
    #wrap track in better API
    strippedTrack = SmallTrack(title = track.track.title, artist = str(track.track.artist), album = str(track.album), timestamp_array = [track.timestamp])
    #check track against all stored tracks
    dupeFound = False
    for checkTrack in strippedHistory:
        if(checkTrack.title == strippedTrack.title):
            #if we found a dupe, add the timestamp to the array
            checkTrack.timestamp_array.append(strippedTrack.timestamp_array[0])
            checkTrack.timestamp_array.sort(reverse = True)
            dupeFound = True
            #print("Dupe Found: " + strippedTrack.title)
            #print(checkTrack.timestamp_array)
            break

    if(dupeFound == False):
        strippedHistory.append(strippedTrack)
    
#clear memory some
#history.clear()

#print("End parsing")
#print("Start JSON")

#write history to file
#write_history(strippedHistory)

#print("End JSON")

#strippedHistory = load_history()

#print(strippedHistory[0].title)
