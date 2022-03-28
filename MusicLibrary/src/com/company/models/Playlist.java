package com.company.models;

public class Playlist {
    private String playlistName;
    private Integer tracks;
    private Double duration;

    public Playlist(String playlistName, Integer tracks, Double duration) {
        this.playlistName = playlistName;
        this.tracks = tracks;
        this.duration = duration;
    }

    public String getPlaylistName() {
        return playlistName;
    }

    public void setPlaylistName(String playlistName) {
        this.playlistName = playlistName;
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
}
