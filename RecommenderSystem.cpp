#include "RecommenderSystem.h"
#include "NetflixDataLoader.h"

CRSMovie::CRSMovie()
{
	m_num_ratings=0;
}

CRSMovie::~CRSMovie()
{
	//if(m_xs!=NULL)
	//	gsl_vector_free(m_xs);
}

void CRSMovie::Free()
{
	gsl_vector_free(m_xs);
}

void CRSMovie::InitMovieParameters(int num_parameters,gsl_vector* params)
{
	m_num_parameters=num_parameters;
	m_xs = gsl_vector_alloc(m_num_parameters);
	gsl_vector_memcpy(m_xs,params);
}

void CRSMovie::SetParameters(gsl_vector* params)
{
	gsl_vector_memcpy(m_xs,params);
}

void CRSMovie::AddRating(double rating, int index_to_user)
{
	m_indices_to_raters.push_back(index_to_user);
	m_ratings.push_back(rating);
	m_num_ratings++;
}

void CRSUser::SetParameters(gsl_vector* params)
{
	gsl_vector_memcpy(m_thetas,params);
}

CRSUser::CRSUser()
{
	m_num_rated_movies=0;
}

void CRSUser::Free()
{
	gsl_vector_free(m_thetas);
}

CRSUser::~CRSUser()
{
//	if(m_thetas!=NULL)
	//	gsl_vector_free(m_thetas);
}

void CRSUser::InitUserParameters(int num_parameters,gsl_vector* params)
{
	m_num_parameters = num_parameters;
	m_thetas = gsl_vector_alloc(m_num_parameters);
	gsl_vector_memcpy(m_thetas,params);
}

void CRSUser::AddRatedMovie(double rating,CRSMovie* p_to_movie)
{
	m_p_to_movies.push_back(p_to_movie);
	m_ratings.push_back(rating);
	m_num_rated_movies++;
}

CRecommenderSystem::CRecommenderSystem()
{
	m_num_movies=0;
	m_num_users=0;
}

void CRecommenderSystem::AddMovie(CRSMovie movie)
{
	m_movies.push_back(movie);
	m_num_movies++;
}

