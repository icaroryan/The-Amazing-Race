// Filename: AmazingRace.cpp
// Project: Major Assignment #3a
// By: Icaro Ryan Oliveira Souza
// Date: March 08, 2021
// Description: This file contains the function definitions 
//					for the Amazing Race functionalitites



#include "..\..\Headers\include\AmazingRace.h"



// Function: 
// 			dijkstra
// Description: 
// 			Make use of the dijstra algorith to find the shortest route and distance from all nodes (cities) in a 
//			graph to the source node (home city)
// Parameters: 
// 			Node* nodes: pointer to an array of Node (city) Structs that contains all cities' data
//			vector<Edge>* edges: pointer to an array of vectors of Edge (flights) Structs, which contains all flights' data
//			int src: Starting city
//			int destination: Destination City
//			double startingTime: Starting time entered by the user in the beginning
// Return Values: 
//			double with the minimum distance from src to destination
double dijkstra(Node* nodes, vector<Edge>* edges, int src, int destination, double startingTime)
{
	constexpr double kInfinity = numeric_limits<double>::max();

	// Minimum distance from all cities(node) in the graph to the source city(node)
	double minDist[TOTAL_CITIES] = { 0 };

	list<int> queue;
	for (int i = 0; i < TOTAL_CITIES; i++)
	{
		// Assign all distances to to infinity, except the source node
		if (i != src)
		{
			minDist[i] = kInfinity;
		}
		nodes[i].arrivalTime = startingTime;
		// Add node to queue
		queue.push_back(i);
	}

	// Keep track of visited vertices
	bool visited[TOTAL_CITIES] = { false };


	while (!queue.empty())
	{
		// The current node will be the one with the smallest distance from the source node and not visited yet (src node in the first interation)
		int v = smallestV(minDist, visited, kInfinity);

		// Remove the current vertex from the queue as the node is now known
		queue.remove(v);

		// Update the distance from v to each node connected to it (neighbors) -> process of relaxation
		for (list<int>::iterator u = nodes[v].neighbors.begin(); u != nodes[v].neighbors.end(); ++u)
		{
			if (!visited[*u])
			{
				// Departure time of the closest flight so far (not converted to EST timezone)
				double departureTime = 0.0;
				double currentTime = nodes[v].arrivalTime;
				// Get the min distance so far of the current node (v) from the src and sum it to the distance of the neighbor(u) to v
				double currentDist = getCost(nodes, edges, v, *u, currentTime, kInfinity, &departureTime);
				double newDist = minDist[v] + currentDist;

				// If a new better path to u was found
				if (newDist < minDist[*u])
				{
					// Update the data of U to the fastest route
					minDist[*u] = newDist;
					nodes[*u].departureTimePrevNode = departureTime;
					nodes[*u].arrivalTime = nodes[v].arrivalTime + currentDist;
					nodes[*u].prevNodeRoute = v;
				}
			}
		}
		visited[v] = true;

	}

	return minDist[destination];
}




// Function: 
//			timeToHour
// Description: 
//			Converts a time in the "hhmm" string format to hours
// Parameters: 
// 			const char* time: string with the time
// Return Values: 
//			double of the time converted to hours
double timeToHour(const char* time)
{
	char timeStr[kArraySizeBuffer] = { 0 };

	// Invalid Format
	if (strlen(time) > 4)
	{
		exit(1);
	}
	// If we have 3 digits, we will pad a leading 0's to avoid avoid any issue with sscanf
	else if (strlen(time) == 3)
	{
		snprintf(timeStr, kArraySizeBuffer, "%04s", time);
	}
	else
	{
		snprintf(timeStr, kArraySizeBuffer, "%s", time);
	}

	int hours = 0;
	int minutes = 0;

	// If at least hours don't get a value, exit the program
	if (sscanf(timeStr, "%02d%02d", &hours, &minutes) < 1)
	{
		exit(1);
	}

	//  Convert minutes to hours (with floating point) and sum it to hours variable
	double convertedTime = hours + (minutes / (double)kOneHourM);

	return convertedTime;
}



