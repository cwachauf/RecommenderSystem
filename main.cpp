#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "NetflixDataLoader.h"
#include "RecommenderSystem.h"
using namespace std;

CRecommenderSystem recommender_system;

const int NUM_ITERATIONS=100;
void main()
{	string pathname("C:\\Studium\\DataMining\\NETFLIX\\");
	
	recommender_system.LoadRatingData(pathname,7);
	recommender_system.InitRecommenderSystem(5);
	recommender_system.SetLearningRate(0.005f);
	recommender_system.SetLambda(0.1f);
	// Iterate the learning...
	for(int iter=0;iter<NUM_ITERATIONS;iter++)
	{
		cout << "iteration: " << iter << endl;
		recommender_system.ComputeCurrError();
		recommender_system.Learn();
	}
	recommender_system.Free();
	cout << "Eingabe zum Beenden" << endl;
	int d;
	cin >> d;
}