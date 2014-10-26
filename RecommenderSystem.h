#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_rng.h>


using namespace std;
#ifndef RECOMMENDER_SYSTEM_H
#define RECOMMENDER_SYSTEM_H
class CRSUser;
class CRSMovie
{
private:
	int m_movie_ID; 
	string m_movie_title;
	int m_year_of_release;
	int m_num_ratings;
	vector<double> m_ratings;
	//vector<CRSUser*> m_p_to_raters;
	// work-around:
	vector<int> m_indices_to_raters;
	//vector<double> m_x; // parameters defining the movies characteristics...
	gsl_vector* m_xs; // parameters defining the movie characteristics...
	int m_num_parameters;
public:
	// Setters
	void SetMovieID(int movie_ID){m_movie_ID = movie_ID;};
	void SetMovieName(string movie_title){m_movie_title=movie_title;};
	void SetYearOfRelease(int yor){m_year_of_release=yor;};
	void SetParameters(gsl_vector* params);
	// Getters
	int GetYearOfRelease(){return m_year_of_release;};
	int GetMovieID(){return m_movie_ID;};
	string GetMovieTitle(){return m_movie_title;};
	int GetNumRatings(){return m_num_ratings;};
	gsl_vector** GetPointerToParameterVector(){return &m_xs;};

	//void AddRating(double rating,CRSUser* p_rater);
	void AddRating(double rating,int index_rater);
	void Free();

	vector<double>& GetRefToRatings(){return m_ratings;};
	//vector<CRSUser*>& GetRefToRaters(){return m_p_to_raters;};
	vector<int>& GetRefToRaterIndices(){return m_indices_to_raters;};
	CRSMovie();
	~CRSMovie();

	//void InitMovieParameters(int num_params,double* params);
	void InitMovieParameters(int num_params,gsl_vector* params);
};

class CRSUser
{
private:
	int m_user_ID;
	int m_num_rated_movies; 
	string m_user_name;
	vector<CRSMovie*> m_p_to_movies;
	vector<double> m_ratings; // the actual rating that the user has given...
	//vector<double> m_theta; // describing the users preferences 
	gsl_vector* m_thetas; // describing the users preferences
	int m_num_parameters; // number of parameters (theta) describing the users preferences..
public:
// Setters	
	void SetUserID(int user_ID){m_user_ID = user_ID;};
	void SetUserName(string user_name){m_user_name = user_name;};
	void SetParameters(gsl_vector* params);

// Getters
	int GetNumberOfRatedMovies(){return m_num_rated_movies;};
	gsl_vector** GetPointerToParameterVector(){return &m_thetas;};

	vector<CRSMovie*>& GetRefToRatedMovies(){return m_p_to_movies;};
	vector<double>& GetRefToRatings(){return m_ratings;};
	int GetUserID(){return m_user_ID;};
	string GetUserName(){return m_user_name;};

	void InitUserParameters(int num_parameters,gsl_vector* params);
	void Free();
	void AddRatedMovie(double rating,CRSMovie* p_to_movie);
	

	CRSUser();
	~CRSUser();
};

class CRecommenderSystem
{
private:
	int m_num_users;
	int m_num_movies; // replace "movies" by "books" or whatever...
	vector<CRSUser> m_users;
	vector<CRSMovie> m_movies;
	int m_ID_typ; // supposed to signify at some point which algorithm is used, ignored for now
	int m_num_parameters; // how many parameters ? (
	double m_learning_rate;
	double m_lambda; // regularization parameter...
public:
	void AddUser(CRSUser user);
	void AddMovie(CRSMovie movie);

// Setters
	void SetLearningRate(double learning_rate){m_learning_rate=learning_rate;};
	void SetLambda(double lambda){m_lambda=lambda;};

// Getters
	int GetNumMovies(){return m_num_movies;};
	int GetNumUsers(){return m_num_users;};

	void LoadRatingData(string path,int num_datasets);
	void InitRecommenderSystem(int num_parameters);

	CRecommenderSystem();
	void Free();
	void PrintStatus(); // for test-purposes only...
	void Learn(); // Does one iteration of the learning algorithm....
	double PredictRating(int index_user,int index_movie);
	double ComputeCurrError(); // compute the current error of the recommender system
};

#endif