// Function: 
// 			createGraph
// Description: 
// 			Creates and initializes the the undirected weighted multigraph with the nodes (cities) and all its edges (flights)
// Parameters: 
// 			Node* nodes: pointer to an array of Node (city) Structs that contains all cities' data
//			vector<Edge>* edges: pointer to an array of vectors of Edge (flights) Structs, which contains all flights' data
// Return Values: 
//			nothing (void)
void createGraph(Node* nodes, vector<Edge>* edges)
{
	string vertexList[] = { "Toronto", "Atlanta", "Austin", "Santa Fe", "Denver", "Chicago", "Buffalo" };

	// The adjacency list tell whether a city is connectec to the other or not
	list<int> adjacencyList[TOTAL_CITIES];
	populateAdjList(adjacencyList);

	// Go through all edges and initialize variables and neighbors
	for (int v = 0; v < TOTAL_CITIES; v++)
	{
		nodes[v].node = v;
		nodes[v].cityName = vertexList[v];
		nodes[v].prevNodeRoute = 0;
		nodes[v].prevNodeRoute = -1;
		nodes[v].arrivalTime = 0.0;
		setTimeZone(nodes, v);

		// Go over each connection of that particular edge
		for (list<int>::iterator u = adjacencyList[v].begin(); u != adjacencyList[v].end(); ++u)
		{
			// Add neighbor
			nodes[v].neighbors.push_back(*u);

			// Get a list with all flight data from the city v to the city u
			list<FlightSchedule> schedule;
			getFlightSchedule(&schedule, v, *u);

			// Populate the edges list (v, u, list<FlightSchedule> times)
			// Connection from v to u
			Edge edge = { v, *u, schedule };
			edges[v].push_back(edge);
		}
	}
}



// Function: 
// 			populateAdjList
// Description: 
//			Populate the adjency list for the graph by adding the edges
// Parameters: 
// 			list<int> adjacencyList[]: array of lists that will store all connections (edges) between cities
// Return Values: 
//			nothing (void)
void populateAdjList(list<int> adjacencyList[])
{
	addEdge(adjacencyList, 0, 1);
	addEdge(adjacencyList, 0, 4);
	addEdge(adjacencyList, 0, 5);
	addEdge(adjacencyList, 1, 2);
	addEdge(adjacencyList, 1, 5);
	addEdge(adjacencyList, 1, 4);
	addEdge(adjacencyList, 5, 4);
	addEdge(adjacencyList, 5, 6);
	addEdge(adjacencyList, 2, 4);
	addEdge(adjacencyList, 2, 3);
}



// Function: 
// 			addEdge
// Description: 
// 			Create an edge on both ways (V <-> U), since it's an undirected graph
// Parameters: 
// 			list<int> adjacencyList[]: array of lists that will store all connections (edges) between cities
//			int v: Origin city
//			int u: Destination city
// Return Values:
//			nothing (void)
void addEdge(list<int> adjacencyList[], int v, int u)
{
	adjacencyList[u].push_back(v);
	adjacencyList[v].push_back(u);
}



// Function: 
// 			setTimeZone
// Description: 
// 			Sets the time zone and time difference from EST (on nodes) for the city (node) in question
// Parameters: 
// 			Node* nodes: pointer to an array of Node Structs that contains all cities' data
//			int cityIndex: index of the city in question
// Return Values: 
//			nothing (void)
void setTimeZone(Node* nodes, int cityIndex)
{
	// Toronto, Atlanta and Buffalo
	if (cityIndex == 0 || cityIndex == 1 || cityIndex == 6)
	{
		nodes[cityIndex].timeZone = "EST";
		nodes[cityIndex].estDifference = 0;
	}
	// Austin and Chicago
	else if (cityIndex == 2 || cityIndex == 5)
	{
		nodes[cityIndex].timeZone = "CST";
		nodes[cityIndex].estDifference = -1;
	}
	// Santa Fe, Denver
	else if (cityIndex == 3 || cityIndex == 4)
	{
		nodes[cityIndex].timeZone = "MST";
		nodes[cityIndex].estDifference = -2;
	}

}



