package com.company.models;

public class Album {
    private Integer albumID;
    private String albumTitle;
    private String artistName;
    private String genreName;
    private Integer tracks;
    private Double duration;
    private Integer year;

    public Album(Integer albumID, String albumTitle, String artistName, String genreName, Integer tracks, Double duration, Integer year) {
        this.albumID = albumID;
        this.albumTitle = albumTitle;
        this.artistName = artistName;
        this.genreName = genreName;
        this.tracks = tracks;
        this.duration = duration;
        this.year = year;
    }

    public Album() {
        this.albumID = 0;
        this.albumTitle = "";
        this.artistName = "";
        this.genreName = "";
        this.tracks = 0;
        this.duration = 0.0;
        this.year = 0;
    }

    public Integer getAlbumID() {
        return albumID;
    }

    public void setAlbumID(Integer albumID) {
        this.albumID = albumID;
    }

    public String getAlbumTitle() {
        return albumTitle;
    }

    public void setAlbumTitle(String albumTitle) {
        this.albumTitle = albumTitle;
    }

    public String getArtistName() {
        return artistName;
    }

    public void setArtistName(String artistName) {
        this.artistName = artistName;
    }

    public String getGenreName() {
        return genreName;
    }

    public void setGenreName(String genreName) {
        this.genreName = genreName;
    }

    public Integer getTracks() {
        return tracks;
    }

    public void setTracks(Integer tracks) {
        this.tracks = tracks;
    }

    public Double getDuration() {
        return duration;
    }

    public void setDuration(Double duration) {
        this.duration = duration;
    }

    public Integer getYear() {
        return year;
    }

    public void setYear(Integer year) {
        this.year = year;
    }
}
