package com.company.models;

public class Artist {
    private Integer artistID;
    private String artistName;
    private String genreName;
    private String artistDescription;

    public Artist(Integer artistID, String artistName, String artistDescription, String genreName) {
        this.artistID = artistID;
        this.artistName = artistName;
        this.genreName = genreName;
        this.artistDescription = artistDescription;
    }

    public Integer getArtistID() {
        return artistID;
    }

    public void setArtistID(Integer artistID) {
        this.artistID = artistID;
    }

    public String getArtistDescription() {
        return artistDescription;
    }

    public void setArtistDescription(String artistDescription) {
        this.artistDescription = artistDescription;
    }

    public Artist(){
        this.artistID = 0;
        this.artistName = "";
        this.artistDescription = "";
        this.genreName = "";
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
}