void CRecommenderSystem::LoadRatingData(string path,int num_datasets)
{
	cout << "Initializing Recommender System: " << endl;

	// reading in movie-list (file with movie_ID and movie_titles...
	string filename_movie_list = path+"movie_titles.txt";
	cout << "loading file: " << filename_movie_list << endl;
	CMovieList ml;
	ml.ReadMovieListFromFile(filename_movie_list);

	// create new "movie entries in the CRecommenderSystem classe
	int local_num_movies = ml.GetNumMovies();
	cout << "number of movies in movie list: " << endl;

	vector<CMovieEntry> p_to_movie_entries = ml.GetRefToEntries();

	//int NUM_TRAINING_SETS = 20;
	for(int i=0;i<num_datasets;++i)
	{
		CRSMovie temp_movie;
		temp_movie.SetMovieID(p_to_movie_entries[i].GetMovieID());
		temp_movie.SetMovieName(p_to_movie_entries[i].GetMovieTitle());
		temp_movie.SetYearOfRelease(p_to_movie_entries[i].GetMovieYOR());

	//	cout << p_to_movie_entries[i].GetMovieID() << endl;
		//cout << p_to_movie_entries[i].GetMovieTitle() << endl;
	//	cout << p_to_movie_entries[i].GetMovieYOR() << endl;
		AddMovie(temp_movie);
	}



	// start reading the movie ratings...
	// CAREFUL, WORK IN PROGRESS HERE !!!
	// !!!!!!!!!
	// !!!!!!!!!

	 // start with 50 (out of a total of > 17000) training sets

	for(int index_movie=0;index_movie<num_datasets;++index_movie)
	{
		stringstream ss;
		ss << setw(7) << setfill('0') << index_movie+1;
		string filename_movie_rating_set = path+"TrainingSet\\mv_"+ss.str()+".txt";
		CMovieRatingSet rating_set;
		cout << "loading rating set: " << filename_movie_rating_set << endl;
		rating_set.LoadMovieRatingSet(filename_movie_rating_set);
		rating_set.CalculateAverageRating();

		// compare the ID of the currently read movie (from rating set) to the
		// ID of the movie with index i in the recommender system
		int ID_from_index = this->m_movies[index_movie].GetMovieID();
		int ID_from_curr_dataset = rating_set.GetMovieID();
		if(ID_from_index!=ID_from_curr_dataset)
		{
			cout << "SHIT, indices don't match" << endl;
		}
		else
		{
			cout << "Indices do match" << endl;
			// add all the ratings to the movie...
			int num_ratings = rating_set.GetNumRatings();
			// ratings
			vector<CMovieRatingEntry> p_to_rating_entries = rating_set.GetRefToRatingEntries();

			double curr_average = rating_set.GetAverageRating();

			for(int index_rating=0;index_rating<num_ratings;++index_rating)
			{
				int curr_user_ID = p_to_rating_entries[index_rating].GetUserID();

				int index_user=-1;
				//int curr_size_user_ids = user_ids_vector.size();
				int curr_num_user = this->m_num_users; // how many users do we already have ???

				for(int j=0;j<curr_num_user;j++)
				{
					if(curr_user_ID==m_users[j].GetUserID())
					{
						index_user=j;
						break;
					}
				}

				if(index_user==-1) // add new user and add user id to the vector of user ids...
				{
					CRSUser new_user;
					new_user.SetUserID(curr_user_ID);
					this->AddUser(new_user);
					index_user = m_num_users-1; // ??CAREFUL HERE...
					//index_user = user_ids_vector.size();
				}

				double curr_rating = p_to_rating_entries[index_rating].GetRating() - curr_average;
				m_movies[index_movie].AddRating(curr_rating,index_user);
				m_users[index_user].AddRatedMovie(curr_rating,&m_movies[index_movie]);
			}	
		}


	}
}

void CRecommenderSystem::AddUser(CRSUser new_user)
{
	m_users.push_back(new_user);
	m_num_users++;
}

void CRecommenderSystem::PrintStatus()
{
	cout << "The recommender system includes data of: " << this->m_num_movies << " movies" << endl;
	cout << "from: " << this->m_num_users << " users" << endl;

	// print out theta's of first users..
	for(int i=0;i<10;++i)
	{
		cout << "user number: " << i << endl;
		gsl_vector** p_curr_vector = m_users[i].GetPointerToParameterVector();
		for(int j=0;j<this->m_num_parameters;j++)
		{
			double cv = gsl_vector_get(*p_curr_vector,j);
			cout << cv << "\t";
		}
		cout << endl << endl;

	}
	// list of movies: 
	/*for(int i=0;i<m_num_movies;++i)
	{
		cout << "movie ID:  " << this->m_movies[i].GetMovieID() << endl;
		cout << "movie title: " << this->m_movies[i].GetMovieTitle() << endl;
		cout << "number of ratings for this movie: " << this->m_movies[i].GetNumRatings() << endl;
		
	}*/

}

