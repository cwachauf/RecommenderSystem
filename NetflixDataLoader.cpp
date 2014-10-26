#include "NetflixDataLoader.h"


void CMovieRatingSet::CalculateAverageRating()
{
	//this->m_num_ratings;
	double sum=0.0f;
	for(int i=0;i<m_num_ratings;++i)
		sum+=(double)m_movie_rating_entries[i].GetRating();

	sum/=(double)m_num_ratings;
	m_average_rating=sum;
	//m_average_rating=sum/(double)m_num_ratings;

	cout << "number of ratings: " << m_num_ratings << endl;
	cout << "average rating: " << m_average_rating << endl;
}

void CMovieRatingSet::AddMovieRatingEntry(CMovieRatingEntry m_rating_entry)
{
	m_movie_rating_entries.push_back(m_rating_entry);
	m_num_ratings++;
}

void CMovieRatingSet::LoadMovieRatingSet(string filename)
{
	string line;
    ifstream infile(filename) ;
	getline(infile,line);
	string movie_ID_string = line.substr(0,line.find(":"));	
	char* pEnd;
	CMovieRatingEntry temp_entry;
	int temp_movie_ID = strtol(movie_ID_string.c_str(),&pEnd,10);
	m_movie_ID=temp_movie_ID;
	cout << "movie ID: " << temp_movie_ID << endl;
	temp_entry.SetMovieID(temp_movie_ID);
	int index_rating = 0;
	if ( infile ) 
	{
		while ( getline( infile , line ) ) 
		{
			//cout << line << endl;
			stringstream ss(line);
			string token;
			
			int temp_current_rating=0;
			int temp_current_userID=0;

			int position=0;
			while(getline(ss, token, ',')) 
			{
				if(position==0)
				{
					temp_current_userID=strtol(token.c_str(),&pEnd,10);
					temp_entry.SetUserID(temp_current_userID);
				}
				else if(position==1)
				{
					temp_current_rating=strtol(token.c_str(),&pEnd,10);
					temp_entry.SetRating(temp_current_rating);
				}
				else if(position==2)
				{
					temp_entry.SetDateString(token);
				}
				
				position++;
			}
			AddMovieRatingEntry(temp_entry);
	    }
	}
   infile.close( ) ;
}

void CMovieRatingSet::Print()
{
	cout << "Movie ID: " << this->m_movie_ID << endl;
	cout << "number of ratings: " << this->m_num_ratings << endl;
	for(int i=0;i<m_num_ratings;++i)
	{
		cout << "User ID: " << this->m_movie_rating_entries[i].GetUserID() << " " << "Rating: " << this->m_movie_rating_entries[i].GetRating() << endl;
	}
}

void CMovieList::AddMovieToList(CMovieEntry movie_entry)
{
	movie_entries.push_back(movie_entry);
	m_num_movies++;
}

void CMovieList::ReadMovieListFromFile(string filename)
{
	string line;
    ifstream infile(filename) ;
	
	CMovieEntry temp_movie_entry;

	char* pEnd;
	int curr_line=0;
	if ( infile ) 
	{
		while ( getline( infile , line ) ) 
		{
			//cout << line << endl;
			stringstream ss(line);
			string token;

			int temp_year;
			int temp_movie_ID;
			
			int position=0;
			while(getline(ss, token, ',')) 
			{
				if(position==0)
				{
					temp_movie_ID=strtol(token.c_str(),&pEnd,10);
					temp_movie_entry.SetMovieID(temp_movie_ID);
				}
				else if(position==1)
				{	
					temp_year = strtol(token.c_str(),&pEnd,10);
					temp_movie_entry.SetYearOfRelease(temp_year);
				}
				else if(position==2)
				{	
					temp_movie_entry.SetTitle(token);
				}
				position++;
			}
			this->AddMovieToList(temp_movie_entry);
			curr_line++;
		}
	}
	cout << "number of lines read: " << curr_line << endl;
   infile.close( ) ;
}

void CMovieList::Print()
{
	cout << "total number of movies: " << this->m_num_movies << endl;
	for(int i=0;i<m_num_movies;++i)
	{
		if(i%100==0)
		{
			cout << "movie-ID: " << this->movie_entries[i].GetMovieID() << " " << "movie title: " << this->movie_entries[i].GetMovieTitle() << " " << "YOR: " << this->movie_entries[i].GetMovieYOR() << endl;
		}
	}
}

void CMovieRatingData::LoadRatingData(string pathname,int num_datasets)
{
	//CMovieRatingSet
	for(int i=1;i<=num_datasets;++i)
	{
		stringstream ss;
		ss << setw(7) << setfill('0') << i;
		//string s = ss.str();
		string filename=pathname+"mv_"+ss.str()+".txt";
		CMovieRatingSet temp_rating_set;
		temp_rating_set.LoadMovieRatingSet(filename);
		temp_rating_set.CalculateAverageRating();
		this->m_movie_rating_dataset.push_back(temp_rating_set);
		this->m_num_datasets++;
		cout << filename << endl;
	}

}