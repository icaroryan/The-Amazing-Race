// Filename: AmazingRace.h
// Project: Major Assignment #3a
// By: Icaro Ryan Oliveira Souza
// Date: March 08, 2021
// Description: This file contains the prototypes, constants and structs 
//				for the Amazing Race functionalitites 


#pragma once


#include <iostream>
#include <string>
#include <cstring>
#include <list>
#include <vector> 
#include <limits>
#include <cmath>


using namespace std;


#define TOTAL_CITIES 7
#define kArraySizeBuffer 80
#define kOneDayH 24
#define kTwoDayH 48
#define kOneHourM 60


struct FlightSchedule
{
	double departureTime;
	double flyingTime;
};


struct Edge
{
	int v;
	int u;
	list<FlightSchedule> times;
};


struct Node
{
	int node{};
	string cityName;
	int prevNodeRoute{};
	list<int> neighbors;
	double arrivalTime{};
	double departureTimePrevNode{};
	string timeZone;
	int estDifference{};
};


double dijkstra(Node* nodes, vector<Edge>* edges, int src, int destination, double startingTime);
double timeToHour(const char* time);
void createGraph(Node* nodes, vector<Edge>* edges);
void populateAdjList(list<int> adjacencyList[]);
void addEdge(list<int> adjacencyList[], int v, int u);
void setTimeZone(Node* nodes, int cityIndex);
void getFlightSchedule(list<FlightSchedule>* schedule, int v, int u);
int smallestV(double minDist[], bool visited[], double kInfinity);
double getCost(Node* nodes, vector<Edge> edges[], int v, int u, double currentTime, double kInfinity, double* departureTime);
void displayShortestPath(Node* nodes, vector<Edge>* edges, int src, int destination, double totalTime);
void getPath(Node* nodes, list<int>& path, int src, int destination);
void fixTimezones(Node* nodes, list<int> path);
void hourToTime(char strTime[], double time);
void calculateTotalTime(char strTime[], double totalTime);