void CRecommenderSystem::InitRecommenderSystem(int num_parameters)
{
	const gsl_rng_type *T;
	gsl_rng* r;
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc(T);

	m_num_parameters=num_parameters;
	cout << "number of users: " << m_num_users << endl;
	cout << "number of movies: " << m_num_movies << endl;

	// initialize "thetas" for the users
	gsl_vector* theta_vector = gsl_vector_alloc(num_parameters);

	double curr_random_value=0.0f;
	double scale=1.0f;

	cout << "Initializing Thetas" << endl;
	for(int index_user=0;index_user<m_num_users;index_user++)
	{
		for(int index_dim=0;index_dim<num_parameters;index_dim++)
		{
			curr_random_value=gsl_rng_uniform(r);
			curr_random_value-=0.5f; // to have it from -0.5f to 0.5f...
			curr_random_value*=scale;
			gsl_vector_set(theta_vector,index_dim,curr_random_value);
		}
		m_users[index_user].InitUserParameters(num_parameters,theta_vector);
	}

	cout << "Initializing X's" << endl;
	gsl_vector* x_vector = gsl_vector_alloc(num_parameters);

	for(int index_movie=0;index_movie<m_num_movies;index_movie++)
	{
		for(int index_dim=0;index_dim<num_parameters;index_dim++)
		{
			curr_random_value=gsl_rng_uniform(r);
			curr_random_value-=0.5f; // to have it from -0.5f to 0.5f...
			curr_random_value*=scale;
			gsl_vector_set(x_vector,index_dim,curr_random_value);
		}
		m_movies[index_movie].InitMovieParameters(num_parameters,x_vector);
	}

	gsl_vector_free(theta_vector);
	gsl_vector_free(x_vector);
}

void CRecommenderSystem::Free()
{
	for(int i=0;i<m_num_users;++i)
		m_users[i].Free();
	for(int i=0;i<m_num_movies;++i)
		m_movies[i].Free();
}

void CRecommenderSystem::Learn()
{
	gsl_vector* dX =gsl_vector_calloc(m_num_parameters);
	gsl_vector* newX = gsl_vector_calloc(m_num_parameters);
	gsl_vector* dTheta=gsl_vector_calloc(m_num_parameters);
	gsl_vector* newTheta = gsl_vector_calloc(m_num_parameters);

	int j=0;
	double term1;
	double desired_rating;
	double sum=0.0f;
	for(int i=0;i<m_num_movies;++i)
	{
		cout << "learning parameters for movie: " << i << endl;
		vector<int> indices_raters = m_movies[i].GetRefToRaterIndices();
		vector<double> ratings = m_movies[i].GetRefToRatings();
		int num_raters = m_movies[i].GetNumRatings();
		gsl_vector** x_i = m_movies[i].GetPointerToParameterVector();

		// loop over all parameters...
		for(int k=0;k<this->m_num_parameters;++k)
		{
			sum=0.0f;
			// sum up over all users that have rated this particular movie...
			for(int z=0;z<num_raters;z++)
			{
				j=indices_raters[z];
				desired_rating = ratings[z];

				gsl_vector** theta_j = m_users[j].GetPointerToParameterVector();
				gsl_blas_ddot(*theta_j,*x_i,&term1);
				term1-=desired_rating;
				term1*=gsl_vector_get(*theta_j,k);
				//term1+=(m_lambda*gsl_vector_get(*x_i,k));
				sum+=term1;
			}
			sum+=(m_lambda*gsl_vector_get(*x_i,k));
			gsl_vector_set(dX,k,sum);
		}

		gsl_vector_memcpy(newX,*x_i);
		gsl_vector_scale(dX,m_learning_rate);
		gsl_vector_sub(newX,dX);

		m_movies[i].SetParameters(newX);
		// now update the current x_i,....

	}

	// now learn the new parameters for the users (thetas)

	for(int j=0;j<m_num_users;++j)
	{
		if(j%1000==0)
			cout << "learning parameters for user: " << j << endl;
		vector<CRSMovie*> p_to_movies = m_users[j].GetRefToRatedMovies();
		vector<double> ratings = m_users[j].GetRefToRatings();
		int num_rated_movies = m_users[j].GetNumberOfRatedMovies();
		gsl_vector** theta_j = m_users[j].GetPointerToParameterVector();

		for(int k=0;k<m_num_parameters;++k)
		{
			sum=0.0f;
			for(int i=0;i<num_rated_movies;++i)
			{
				//
				desired_rating = ratings[i];
				gsl_vector** x_i = p_to_movies[i]->GetPointerToParameterVector();
				gsl_blas_ddot(*theta_j,*x_i,&term1);
				term1-=desired_rating;
				term1*=gsl_vector_get(*x_i,k);
				sum+=term1;
			}
			sum+=(m_lambda*gsl_vector_get(*theta_j,k));
			gsl_vector_set(dTheta,k,sum);
		}
		gsl_vector_memcpy(newTheta,*theta_j);
		gsl_vector_scale(dTheta,m_learning_rate);
		gsl_vector_sub(newTheta,dTheta);
		m_users[j].SetParameters(newTheta);
	}
	gsl_vector_free(dX);
	gsl_vector_free(newX);
	gsl_vector_free(dTheta);
	gsl_vector_free(newTheta);
}

