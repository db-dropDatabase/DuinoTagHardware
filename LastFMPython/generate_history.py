import os
import yaml
import pylast
import pickle

secrets_file = "LastFMPython\\secrets.yaml"
listening_history_file = "LastFMPython\\history.bin"

#get secrets from a file *not* on github
def load_secrets():
    
    if os.path.isfile(os.getcwd() + '\\' + secrets_file):
        with open(os.getcwd() + '\\' + secrets_file, "r") as f: 
            return yaml.load(f)
    else:
        print("No secrets file found")
        quit()

#get array of previous listening history from file
def load_history():
    with open(os.getcwd() + "\\" + listening_history_file, "r+b") as f:
        return pickle.load(f)

#make history (no pun intendid)
def write_history(history):
    with open(os.getcwd() + "\\" + listening_history_file, "w+b") as f:
        pickle.dump(history, f)

#class which makes the storing of my last.fm data a little safer and intuitive
class SmallTrack(object):
    def __init__(self, track_name, track_artist, track_album, track_timestamp):
        self.name = track_name
        self.artist = track_artist
        self.album = track_album
        self.timestamp = [track_timestamp]

    def get_name(self):
        return self.name
    
    def get_artist(self):
        return self.artist

    def get_album(self):
        return self.album
    
    def get_timestamp_array(self):
        return self.timestamp
    
    def get_timestamp_recent(self):
        return self.timestamp[0]

    def add_timestamp(self, addTimestamp):
        self.timestamp.append(addTimestamp)
        self.timestamp.sort(reverse=True)


secrets = load_secrets()

#fire up API
print("Logging in with: " + secrets["username"])
network = pylast.LastFMNetwork(api_key = secrets["api_key"], api_secret = secrets["api_secret"],
                                username = secrets["username"])

#get the user you want to shuffle
user = network.get_user(secrets["username"])
print("Got user: " + user.get_name())

#get the recent tracks of the user
history = user.get_recent_tracks(limit = 0)

#put them into a friendly array, and dedupe while we're at it
strippedHistory = []

for track in history:
    #wrap track in better API
    strippedTrack = SmallTrack(track.track.title, track.track.artist, track.album, track.timestamp)
    #check track against all stored tracks
    dupeFound = False
    for checkTrack in strippedHistory:
        if(checkTrack.get_name() == strippedTrack.get_name()):
            #if we found a dupe, add the timestamp to the array
            checkTrack.add_timestamp(strippedTrack.get_timestamp_recent())
            dupeFound = True
            break
    
    if(dupeFound == False):
        strippedHistory.append(strippedTrack)
    
#clear memory some
history.clear()

#write history to file
write_history(strippedHistory)

strippedHistory = load_history()

print(strippedHistory[0].get_name())