// Function: 
// 			getFlightSchedule
// Description: 
// 			It pushes back all flight schedules from v (city origin) to u (its city neighbor) to a list
// Parameters: 
// 			list<FlightSchedule> schedule[]: pointer to a list of flights schedule
//			int v: Origin city (node)
//			int u: Destination city (node)
// Return Values: 
//			nothing (void)
void getFlightSchedule(list<FlightSchedule>* schedule, int v, int u)
{
	// Home
	switch (v)
	{
		// Toronto
	case 0:
		// Destination
		switch (u)
		{
			// -> Atlanta
		case 1:
			*schedule = { {timeToHour("625"), timeToHour("220")}, {timeToHour("910"), timeToHour("450")}, {timeToHour("1230"), timeToHour("415")},
						{timeToHour("1610"), timeToHour("610")}, {timeToHour("20"), timeToHour("215")} };
			break;
			// -> Denver
		case 4:
			*schedule = { {timeToHour("730"), timeToHour("335")}, {timeToHour("15"), timeToHour("6")} };
			break;
			// -> Chicago
		case 5:
			*schedule = { {timeToHour("640"), timeToHour("120")}, {timeToHour("740"), timeToHour("135")}, {timeToHour("840"), timeToHour("135")},
						{timeToHour("940"), timeToHour("135")}, {timeToHour("1040"), timeToHour("135")}, {timeToHour("1140"), timeToHour("135")},
						{timeToHour("1240"), timeToHour("135")}, {timeToHour("1340"), timeToHour("135")}, {timeToHour("1440"), timeToHour("135")},
						{timeToHour("1530"), timeToHour("145")}, {timeToHour("1630"), timeToHour("145")}, {timeToHour("1730"), timeToHour("145")},
						{timeToHour("1830"), timeToHour("145")}, {timeToHour("1930"), timeToHour("145")}, {timeToHour("21"), timeToHour("130")},
						{timeToHour("22"), timeToHour("115")} };
			break;
		}
		break;
		// Atlanta
	case 1:
		switch (u)
		{
			// -> Toronto
		case 0:
			*schedule = { {timeToHour("710"), timeToHour("210")}, {timeToHour("1030"), timeToHour("410")}, {timeToHour("15"), timeToHour("350")},
						{timeToHour("1710"), timeToHour("610")}, {timeToHour("21"), timeToHour("220")} };
			break;
			// -> Austin
		case 2:
			*schedule = { {timeToHour("9"), timeToHour("210")}, {timeToHour("1530"), timeToHour("250")}, {timeToHour("20"), timeToHour("230")} };
			break;
			// -> Denver
		case 4:
			*schedule = { {timeToHour("6"), timeToHour("3")}, {timeToHour("1320"), timeToHour("5")}, {timeToHour("1710"), timeToHour("250")} };
			break;
			// -> Chicago
		case 5:
			*schedule = { {timeToHour("650"), timeToHour("210")}, {timeToHour("750"), timeToHour("3")}, {timeToHour("850"), timeToHour("3")},
						{timeToHour("950"), timeToHour("3")}, {timeToHour("1050"), timeToHour("3")}, {timeToHour("1150"), timeToHour("3")},
						{timeToHour("1250"), timeToHour("3")}, {timeToHour("1350"), timeToHour("3")}, {timeToHour("1450"), timeToHour("3")},
						{timeToHour("1550"), timeToHour("230")}, {timeToHour("1650"), timeToHour("230")}, {timeToHour("1750"), timeToHour("230")},
						{timeToHour("1850"), timeToHour("230")}, {timeToHour("1950"), timeToHour("230")}, {timeToHour("2030"), timeToHour("210")}, };
			break;
		}
		break;
		// Austin
	case 2:
		switch (u)
		{
			// -> Atlanta
		case 1:
			*schedule = { {timeToHour("910"), timeToHour("220")}, {timeToHour("15"), timeToHour("220")}, {timeToHour("2130"), timeToHour("230")} };
			break;
			// -> Denver
		case 4:
			*schedule = { {timeToHour("1030"), timeToHour("220")}, {timeToHour("1820"), timeToHour("220")} };
			break;
			// -> Santa Fe
		case 3:
			*schedule = { {timeToHour("17"), timeToHour("055")} };
			break;
		}
		break;
		// Santa Fe
	case 3:
		switch (u)
		{
			// -> Austin
		case 2:
			*schedule = { {timeToHour("15"), timeToHour("045")} };
			break;
		}
		break;
		// Denver
	case 4:
		switch (u)
		{
			// -> Toronto
		case 0:
			*schedule = { {timeToHour("630"), timeToHour("410")}, {timeToHour("1030"), timeToHour("520")}, {timeToHour("14"), timeToHour("5")} };
			break;
			// -> Atlanta
		case 1:
			*schedule = { {timeToHour("6"), timeToHour("310")}, {timeToHour("13"), timeToHour("320")}, {timeToHour("15"), timeToHour("350")} };
			break;
			// -> Austin
		case 2:
			*schedule = { {timeToHour("12"), timeToHour("2")}, {timeToHour("15"), timeToHour("220")} };
			break;
			// -> Chicago
		case 5:
			*schedule = { {timeToHour("7"), timeToHour("220")}, {timeToHour("8"), timeToHour("250")}, {timeToHour("10"), timeToHour("250")},
						{timeToHour("12"), timeToHour("250")}, {timeToHour("14"), timeToHour("250")}, {timeToHour("16"), timeToHour("250")},
						{timeToHour("1830"), timeToHour("240")} };
			break;
		}
		break;
		// Chicago
	case 5:
		switch (u)
		{
			// -> Toronto
		case 0:
			*schedule = { {timeToHour("740"), timeToHour("110")}, {timeToHour("910"), timeToHour("230")}, {timeToHour("1010"), timeToHour("230")},
						{timeToHour("1110"), timeToHour("230")}, {timeToHour("1210"), timeToHour("230")}, {timeToHour("1310"), timeToHour("230")},
						{timeToHour("1410"), timeToHour("230")}, {timeToHour("1510"), timeToHour("230")}, {timeToHour("1610"), timeToHour("230")},
						{timeToHour("1710"), timeToHour("230")}, {timeToHour("1910"), timeToHour("2")}, {timeToHour("2110"), timeToHour("210")} };
			break;
			// -> Atlanta
		case 1:
			*schedule = { {timeToHour("650"), timeToHour("210")}, {timeToHour("8"), timeToHour("240")}, {timeToHour("9"), timeToHour("240")},
						{timeToHour("10"), timeToHour("240")}, {timeToHour("11"), timeToHour("240")}, {timeToHour("12"), timeToHour("240")},
						{timeToHour("13"), timeToHour("240")}, {timeToHour("14"), timeToHour("240")}, {timeToHour("15"), timeToHour("240")},
						{timeToHour("16"), timeToHour("240")}, {timeToHour("17"), timeToHour("240")}, {timeToHour("18"), timeToHour("240")},
						{timeToHour("19"), timeToHour("240")}, {timeToHour("20"), timeToHour("240")}, {timeToHour("950"), timeToHour("3")} };
			break;
			// -> Denver
		case 4:
			*schedule = { {timeToHour("9"), timeToHour("210")}, {timeToHour("1130"), timeToHour("220")}, {timeToHour("1330"), timeToHour("220")},
						{timeToHour("1530"), timeToHour("220")}, {timeToHour("1730"), timeToHour("220")}, {timeToHour("21"), timeToHour("250")}, };
			break;
			// -> Buffalo
		case 6:
			*schedule = { {timeToHour("11"), timeToHour("2")}, {timeToHour("1310"), timeToHour("150")}, {timeToHour("15"), timeToHour("230")},
						{timeToHour("18"), timeToHour("210")} };
			break;
		}
		break;
		// Buffalo
	case 6:
		switch (u)
		{
			// -> Chicago
		case 5:
			*schedule = { {timeToHour("940"), timeToHour("140")}, {timeToHour("1110"), timeToHour("150")}, {timeToHour("1740"), timeToHour("240")},
						{timeToHour("2010"), timeToHour("220")} };
			break;
		}
		break;
	}
}