double CRecommenderSystem::PredictRating(int index_user,int index_movie)
{
	gsl_vector** thetas_user = m_users[index_user].GetPointerToParameterVector();
	gsl_vector** xs_movie = m_movies[index_movie].GetPointerToParameterVector();
	
	// print out the stuff for test purposes...
	cout << "parameter vector of user: " << endl;
	for(int i=0;i<this->m_num_parameters;++i)
	{
		double cv = gsl_vector_get(*thetas_user,i);
		cout << cv << "\t";
	}
	cout << endl;
	cout << "parameter vector of movie: " << endl;
	for(int i=0;i<this->m_num_parameters;++i)
	{
		double cv = gsl_vector_get(*xs_movie,i);
		cout << cv << "\t";
	}
	cout << endl;
	double result;
	gsl_blas_ddot(*thetas_user,*xs_movie,&result);
	cout << "result: " << result << endl;
	return result;
}

double CRecommenderSystem::ComputeCurrError()
{
	double total_error = 0.0f;
	double total_error_with_reg_terms=0.0f;
	int j=0;
	double computed_rating = 0.0f;
	double actual_rating = 0.0f;

	for(int index_movie=0;index_movie<m_num_movies;index_movie++)
	{
		int num_ratings=m_movies[index_movie].GetNumRatings();
		vector<double> ratings = m_movies[index_movie].GetRefToRatings(); // the ideal ratings
		vector<int> rater_indices = m_movies[index_movie].GetRefToRaterIndices();
		gsl_vector** x_i = m_movies[index_movie].GetPointerToParameterVector();
		for(int index_user=0;index_user<num_ratings;index_user++)
		{
			j = rater_indices[index_user];
			gsl_vector** theta_j = m_users[j].GetPointerToParameterVector();
			gsl_blas_ddot(*theta_j,*x_i,&computed_rating);
			actual_rating = ratings[index_user];
			total_error+=(computed_rating-actual_rating)*(computed_rating-actual_rating);
		}
	}

	total_error_with_reg_terms = total_error;
	// add error of regularization terms...
	for(int index_movie=0;index_movie<m_num_movies;index_movie++)
	{
		gsl_vector** x_i = m_movies[index_movie].GetPointerToParameterVector();
		for(int index_dim=0;index_dim<m_num_parameters;index_dim++)
		{
			double curr_value = gsl_vector_get(*x_i,index_dim);
			total_error_with_reg_terms+=(m_lambda/2.0f)*curr_value*curr_value;
		}
	}
	// regularization term from user parameters...
	for(int index_user=0;index_user<m_num_users;index_user++)
	{
		gsl_vector** theta_j = m_users[index_user].GetPointerToParameterVector();
		for(int index_dim=0;index_dim<m_num_parameters;index_dim++)
		{
			double curr_value = gsl_vector_get(*theta_j,index_dim);
			total_error_with_reg_terms+=(m_lambda/2.0f)*curr_value*curr_value;
		}
	}
	cout << "total error: " << total_error << endl;
	cout << "total error with regularization terms: " << total_error_with_reg_terms << endl;
	return total_error_with_reg_terms;
}

