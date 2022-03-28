package com.company.models;

public class SongFile {

    private Integer songFileID;
    private String format;
    private String songTitle;
    private Integer artistID;
    private Integer albumID;
    private Integer trackNo;
    private Double duration;
    private String location;
    private Integer genreID;
    private String playlistName;

    public SongFile(Integer songFileID,
                    String songTitle,
                    Integer artistID,
                    Integer albumID,
                    Integer trackNo,
                    Double duration,
                    String format,
                    String location,
                    Integer genreID,
                    String playlistName){
        this.songFileID = songFileID;
        this.songTitle = songTitle;
        this.artistID = artistID;
        this.albumID = albumID;
        this.trackNo = trackNo;
        this.duration = duration;
        this.format = format;
        this.location = location;
        this.genreID = genreID;
        this.playlistName = playlistName;
    }

    public SongFile(){
        this.songFileID = 0;
        this.songTitle = "";
        this.artistID = 0;
        this.albumID = 0;
        this.trackNo = 0;
        this.duration = 0.0;
        this.format = "";
        this.location = "";
        this.genreID = 0;
        this.playlistName = "";
    }

    public String getFormat() {
        return format;
    }

    public void setFormat(String format) {
        this.format = format;
    }

    public String getSongTitle() {
        return songTitle;
    }

    public void setSongTitle(String songTitle) {
        this.songTitle = songTitle;
    }

    public Integer getTrackNo() {
        return trackNo;
    }

    public void setTrackNo(Integer trackNo) {
        this.trackNo = trackNo;
    }

    public Double getDuration() {
        return duration;
    }

    public Integer getSongFileID() {
        return songFileID;
    }

    public void setSongFileID(Integer songFileID) {
        this.songFileID = songFileID;
    }

    public Integer getArtistID() {
        return artistID;
    }

    public void setArtistID(Integer artistID) {
        this.artistID = artistID;
    }

    public Integer getAlbumID() {
        return albumID;
    }

    public void setAlbumID(Integer albumID) {
        this.albumID = albumID;
    }

    public Integer getGenreID() {
        return genreID;
    }

    public void setGenreID(Integer genreID) {
        this.genreID = genreID;
    }

    public void setDuration(Double duration) {
        this.duration = duration;
    }

    public String getLocation() {
        return location;
    }

    public void setLocation(String location) {
        this.location = location;
    }

    public String getPlaylistName() {
        return playlistName;
    }

    public void setPlaylistName(String playlistName) {
        this.playlistName = playlistName;
    }
}