// Function: 
// 			smallestV
// Description: 
// 			Get the non-visited node (city) with the smallest distance (total time) from the source node (city)
// Parameters: 
// 			double minDist[]: array with the minimum distances to the source node (city)
//			bool visited[]: array that tells if the city was already visited or not
//			double kInfinity: max value for a double
// Return Values: 
//			return the index of the closest non-visited node to source node
int smallestV(double minDist[], bool visited[], double kInfinity)
{
	double min = kInfinity;
	int minCityIdx = -1;

	for (int cityIdx = 0; cityIdx < TOTAL_CITIES; cityIdx++)
	{
		if (!visited[cityIdx] && minDist[cityIdx] <= min)
		{
			min = minDist[cityIdx];
			minCityIdx = cityIdx;
		}
	}

	return minCityIdx;
}



// Function: 
// 			getCost
// Description: 
// 			This functions calculates the closest distance (flyingTime + layoverTime) from the origin city to the flight destination city
// Parameters: 
// 			Node* nodes: pointer to an array of Node (city) Structs that contains all cities' data
//			vector<Edge>* edges: pointer to an array of vectors of Edge (flights) Structs that contains all flights' data
//			int v: Origin city (node)
//			int u: Flight destination city (node)
//			double currentTime: Arrival time on city v, which is the current time now
//			double kInfinity: max value for a double
//			double* departureTime: pointer to a variable that stores the closest departureTime from v to u
// Return Values: 
//			double of the running total of the closest distance from source to u passing through node v
double getCost(Node* nodes, vector<Edge> edges[], int v, int u, double currentTime, double kInfinity, double* departureTime)
{
	// Get the current time in the day
	currentTime = fmod(currentTime, kOneDayH);
	double dist = kInfinity;

	// Go over all edges connecting v to find the earliest flight possible to u
	for (auto edge = edges[v].begin(); edge != edges[v].end(); edge++)
	{
		if (edge->u == u)
		{
			// Iterate over all scheduled flights from v to u
			for (auto flightTime = edge->times.begin(); flightTime != edge->times.end(); ++flightTime)
			{
				double layoverTime = 0.0;

				// We convert everything to est time as default
				double estDepartureTime = flightTime->departureTime - nodes[v].estDifference;
				// Get the closest flight to the current time -> They're already sorted, so no worries with getting the closest time
				if (estDepartureTime >= currentTime)
				{

					*departureTime = flightTime->departureTime;
					layoverTime = estDepartureTime - currentTime;
					dist = flightTime->flyingTime + layoverTime;

					break;
				}
				// If it's the last scheduled time, wait until the first flight of the next day
				if (next(flightTime, 1) == edge->times.end())
				{

					// Get the first available flight next day
					auto firstFlight = edge->times.begin();

					estDepartureTime = firstFlight->departureTime - nodes[v].estDifference;
					*departureTime = firstFlight->departureTime;
					// Sum the number of hours to finish the day to the departure time of the first flight next day
					layoverTime = (kOneDayH - currentTime) + estDepartureTime;
					dist = firstFlight->flyingTime + layoverTime;

					break;
				}
			}
			break;
		}
	}
	return dist;
}



