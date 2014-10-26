#ifndef NETFLIX_DATA_LOADER_H
#define NETFLIX_DATA_LOADER_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdlib.h>


using namespace std;

class CMovieEntry
{
private:
	int m_movie_ID;
	int m_year_of_release;
	string m_title;
public:

//  Setters
	void SetMovieID(int movieID){m_movie_ID = movieID;};
	void SetYearOfRelease(int yor){m_year_of_release = yor;};
	void SetTitle(string title){m_title=title;};
// Getters
	int GetMovieID(){return m_movie_ID;};
	int GetMovieYOR(){return m_year_of_release;};
	string GetMovieTitle(){return m_title;};
};

class CMovieList
{
private:
	int m_num_movies;
	vector<CMovieEntry> movie_entries;
public:
	// Setters
	void SetNumMovies(int num_movies){m_num_movies=num_movies;};
	// Getters
	int GetNumMovies(){return m_num_movies;};
	vector<CMovieEntry>& GetRefToEntries(){return movie_entries;};
	void AddMovieToList(CMovieEntry movie_entry);
	void ReadMovieListFromFile(string filename);
	void Print();
};

class CMovieRatingEntry
{
private:
	int m_movie_ID;
	int m_user_ID;
	int m_rating;	
	string m_date_string;
public:

// Setters
	void SetMovieID(int movie_ID){m_movie_ID = movie_ID;};
	void SetUserID(int user_ID){m_user_ID = user_ID;};
	void SetDateString(string date_string){m_date_string = date_string;};
	void SetRating(int rating){m_rating = rating;};

// Getters
	int GetMovieID(){return m_movie_ID;};
	int GetUserID(){return m_user_ID;};
	int GetRating(){return m_rating;};
	string GetDateString(){return m_date_string;};
	CMovieRatingEntry(){m_rating=0;};
};

class CMovieRatingSet
{
private:
	int m_movie_ID;
	int m_num_ratings;
	vector<CMovieRatingEntry> m_movie_rating_entries;
	double m_average_rating;
public:

// Setters
	void SetMovieID(int movieID){m_movie_ID = movieID;};
	//void SetNumRatings(int num_ratings){m_num_ratings=num_ratings;};
	// Getters
	int GetMovieID(){return m_movie_ID;};
	int GetNumRatings(){return m_num_ratings;};
	double GetAverageRating(){return m_average_rating;};
	vector<CMovieRatingEntry>& GetRefToRatingEntries(){return m_movie_rating_entries;};

	void AddMovieRatingEntry(CMovieRatingEntry m_rating_entry);
	void LoadMovieRatingSet(string filename);
	void CalculateAverageRating();
	CMovieRatingSet(){m_num_ratings=0;};
	void Print();
};

class CMovieRatingData			// representing the entire dataset
{
private:
	int m_num_datasets;
	vector<CMovieRatingSet> m_movie_rating_dataset;	
public:
	
// Setters:
	void SetNumDatasets(int num_datasets){m_num_datasets = num_datasets;};

// Getters:
	int GetNumDatasets(){return m_num_datasets;};

	void LoadRatingData(string pathname,int num_datasets);
	void Print();
};




#endif
