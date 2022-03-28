package com.company.models;

public class Genre {
    private Integer genreID;
    private String genreName;
    private String description;

    public Genre(Integer genreID, String genreName, String description) {
        this.genreID = genreID;
        this.genreName = genreName;
        this.description = description;
    }

    public Genre(){
        this.genreID = 0;
        this.genreName = "";
        this.description = "";
    }

    public Integer getGenreID() {
        return genreID;
    }

    public void setGenreID(Integer genreID) {
        this.genreID = genreID;
    }

    public String getGenreName() {
        return genreName;
    }

    public void setGenreName(String genreName) {
        this.genreName = genreName;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }
}
