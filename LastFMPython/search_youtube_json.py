#!/usr/bin/python

from apiclient.discovery import build
from random import randint
from collections import namedtuple
import json
import os
import pafy
import vlc

listening_history_file = "LastFMPython\\history.json"

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

DEVELOPER_KEY = "AIzaSyBVdkLBKAUCckEZ4sxTOQhxdG18euSJLQc"
YOUTUBE_API_SERVICE_NAME = "youtube"
YOUTUBE_API_VERSION = "v3"

def get_track(youtube, title):
    search_response = youtube.search().list(
        q=title,
        part="snippet",
        maxResults=5
    ).execute()
    return search_response['items'][0]['id']['videoId']

# load history from JSON file
history = load_history()

youtube = build(YOUTUBE_API_SERVICE_NAME, YOUTUBE_API_VERSION, developerKey=DEVELOPER_KEY)
pafy.set_api_key(DEVELOPER_KEY)

for i in range(0, 100):
# get a random track title and artist
    rand = randint(0, len(history))
    search = history[rand].title + " " + history[rand].artist

    url = 'https://www.youtube.com/watch?v=' + get_track(youtube, search)
    print(url)
    vid = pafy.new(url)
    vid.getbestaudio().download(quiet = True)