// Function: 
// 			displayShortestPath
// Description: 
// 			Display the fastest route to get from the home city to the destination city and the total time it took
// Parameters: 
// 			Node* nodes:
//			vector<Edge>* edges: pointer to an array of vectors of Edge (flights) Structs, which contains all flights' data
//			int src: Starting city
//			int destination: Destination City
//			double totalTime: Total travel time
// Return Values: 
//			nothing (void)
void displayShortestPath(Node* nodes, vector<Edge>* edges, int src, int destination, double totalTime)
{
	char arrivalTimeStr[kArraySizeBuffer] = { 0 };
	char departureTimeStr[kArraySizeBuffer] = { 0 };
	char totalTimeStr[kArraySizeBuffer] = { 0 };
	char startingTimeStr[kArraySizeBuffer] = { 0 };
	double arrivalTime = 0.0;

	list<int> path;
	getPath(nodes, path, src, destination);


	// Convert all times to their correspondent timezones
	fixTimezones(nodes, path);

	cout << "\nFlying from " << nodes[src].cityName << " to " << nodes[destination].cityName << "." << endl;
	cout << "----------------------------------------------\n" << endl;

	// The arrivalTime of the Source city is the same as the starting time
	double startingTime = nodes[src].arrivalTime;
	hourToTime(startingTimeStr, startingTime);
	cout << "Starting from " << nodes[src].cityName << " at " << startingTimeStr << " " << nodes[src].timeZone << "\n" << endl;



	for (list<int>::iterator city = path.begin(); city != path.end(); ++city)
	{
		if (*city != destination)
		{
			// +1 means the next day and +2 means 2 days after
			string nextDay;
			auto nextCity = next(city, 1);

			// nodes[*nextCity].departureTimePrevNode stores the departure time (in hours), from the midnight of the current day, from the current city to the next one
			// e.g. 16 means 16 hours past midnight of the current day (4 p.m.)
			double departureTime = nodes[*nextCity].departureTimePrevNode;
			hourToTime(departureTimeStr, departureTime);

			// nodes[*city].arrivalTime store the arrival time in number of hours from the midnight of the first travel day 
			// e.g. 30.5 means 30 hours and 30 minutes past midnight of the first travel day (real arrivalTime = 6:30 a.m.)
			arrivalTime = nodes[*city].arrivalTime;

			// If the time left from arrivalTime to the second midnight (48h) is lesser that the time left from the departureTime to MN of the current day
			// e.g. if the arrivalTime to the current city is 40 (4 p.m) and we're leaving at 10 a.m (next day), we know that we're 2 days ahead the inital
			//		because 48 - 40 = 8 left to midnight (arrivalTime) and 24 - 10 = 14 left to midnight (departureTime). Thus, since 8 is < 14, we're leaving the next day
			if (kTwoDayH - arrivalTime < kOneDayH - departureTime)
			{
				nextDay = " +2";
			}
			// If the time left to midnight (24h) from the arrivalTime time is lesser than the time left to MN from the departureTime
			// e.g. if we arrive with 4 hours left to midnight (8 p.m) and leave with 3 hours left (9 p.m.), it's the same day
			// e.g. if we arrive with 4 hours left to midnight (8 p.m) and leave with 11 hours left (1 p.m.), it's the next day
			else if (kOneDayH - arrivalTime < kOneDayH - departureTime)
			{
				nextDay = " +1";
			}

			cout << "Leaving " << nodes[*city].cityName << " at " << departureTimeStr << " " << nodes[*city].timeZone << nextDay << " for " << nodes[*nextCity].cityName << endl;

			arrivalTime = nodes[*nextCity].arrivalTime;
			hourToTime(arrivalTimeStr, fmod(arrivalTime, kOneDayH));

			nextDay = "";

			// +2 if the arrival time it's 2 days after
			if (arrivalTime > kTwoDayH)
			{
				nextDay = " +2";
			}
			// +1 if the arrival time it's in the next day
			else if (arrivalTime > kOneDayH)
			{
				nextDay = " +1";
			}


			cout << "Arriving in " << nodes[*nextCity].cityName << " at " << arrivalTimeStr << " " << nodes[*nextCity].timeZone << nextDay << "\n" << endl;
		}
	}

	calculateTotalTime(totalTimeStr, totalTime);
	cout << "Total Travel Time (hh:mm) >> " << totalTimeStr << endl;

	cout << "\n\n";
}



