// Filename: app.cpp
// Project: Major Assignment #3a
// By: Icaro Ryan Oliveira Souza
// Date: March 08, 2021
// Description: This program finds the best/fastest flight
//				route from one city to another.


#include <iostream>
#include <sstream>
#include "..\Headers\include\AmazingRace.h"


int isValidRange(int value, int minRange, int maxRange);


int main()
{
	// pathfinding/graph algorithms: Dijkstra's algorithm;
	string menu = "1) Toronto\n"
		"2) Atlanta\n"
		"3) Austin\n"
		"4) Santa Fe\n"
		"5) Denver\n"
		"6) Chicago\n"
		"7) Buffalo\n"
		"\nEnter a starting city number, ending city number, and the 24-hour starting time(hhmm), separated by a space\n";

	int startingCity = 0;
	int endingCity = 0;
	char startingTimeStr[kArraySizeBuffer] = { 0 };

	cout << "--- WELCOME TO THE AMAZING RACE VARIANT ---\n" << endl;

	// Loop until the user enter an invalid input
	while (1)
	{
		cout << menu;

		string input;
		getline(cin, input);
		istringstream in(input);

		system("CLS");
		cout << input << endl;

		// Valid data types
		if (!(in >> startingCity && in >> endingCity && in >> startingTimeStr) || strlen(startingTimeStr) > 4)
		{
			cout << "\nInvalid Input! Please, follow the correct format (e.g. <1 3 1230>)\n" << endl;
			continue;
		}
		// Starting and Ending city being the same
		else if (startingCity == endingCity)
		{
			cout << "\nInvalid Input! Starting city and Ending city must be different.\n" << endl;
			continue;
		}
		// If invalid range
		else if (!(isValidRange(startingCity, 1, TOTAL_CITIES) && isValidRange(endingCity, 1, TOTAL_CITIES)))
		{
			cout << "\nInvalid Input! The City number is out of range.\n" << endl;
			continue;
		}

		// Parse and convert the starting time
		double startingTime = timeToHour(startingTimeStr);

		int startingCityIndex = startingCity - 1;
		int endingCityIndex = endingCity - 1;

		struct Node nodes[TOTAL_CITIES] = { 0 };
		vector<Edge> edges[TOTAL_CITIES];

		createGraph(nodes, edges);

		// We work with Eastern Time By default and convet
		// e.g. EST Time = CST time (18) - EST Difference (-1) = 19 EST
		double easternStartingTime = startingTime - nodes[startingCityIndex].estDifference;
		double totalTime = 0.0;

		// Use the dijkstra graph path finding algorithm
		totalTime = dijkstra(nodes, edges, startingCityIndex, endingCityIndex, easternStartingTime);

		displayShortestPath(nodes, edges, startingCityIndex, endingCityIndex, totalTime);
	}
	return 0;
}




// Function: 
//			isValidRange
// Description: 
//			Checks if the value entered by the user is within the range of 
//			the p0/<ermitted values
// Paratemers: 
//			int value: the value to be checked
//			int min: the lower range
//			int max: the upper range
// Return Value: 1 if the value is within the range, 0 otherwise
int isValidRange(int value, int minRange, int maxRange)
{
	if (value < minRange || TOTAL_CITIES < value)
	{
		return 0;
	}
	return 1;
}