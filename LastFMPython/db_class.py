import os
import sys
from sqlalchemy import Column, ForeignKey, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import relationship, backref
from sqlalchemy import create_engine
 
#Generate classes to be used in database

Base = declarative_base()

#I think it's going to be set up like so:
#Track super class
#title, album, artist and timestamp (sub)classes
#every (sub)class will have a relation to the Track class
#such that I can get a title object and access the artist by title.track.artist
#or something like that

#Track super class, contains references to other tables containing track metadata
class mTrack(Base):
    #define table values and initializers
    __tablename__ = 'tracks'
    id = Column(Integer, primary_key = True)
    #define all relationships to subclasses, all relationships are bidirectional
    #Title relationship
    #There will normally be only one title per track, so the relationship is one to one
    title_id = Column(Integer, ForeignKey('titles.id'))
    title = relationship("Title", back_populates = "track")
    #Album relationship
    #There will be multiple tracks to an album, so the relationship is many to one
    album_id = Column(Integer, ForeignKey('albums.id'))
    album = relationship("Album", back_populates = "track")
    #Artist relationship
    #Same as album, many to one
    artist_id = Column(Integer, ForeignKey('artists.id'))
    artist = relationship("Artist", back_populates = "track")
    #Timestamp relationship
    #There can be many timestamps to one track (multiple plays), so the relationship is one to many
    timestamp = relationship("Timestamp", back_populates = "track")
    #constructor for ease of use
    #def __init__(self, track_title, track_album, track_artist, track_timestamp):
     #   self.title = track_title
    #    self.album = track_album
     #   self.artist = track_artist
    #    self.timestamp = [track_timestamp]

#(Sub)Class definitions
#Basically just containers for the info being stored

class Title(Base):
    #define table values and initializers
    __tablename__ = 'titles'
    id = Column(Integer, primary_key = True)
    #define container for info
    title = Column(String(length=80))
    #define relationship for back population
    track = relationship("mTrack", back_populates = "title", uselist = False)

class Album(Base):
    #define table values and initializers
    __tablename__ = 'albums'
    id = Column(Integer, primary_key = True)
    #define container for info
    album = Column(String(length = 40))
    #define relationship for back population
    track = relationship("mTrack", back_populates = "album")

class Artist(Base):
    #define table values and initializers
    __tablename__ = 'artists'
    id = Column(Integer, primary_key = True)
    #define container for info
    artist = Column(String(length = 40))
    #define relationship for back population
    track = relationship("mTrack", back_populates = "artist")

#this one is a little different, b/c we need to fix the one to many relationship
class Timestamp(Base):
    #define table values and initializers
    __tablename__ = 'timestamps'
    id = Column(Integer, primary_key = True)
    #define container for info
    timestamp = Column(Integer)
    #fdefine relationship for back population
    track_id = Column(Integer, ForeignKey('tracks.id'))
    track = relationship("mTrack", back_populates = "timestamp")
 