// Function: 
// 			getPath
// Description:
// 			Store the shortest path from src to destination in a list
// Parameters: 
// 			Node* nodes: Pointer to an array of Node (city) Structs
//			list<int>& path: List to store the shortest path from destination to src 
//			int src: Starting (Home) city
//			int destination: Destination city
// Return Values: 
//			nothing (void)
void getPath(Node* nodes, list<int>& path, int src, int destination)
{
	for (int node = 0; node < TOTAL_CITIES; node++)
	{
		if (node == destination)
		{
			// One node points to the other
			// We follow the path until we get to the source node (Ending City -> Starting City)
			int prevNode = node;

			do
			{
				path.push_back(prevNode);
				prevNode = nodes[prevNode].prevNodeRoute;

			} while (prevNode != src);
			path.push_back(src);

			// We have to reverse the path because it's backwards (Ending City -> StartingCity) => (StartingCity -> Ending City)
			path.reverse();
			break;
		}
	}
}



// Function: 
// 			fixTimezones
// Description: 
//			Convert all arrivalTime back to their original timezones (everything was on EST time)
// Parameters: 
//			Node* nodes: pointer to an array of Node (city) Structs
//			list<int> path: List with the shortest path from the home city to destination
// Return Values: 
//			nothing (void)
void fixTimezones(Node* nodes, list<int> path)
{
	// e.g.MST time = EST time(15) + EST difference(-2) = 13 MST
	for (list<int>::iterator city = path.begin(); city != path.end(); city++)
	{
		// We don't need to convert "departureTimePrevNode" because the departure time is already assigned in its original timezone
		nodes[*city].arrivalTime += nodes[*city].estDifference;
	}
}



