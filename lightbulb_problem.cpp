/*
Kristina Kolibab
Algorithms - Assignment 1 
Professor Tamon
Oct. 30th, 2018
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <functional>
#include <list>
#include <string>

using std::cout;
using std::endl;
using std::ifstream;
using std::cerr;
using std::rand;
using std::vector;
using std::iterator;
using std::negate;
using std::list;
using std::string;
using std::cin;

// declarations
struct Bulbs;
struct Switch;
bool isSat(vector<Bulbs> &b, int s[], int &n);
void DFSUtil(Switch &currentSwitch, bool &pathFound);
int DFS(vector<Switch> &switches);
void printConfig(int s[], int &n);

/*
This struct will contain a bool designating whether
a bulb is on or not, the configuration specifications 
for each bulb, and a constructor to set the configuration
*/
struct Bulbs{
	bool isOn = false;  // don't really need this variable
	int config[2];
	Bulbs(int a, int b) : config{a,b} {};	
};


// Struct to hold info about switch nodes
struct Switch{
	const int n; // 1/-1, 2/-2, etc. the label of the switch
	vector<int> sv; // holds all other switches it points to
	Switch(int n_) : n(n_) {};
};

/* 
This will be the function to decide if the bulbs and 
configurations are satisfiable, if so it will return true.
This function takes as arguments the vector b of bulbs, 
the array of configured switches s[], and the number of 
switches n
*/
bool isSat(vector<Bulbs> &bulbs, int s[], int &n){
	int counter = 0;
	bool sat;
	do{
		sat = true;
		for(Bulbs &b : bulbs){
			if((b.config[0] != s[abs(b.config[0])-1]) && (b.config[1] != s[abs(b.config[1])-1])){
				if(rand() % 2 == 0){
					s[abs(b.config[0])-1] = -1*(s[abs(b.config[0])-1]);
				}
				else{
					s[abs(b.config[1])-1] = -1*(s[abs(b.config[1])-1]);
				}
				sat = false;  
			}
			b.isOn = true;
		}
		cout << "i: " << counter << endl;
		counter++;
	} while (sat == false && counter <= n*n);	

	if(sat == false) return false;
	else return true;
}

// Helper function used by BFS to test switches, this is where
// the main magic happens
bool isReachable(Switch &s1, Switch &s2, vector<Switch> &switches){
	// base case
	int s, d = 0;
	s = s1.n;
	d = s2.n;

	if(s == d) return true;
		
	int V = switches.size();
	bool *visited = new bool[V];
	for(int i = 0; i < V; i++) visited[i] = false;

	list<int> queue;
		
	if(s > 0) visited[(abs(s)*2)-2] = true;
	else visited[(abs(s)*2)-1] = true;
	queue.push_back(s);	

	while(!queue.empty())
	{
		s = queue.front();
		queue.pop_front();
	
		// some jank I have to do for the +/- switch values
		if(s > 0){
			for(auto it = switches.at((abs(s)*2)-2).sv.begin(); it != switches.at((abs(s)*2)-2).sv.end(); ++it){
				if(*it == d) return true;
				if(*it > 0){
					if(!visited[(abs(*it)*2)-2]){
						visited[(abs(*it)*2)-2] = true;
						queue.push_back(*it);
					}
				} else {
					if(!visited[(abs(*it)*2)-1]){
						visited[(abs(*it)*2)-1] = true;
						queue.push_back(*it);
					}
				}
			}
		} else {
			for(auto it = switches.at((abs(s)*2)-1).sv.begin(); it != switches.at((abs(s)*2)-1).sv.end(); ++it){
				if(*it == d) return true;
				if(*it > 0){
					if(!visited[(abs(*it)*2)-2]){
						visited[(abs(*it)*2)-2] = true;
						queue.push_back(*it);
					}
				} else {
					if(!visited[(abs(*it)*2)-1]){
						visited[(abs(*it)*2)-1] = true;
						queue.push_back(*it);
					}
				}
			}
		}
	}
	// if you never visited d, return false
	return false;
}

// BFS function to test +/- switches simultaneously
int BFS(vector<Switch> &switches){
	for(auto it = switches.begin(); it != switches.end(); it++){
		Switch s1 = *it;
		it++;
		Switch s2 = *it;
		if(isReachable(s1, s2, switches) and isReachable(s2, s1, switches)){
			cout << "Match found on " << s1.n << " and " << s2.n << endl;
			return s1.n;
		}		
	}
	cout << "No match found\n";
	return 0;
}

// Function to print configuration of switches 
void printConfig(int s[], int &n){
	cout << "Configuration:\n";
	for(int i = 0; i < n; i++) cout << s[i] << " ";
	cout << endl;
}

// Driver code
int main(){

	// read in file
	cout << "Name of input file: ";
	string inputFile;
	getline(cin, inputFile);

	ifstream in(inputFile);
	if(!in){
		cerr << "Could not open file\n";
		exit(1);
	}
	
	// create and set variables
	int noBulbs, noSwitches = 0;
	in >> noSwitches; // here
	in >> noBulbs; // here

	int switches[noSwitches];
	for(int i = 0; i < noSwitches; i++){
		// if 0, flip switch off
		if(rand() % 2 == 0) switches[i] = (-1)*(i+1);
		// if 1, flip switch on
		else switches[i] = i+1;
	}

	// the first value is always 0, not sure why
	switches[0] = 1;

	vector<Bulbs> bulbs;
	int a, b, j = 0;
	int configs[noBulbs*2]; // bulbs have 2 switches
	for(int i = 0; i < noBulbs; i++){
		in >> a >> b; // here
		// save a,b for later use too
		configs[j] = a;
		configs[j+1] = b;
		j += 2; 
		bulbs.push_back(Bulbs(a, b));
	}
	
	if(isSat(bulbs, switches, noSwitches)){
		printConfig(switches, noSwitches);
	} else {
		// for number of switches, create +/- nodes 
		vector<Switch> s;		
		for(int i = 1; i < noSwitches+1; i++){
			s.push_back(Switch(i)); 
			s.push_back(Switch(-1*i));
		} 
		// use configs array, grab 2 per iteration, [a,b]
		// add to the vector, the int of all nodes that node i/i+1 points to
		for(int i = 0; i < noBulbs*2; i+=2){
			// 4 cases, [a,b], [-a,-b], [-a,b], [a,-b]
			if(configs[i] < 0 and configs[i+1] < 0){ 
				s.at((abs(configs[i])*2)-2).sv.push_back(configs[i+1]);
				s.at((abs(configs[i+1])*2)-2).sv.push_back(configs[i]);
			} else if(configs[i] < 0 and configs[i+1] > 0){
				s.at((abs(configs[i])*2)-2).sv.push_back(configs[i+1]);
				s.at((configs[i+1]*2)-1).sv.push_back(configs[i]);	
			} else if(configs[i] > 0 and configs[i+1] > 0){
				s.at((configs[i]*2)-1).sv.push_back(configs[i+1]);
				s.at((configs[i+1]*2)-1).sv.push_back(configs[i]);
			} else if(configs[i] > 0 and configs[i+1] < 0){ 
				s.at((configs[i]*2)-1).sv.push_back(configs[i+1]);
				s.at((abs(configs[i+1])*2)-2).sv.push_back(configs[i]);
			}
		}		
		int answer = BFS(s);
		cout << "Faulty Switch: " << answer << endl;
	}

	in.close();

	return 0;
}
