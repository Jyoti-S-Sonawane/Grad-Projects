package com.company;

import com.company.models.*;
import java.sql.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;

public class Main {
    //Data models
    Album album;
    Artist artist;
    SongFile songFile;
    Genre genre;

    public static void main(String[] args) {
        Main connection = new Main();
        connection.CreateConnection();
    }

    //CreateConnection(): creating database connection and executing sql statements

    void CreateConnection(){
        try {
            Connection connect = DriverManager.getConnection("jdbc:oracle:thin:@//localhost:1521/ORCLPDB1.localdomain", "hr", "hrpass");
            Statement statement = connect.createStatement();
            System.out.println("---- Connected to the Database ----");

            // Initializing the data models
            initDataModels();


            generateTables(connect);
            System.out.println("************************************************************************************************************************");

            insertIntoTables(connect);
            System.out.println("************************************************************************************************************************");

            // ALBUM table
            String album_query = "select * from ALBUM";
            ResultSet result_album = statement.executeQuery(album_query);
            // iterate through the java album result set
            System.out.println("\n ALBUM TABLE");


            System.out.printf("%20s %20s %20s %20s %20s\n", "AlbumID", "AlbumTitle", "Tracks", "Duration" , "AlbumReleaseYear");
            while (result_album.next())
            {
                album.setAlbumID(result_album.getInt("ALBUM_ID"));
                album.setAlbumTitle(result_album.getString("ALBUM_TITLE"));
                album.setTracks(result_album.getInt("TRACKS"));
                album.setDuration(result_album.getDouble("ALBUM_DURATION"));
                album.setYear(result_album.getInt("ALBUM_YEAR"));
                    // print the results
                System.out.printf("%20s %20s %20s %20s %20s\n", album.getAlbumID().toString(), album.getAlbumTitle().toString(), album.getTracks().toString(), album.getDuration().toString() , album.getYear().toString());
                //System.out.println("AlbumID: "+album.getAlbumID()+"\tAlbumTitle: "+album.getAlbumTitle()+ "\tTracks: "+album.getTracks()+"\tDuration: "+album.getDuration()+"\tAlbumReleaseYear: "+ album.getYear());
            }

            System.out.println("************************************************************************************************************************");

            // ARTIST table
            String artist_query = "select * from ARTIST";
            ResultSet result_artist = statement.executeQuery(artist_query);
            // iterate through the java artist result set
            System.out.println("\n ARTIST TABLE");
            System.out.printf("%20s %20s %20s\n", "ArtistID", "ArtistName", "ArtistDescription");
            while (result_artist.next())
            {
                artist.setArtistID(result_artist.getInt("ARTIST_ID"));
                artist.setArtistName(result_artist.getString("ARTIST_NAME"));
                artist.setArtistDescription(result_artist.getString("ARTIST_DESCRIPTION"));
                // print the results
                System.out.printf("%20s %20s %20s\n", artist.getArtistID(), artist.getArtistName(), artist.getArtistDescription());
            }
            System.out.println("************************************************************************************************************************");

            // GENRE table
            String genre_query = "select * from GENRE";
            ResultSet result_genre = statement.executeQuery(genre_query);
            // iterate through the java genre result set
            System.out.println("\n GENRE TABLE");
            System.out.printf("%20s %20s \n", "GenreID", "GenreName");
            while (result_genre.next())
            {
                genre.setGenreID(result_genre.getInt("GENRE_ID"));
                genre.setGenreName(result_genre.getString("GENRE_NAME"));
                // print the results
                System.out.printf("%20s %20s \n", genre.getGenreID(), genre.getGenreName());
            }
            System.out.println("************************************************************************************************************************");

            // SONG FILE table
            String song_query = "select * from SONGFILE";
            ResultSet result_song = statement.executeQuery(song_query);
            // iterate through the java genre result set
            System.out.println("\n SONGFILE TABLE");
            System.out.printf("%20s %20s %20s %20s %20s \n", "SONG_ID", "SONG_TITLE","SONG_DURATION","SONG_FORMAT","SONG_LOCATION");
            while (result_song.next())
            {
                songFile.setSongFileID(result_song.getInt("SONG_ID"));
                songFile.setSongTitle(result_song.getString("SONG_TITLE"));
                songFile.setDuration(result_song.getDouble("SONG_DURATION"));
                songFile.setFormat(result_song.getString("SONG_FORMAT"));
                songFile.setLocation(result_song.getString("SONG_LOCATION"));

                // print the results
                System.out.printf("%20s %20s %20s %20s %20s \n",
                        songFile.getSongFileID(),
                        songFile.getSongTitle(),
                        songFile.getDuration(),
                        songFile.getFormat(),
                        songFile.getLocation());

            }
            System.out.println("************************************************************************************************************************");

            // Inner Join
            String inner_join_song_title_album_title = "select SONG_TITLE, ALBUM_TITLE from SONGFILE s join ALBUM USING(ALBUM_ID)";
            ResultSet inner_join_song_title_album_title_result = statement.executeQuery(inner_join_song_title_album_title);
            // iterate through the java inner join result set
            System.out.println("\n INNER JOIN");
            System.out.printf("%20s %20s \n", "SONG_TITLE", "ALBUM_TITLE");
            while (inner_join_song_title_album_title_result.next())
            {
                songFile.setSongTitle(inner_join_song_title_album_title_result.getString("SONG_TITLE"));
                album.setAlbumTitle(inner_join_song_title_album_title_result.getString("ALBUM_TITLE"));

                System.out.printf("%20s %20s \n", songFile.getSongTitle(), album.getAlbumTitle());
            }
            System.out.println("************************************************************************************************************************");


            // Multi-way Join
            String multiway_join_genre_artist_song = "select SONG_TITLE, ARTIST_NAME, GENRE_NAME from SONGFILE s join GENRE g USING(GENRE_ID) join ARTIST a USING(ARTIST_ID)";
            ResultSet multiway_join_genre_artist_song_result = statement.executeQuery(multiway_join_genre_artist_song);
            // iterate through the java multi-way join result set
            System.out.println("\n MULTIWAY JOIN");
            System.out.printf("%20s %20s %20s\n", "SONG_TITLE", "ARTIST_NAME","GENRE_NAME");
            while (multiway_join_genre_artist_song_result.next())
            {
                songFile.setSongTitle(multiway_join_genre_artist_song_result.getString("SONG_TITLE"));
                artist.setArtistName(multiway_join_genre_artist_song_result.getString("ARTIST_NAME"));
                genre.setGenreName(multiway_join_genre_artist_song_result.getString("GENRE_NAME"));
                System.out.printf("%20s %20s %20s \n", songFile.getSongTitle(), artist.getArtistName(), genre.getGenreName());
            }
            System.out.println("************************************************************************************************************************");

            // Left Join
            String left_join_artist_song = "select ARTIST_NAME, SONG_TITLE from ARTIST a left join SONGFILE s USING(ARTIST_ID)";
            ResultSet left_join_artist_song_result = statement.executeQuery(left_join_artist_song);
            System.out.println("\n LEFT JOIN");
            System.out.printf("%20s %20s \n", "SONG_TITLE", "ARTIST_NAME");
            while (left_join_artist_song_result.next())
            {
                songFile.setSongTitle(left_join_artist_song_result.getString("SONG_TITLE"));
                artist.setArtistName(left_join_artist_song_result.getString("ARTIST_NAME"));
                System.out.printf("%20s %20s\n", songFile.getSongTitle(), artist.getArtistName());
            }

            System.out.println("************************************************************************************************************************");

            // Right Join
            String right_join_genre_song = "select GENRE_NAME, SONG_TITLE from SONGFILE s right join GENRE g USING(GENRE_ID)";
            ResultSet right_join_genre_song_result = statement.executeQuery(right_join_genre_song);
            System.out.println("\n RIGHT JOIN");
            System.out.printf("%20s %20s \n", "GENRE_NAME", "SONG_TITLE");
            while (right_join_genre_song_result.next())
            {
                genre.setGenreName(right_join_genre_song_result.getString("GENRE_NAME"));
                songFile.setSongTitle(right_join_genre_song_result.getString("SONG_TITLE"));
                System.out.printf("%20s %20s\n", songFile.getSongTitle(), genre.getGenreName());
            }

            System.out.println("************************************************************************************************************************");

            // Full Join
            String full_join_artist_song = "select ARTIST_NAME, SONG_TITLE from SONGFILE s full join ARTIST a USING(ARTIST_ID)";
            ResultSet full_join_artist_song_result = statement.executeQuery(full_join_artist_song);
            // iterate through the java multiway join result set
            System.out.println("\n FULL JOIN");
            System.out.printf("%20s %20s \n", "ARTIST_NAME", "SONG_TITLE");
            while (full_join_artist_song_result.next())
            {
                artist.setArtistName(full_join_artist_song_result.getString("ARTIST_NAME"));
                songFile.setSongTitle(full_join_artist_song_result.getString("SONG_TITLE"));

                System.out.printf("%20s %20s\n", songFile.getSongTitle(), artist.getArtistName());
            }

            System.out.println("************************************************************************************************************************");

            // Self Join
            String self_join_song = "select a.SONG_ID as SONG_ID_A, b.SONG_ID SONG_ID_B, a.ALBUM_ID as ALBUM_ID from SONGFILE a inner join songfile b on a.ALBUM_ID = b.ALBUM_ID where a.SONG_ID < b.SONG_ID";
            ResultSet self_join_song_result = statement.executeQuery(self_join_song);
            // iterate through the java multiway join result set
            System.out.println("\n SELF JOIN");
            System.out.printf("%20s %20s %20s\n", "ALBUM_ID", "SONG_ID_A","SONG_ID_B");
            while (self_join_song_result.next())
            {
                System.out.printf("%20s %20s %20s\n", self_join_song_result.getInt("ALBUM_ID"), self_join_song_result.getInt("SONG_ID_A"), self_join_song_result.getInt("SONG_ID_B"));
            }

            System.out.println("************************************************************************************************************************");

            // GROUP BY
            String groupby_loc_song = "select SONG_LOCATION, COUNT(SONG_ID) from SONGFILE GROUP BY SONG_LOCATION";
            ResultSet groupby_loc_song_result = statement.executeQuery(groupby_loc_song);
            // iterate through the java multiway join result set
            System.out.println("\n GROUP BY");

            System.out.printf("%20s %20s \n", "SONG_LOCATION", "COUNT(SONG_ID)");
            while (groupby_loc_song_result.next())
            {
                System.out.printf("%20s %20s\n", groupby_loc_song_result.getString("SONG_LOCATION"), groupby_loc_song_result.getInt("COUNT(SONG_ID)"));

            }

            System.out.println("************************************************************************************************************************");

            // ORDER BY
            String orderby_album = "select album_title, album_year FROM album ORDER BY album_year";
            ResultSet orderby_album_result = statement.executeQuery(orderby_album);
            // iterate through the java multiway join result set
            System.out.println("\n GROUP BY");
            System.out.printf("%20s %20s \n", "ALBUM_TITLE", "ALBUM_YEAR");
            while (orderby_album_result.next())
            {
                System.out.printf("%20s %20s\n", orderby_album_result.getString("ALBUM_TITLE"), orderby_album_result.getString("ALBUM_YEAR"));
            }

            System.out.println("************************************************************************************************************************");

            // UPDATE
            String update_album = "update album set album_duration = 10 where album_id = 2";
            statement.executeUpdate(update_album);
            ResultSet album_update_result = statement.executeQuery(album_query);
            System.out.println("\n ALBUM TABLE");
            System.out.printf("%20s %20s %20s %20s %20s \n", "ALBUM_ID", "ALBUM_TITLE","TRACKS","ALBUM_DURATION","ALBUM_YEAR");
            while (album_update_result.next())
            {
                album.setAlbumID(album_update_result.getInt("ALBUM_ID"));
                album.setAlbumTitle(album_update_result.getString("ALBUM_TITLE"));
                album.setTracks(album_update_result.getInt("TRACKS"));
                album.setDuration(album_update_result.getDouble("ALBUM_DURATION"));
                album.setYear(album_update_result.getInt("ALBUM_YEAR"));
                // print the results
                System.out.printf("%20s %20s %20s %20s %20s\n", album.getAlbumID(), album.getAlbumTitle(), album.getTracks(), album.getDuration(), album.getYear());
            }

            System.out.println("************************************************************************************************************************");

            // DELETE
            String delete_format = "DELETE FROM songfile WHERE song_format = 'mp3'";
            statement.executeUpdate(delete_format);
            ResultSet result_song_update = statement.executeQuery(song_query);
            // iterate through the java genre result set
            System.out.println("\n SONGFILE TABLE");
            System.out.printf("%20s %20s %20s %20s %20s \n", "SONG_ID", "SONG_TITLE","SONG_DURATION","SONG_FORMAT","SONG_LOCATION");

            while (result_song_update.next())
            {
                songFile.setSongFileID(result_song_update.getInt("SONG_ID"));
                songFile.setSongTitle(result_song_update.getString("SONG_TITLE"));
                songFile.setDuration(result_song_update.getDouble("SONG_DURATION"));
                songFile.setFormat(result_song_update.getString("SONG_FORMAT"));
                songFile.setLocation(result_song_update.getString("SONG_LOCATION"));

                // print the results
                System.out.printf("%20s %20s %20s %20s %20s\n",
                        songFile.getSongFileID(),
                        songFile.getSongTitle(),
                        songFile.getDuration(),
                        songFile.getFormat(),
                        songFile.getLocation());
            }

            System.out.println("************************************************************************************************************************");

            // SUB QUERY
            String subquery_duration = "SELECT * FROM songfile WHERE song_duration > (SELECT AVG(songfile.song_duration) FROM songfile)";
            ResultSet subquery_duration_result = statement.executeQuery(subquery_duration);
            System.out.println("\n SUBQUERY");
            System.out.printf("%20s %20s %20s %20s %20s \n", "SONG_ID", "SONG_TITLE","SONG_DURATION","SONG_FORMAT","SONG_LOCATION");
            while (subquery_duration_result.next())
            {
                songFile.setSongFileID(subquery_duration_result.getInt("SONG_ID"));
                songFile.setSongTitle(subquery_duration_result.getString("SONG_TITLE"));
                songFile.setDuration(subquery_duration_result.getDouble("SONG_DURATION"));
                songFile.setFormat(subquery_duration_result.getString("SONG_FORMAT"));
                songFile.setLocation(subquery_duration_result.getString("SONG_LOCATION"));

                // print the results
                System.out.printf("%20s %20s %20s %20s %20s \n",
                        songFile.getSongFileID(),
                        songFile.getSongTitle(),
                        songFile.getDuration(),
                        songFile.getFormat(),
                        songFile.getLocation());
            }

            System.out.println("************************************************************************************************************************");

            // Exists
            String exists_song = "SELECT s.song_title FROM songfile s WHERE EXISTS (SELECT * FROM artist a WHERE a.artist_id = s.artist_id AND a.artist_name = 'Coldplay')";
            ResultSet exists_song_res = statement.executeQuery(exists_song);
            System.out.println("\n EXISTS");
            System.out.printf("%20s \n", "SONG_TITLE");
            while (exists_song_res.next())
            {
                // print the results
                System.out.printf("%20s \n",exists_song_res.getString("SONG_TITLE"));
            }

            System.out.println("************************************************************************************************************************");

            // Not Exists
            String not_exists_song = "SELECT s.song_title FROM songfile s WHERE NOT EXISTS (SELECT * FROM genre g WHERE g.genre_id = s.genre_id AND g.genre_name = 'reggae')";
            ResultSet not_exists_song_res = statement.executeQuery(not_exists_song);
            System.out.println("\n NOT EXISTS");
            System.out.printf("%20s \n", "SONG_TITLE");
            while (not_exists_song_res.next())
            {
                // print the results
                System.out.printf("%20s \n",not_exists_song_res.getString("SONG_TITLE"));
            }

            System.out.println("************************************************************************************************************************");

            // NATURAL JOIN
            String natural_song_genre = "select * from songfile natural join genre";
            ResultSet natural_song_genre_res = statement.executeQuery(natural_song_genre);
            System.out.println("\n NATURAL JOIN");
            System.out.printf("%20s %20s %20s %20s %20s \n", "SONG_ID", "SONG_TITLE","SONG_DURATION","SONG_FORMAT","SONG_LOCATION");
            while (natural_song_genre_res.next())
            {
                songFile.setSongFileID(natural_song_genre_res.getInt("SONG_ID"));
                songFile.setSongTitle(natural_song_genre_res.getString("SONG_TITLE"));
                songFile.setDuration(natural_song_genre_res.getDouble("SONG_DURATION"));
                songFile.setFormat(natural_song_genre_res.getString("SONG_FORMAT"));
                songFile.setLocation(natural_song_genre_res.getString("SONG_LOCATION"));

                // print the results
                System.out.printf("%20s %20s %20s %20s %20s\n",
                        songFile.getSongFileID(),
                        songFile.getSongTitle(),
                        songFile.getDuration(),
                        songFile.getFormat(),
                        songFile.getLocation());
            }
            System.out.println("************************************************************************************************************************");
            // THETA JOIN
            String theta_song_artist = "select * from songfile join artist on songFile.artist_ID = artist.artist_ID";
            ResultSet theta_song_artist_res = statement.executeQuery(theta_song_artist);
            System.out.println("\n THETA JOIN");
            System.out.printf("%20s %20s %20s %20s %20s  %20s %20s\n", "SONG_ID", "SONG_TITLE","SONG_DURATION","SONG_FORMAT","SONG_LOCATION", "ARTIST_NAME","ARTIST_DESCRIPTION");
            while (theta_song_artist_res.next())
            {
                songFile.setSongFileID(theta_song_artist_res.getInt("SONG_ID"));
                songFile.setSongTitle(theta_song_artist_res.getString("SONG_TITLE"));
                songFile.setDuration(theta_song_artist_res.getDouble("SONG_DURATION"));
                songFile.setFormat(theta_song_artist_res.getString("SONG_FORMAT"));
                songFile.setLocation(theta_song_artist_res.getString("SONG_LOCATION"));
                artist.setArtistName(theta_song_artist_res.getString("ARTIST_NAME"));
                artist.setArtistDescription(theta_song_artist_res.getString("ARTIST_DESCRIPTION"));

                // print the results
                System.out.printf("%20s %20s %20s %20s %20s %20s %20s\n",
                        songFile.getSongFileID(),
                        songFile.getSongTitle(),
                        songFile.getDuration(),
                        songFile.getFormat(),
                        songFile.getLocation(),
                        artist.getArtistName(),
                        artist.getArtistDescription());
            }

            System.out.println("************************************************************************************************************************");
            // Max
            String max_song_dur = "SELECT max(song_duration) AS longest_song_duration FROM songFile";
            ResultSet max_song_dur_res = statement.executeQuery(max_song_dur);
            System.out.println("\n MAX");
            while (max_song_dur_res.next())
            {
                songFile.setDuration(max_song_dur_res.getDouble("longest_song_duration"));

                // print the results
                System.out.printf("%s\n", songFile.getDuration());
            }

            System.out.println("************************************************************************************************************************");
            // Min
            String min_song_dur = "SELECT min(song_duration) AS shortest_song_duration FROM songFile";
            ResultSet min_song_dur_res = statement.executeQuery(min_song_dur);
            System.out.println("\n MIN");
            while (min_song_dur_res.next())
            {
                songFile.setDuration(min_song_dur_res.getDouble("shortest_song_duration"));
                // print the results
                System.out.format("%s \n", songFile.getDuration());
            }

            System.out.println("************************************************************************************************************************");
            // SUM
            String sum_song_dur = "SELECT sum(song_duration) AS total_playlist_time FROM songFile";
            ResultSet sum_song_dur_res = statement.executeQuery(sum_song_dur);
            System.out.println("\n SUM");
            while (sum_song_dur_res.next())
            {
                songFile.setDuration(sum_song_dur_res.getDouble("total_playlist_time"));

                // print the results
                System.out.format("%s \n", songFile.getDuration());
            }

            System.out.println("************************************************************************************************************************");
            statement.close();
        }
        catch (SQLException e){
            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, "Error encountered while connecting to the database.", e);
            e.printStackTrace();
        }
    }

    public void initDataModels(){
         album = new Album();
         artist = new Artist();
         songFile = new SongFile();
         genre = new Genre();
    }

    public static void generateTables(Connection connection) throws SQLException {
        String str = new String();
        StringBuffer sb = new StringBuffer();
        try {
            FileReader fr = new FileReader(new File("/Users/Jyoti/Desktop/sql_files/create_table.sql"));
            BufferedReader br = new BufferedReader(fr);
            while ((str = br.readLine()) != null) {
                sb.append(str);
            }
            br.close();

            String[] instuction = sb.toString().split(";");
            Statement st = connection.createStatement();
            for (int i = 0; i < instuction.length; i++) {

                {
                    st.executeUpdate(instuction[i]);
                    System.out.println("->" + instuction[i]);
                }
            }
        } catch (Exception e) {
            System.out.println(" -----> Error : " + e.toString());
            System.out.println("-------- ");
            System.out.println(" -----> Error : ");
            e.printStackTrace();
            System.out.println("--------------------------------------------------");
            System.out.println(sb.toString());
        }
    }


    public static void insertIntoTables(Connection connection) {
        String str = new String();
        StringBuffer sb = new StringBuffer();
        try
        {
            FileReader fr = new FileReader(new File("/Users/Jyoti/Desktop/sql_files/insert_table.sql"));
            BufferedReader br = new BufferedReader(fr);
            while((str = br.readLine()) != null) {
                sb.append(str);
            }
            br.close();
            String[] instuction = sb.toString().split(";");
            Statement st = connection.createStatement();
            for(int i = 0; i < instuction.length; i++) {
                {
                    st.executeUpdate(instuction[i]);
                    System.out.println("->"+instuction[i]); }
            }
        }
        catch(Exception e) {
            System.out.println(" -----> Error : " + e.toString());
            System.out.println("-------- ");
            System.out.println(" -----> Error : ");
            e.printStackTrace();
            System.out.println("--------------------------------------------------");
            System.out.println(sb.toString());
        }
    }


}