// Function: 
//			hourToTime
// Description:
//			Convert a double with the time in hours to formatted string (HH:MM a.m./p.m.)
// Parameters: 
//			char strTime[]: string to save the formatted string
//			double time: time in hours
// Return Values: 
//			nothing (void)
void hourToTime(char strTime[], double time)
{
	// Truncate the decimal part (minutes) of time
	int hours = (int)time;
	int minutes = (int)round((time - hours) * kOneHourM);
	if (minutes == kOneHourM)
	{
		hours++;
		minutes = 0;
	}

	const char* meridiem = NULL;

	// a.m.
	if (hours < 12)
	{
		meridiem = "a.m.";
	}
	// p.m
	else
	{
		meridiem = "p.m.";
		hours %= 12;
	}

	// if "12 p.m."
	if (hours == 0)
	{
		snprintf(strTime, kArraySizeBuffer, "12:%02d %s", minutes, meridiem);
	}
	else
	{
		snprintf(strTime, kArraySizeBuffer, "%02d:%02d %s", hours, minutes, meridiem);
	}
}



// Function: 
//			calculateTotalTime
// Description:
//			This function convert the total time in hours (double) to a formatted string ((hh:mm))
// Parameters: 
//			char strTime[]: String to store the total time formatted 
//			double totalTime: total time in hours
// Return Values: 
//			nothing (void)
void calculateTotalTime(char strTime[], double totalTime)
{
	int hours = (int)totalTime;
	int minutes = (int)round((totalTime - hours) * kOneHourM);

	sprintf(strTime, "%02d:%02d", hours, minutes);
}
