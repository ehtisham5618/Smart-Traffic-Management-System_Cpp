# include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <windows.h>
#include <conio.h>
#include <cstdlib>

using namespace std;

const int roads = 26;
const int max_vehicles = 50;
const int intMax = 2147483647; // Maximum value of a 32-bit signed integer

struct QueueElement {
    int nodeIndex;
    string path;
};

class Queue {
private:
    static const int SIZE = 100; // Maximum size of the queue
    int front, rear;

    QueueElement elements[SIZE]; // Array to store Queue elements

public:
    Queue() {
        front = 0; 
        rear = 0;
    }

    bool isEmpty() {
        return front == rear;
    }

    bool isFull() {
        return (rear + 1) % SIZE == front;
    }

    void enqueue(int nodeIndex, string path) {
        if(isFull()) {
            cout << "Queue overflow!" << endl;
            return;
        }

        elements[rear].nodeIndex = nodeIndex;
        elements[rear].path = path;
        rear = (rear + 1) % SIZE;
    }

    QueueElement dequeue() {
        if(isEmpty()) {
            cout << "Queue underflow!" << endl;
            return {-1, ""}; // Return an invalid value
        }

        QueueElement element = elements[front];
        front = (front + 1) % SIZE;
        return element;
    }
};

class priorityQueue {
private:    
    int *num_of_vehicles, *lvl;
    string *roads;
    int front = 0, rear = 0;

public:
    priorityQueue() {
        num_of_vehicles = new int [50];
        roads = new string [50];
        lvl = new int [50];
    }

    void enqueue(string rd , int traffic, int level) {
        roads[rear] = rd;
        num_of_vehicles[rear] = traffic;
        lvl[rear] = level ;
        rear++;
    }

    void empty() {
        front = rear = 0;
    }

    bool isEmpty() {
        return front >= rear;
    }

    // Function to arrange the queue in a descending order
    void arrange() {
        for(int i = front; i < rear - 1; i++) {
            for(int j = front; j < rear - (i + 1); j++) {
                if(lvl[j] < lvl [j+1]) {
                    // Swap arr[j] and arr[j + 1]
                    int temp = num_of_vehicles[j];
                    num_of_vehicles[j] = num_of_vehicles[j + 1];
                    num_of_vehicles[j + 1] = temp;

                    string temp1 = roads[j];
                    roads[j] = roads[j + 1];
                    roads[j + 1] = temp1;
                }
                else if(num_of_vehicles[j] < num_of_vehicles[j + 1] && lvl[j] <= lvl [j+1]) {
                    // Swap arr[j] and arr[j + 1]
                    int temp = num_of_vehicles[j];
                    num_of_vehicles[j] = num_of_vehicles[j + 1];
                    num_of_vehicles[j + 1] = temp;

                    string temp1 = roads[j];
                    roads[j] = roads[j + 1];
                    roads[j + 1] = temp1;
                }
            }
        }
    }

    string dequeue() {
        if(front < rear) {
            string mixed = roads[front];
            string vehiclesOnRoad = to_string(num_of_vehicles[front]);
            mixed += vehiclesOnRoad;
            front++;
            return mixed;
        }
    }

    ~priorityQueue() {
        delete[] num_of_vehicles;
        delete[] roads;
    }
};

class Vehicle {
private:
    string name;
    char start_point, end_point;
    string current_location, path;
    int current_pos = 0; // Track current position in the path
    int current_weight = 0; // Track the position in the current road
    string emergency_type;

public:
    Vehicle(string a, char b, char c, string emergency) {
        name = a;
        start_point = b;
        end_point = c;
        emergency_type = emergency;
        
        current_location = string(1, start_point);  // Start from the start_point
        path = "";
    }

    void set_path(string a) {
        path = a;
        current_location += string(1, path[1]); 
    }

    char get_end_point(){
            return end_point;
    }

    string getName(){
        return name;
    }

    void update_location(int weightOfRoads[roads][roads]) {
        if(current_pos < path.length()) {
            // Get the start and end points of the current road segment
            char start = path[current_pos];
            char end = path[current_pos + 1];

            // Find the indices of the start and end points
            int start_idx = start - 'A';
            int end_idx = end - 'A';

            // Get the weight (time) for the road
            int road_weight = weightOfRoads[start_idx][end_idx];

            current_weight += 2;
            current_location = path.substr(current_pos, 2); 
            
            if(current_weight >= road_weight) {
                current_pos += 1; // Update position in path
                current_weight = 0;
            }    
        }
    }

    string get_current_location() {
        return current_location;
    }

    string get_emergency_type(){
        return emergency_type;
    }

    void display() {
        if(current_location[1] == '\0') {
            cout << name << ": Reached at " << current_location;
        }
        else {  
            cout << name << ": " << current_location;
        }
        cout << endl;    
    }
};

class Graph {
private:
    Vehicle* cars[max_vehicles];
    priorityQueue signal_duration;
    int car_index = 0;
    int weightOfRoads[roads][roads];
    char roadName[roads]; // Stores names of intersections
    int intersectionsGreenTime[roads]; // Stores the green time of intersections

    void assignNames() {
        for(int i = 0; i < roads; i++) {
            char ch = 65 + i;
            roadName[i] = ch;
        }
    }

    // Helper function to find the index of an intersection
    int findIndex(char intersection) {
        for(int i = 0; i < roads; i++) {
            if(roadName[i] == intersection) {
                return i;
            }
        }
        return -1;
    }

    void assignWeights() {
        for(int i = 0; i < roads; i++) {
            for(int j = 0; j < roads; j++) {
                weightOfRoads[i][j] = -1;
            }
        }
    }

    // Function to Reroute the traffic
    void rerouteVehicle(Vehicle* vehicle) {
        char start = vehicle->get_current_location()[0];
        char end = vehicle->get_current_location()[1];
        int start_idx = findIndex(start);
        int end_idx = findIndex(vehicle->get_end_point());

        // Perform BFS to find an alternate route
        string newPath = bfsForAlternatePath(start_idx, end_idx);
        if(!newPath.empty()) {
            vehicle->set_path(newPath);
        }
    }
    
    // Function to return the new path after Rerouting using BFS
    string bfsForAlternatePath(int start, int end) {
        Queue queue;
        bool visited[roads] = {false};

        queue.enqueue(start, string(1, roadName[start]));
        visited[start] = true;

        while(!queue.isEmpty()) {
            QueueElement temp = queue.dequeue();
            int current = temp.nodeIndex;
            string path = temp.path;

            if(current == end) {
                return path; // Return the path if the destination is reached
            }

            for(int neighbor = 0; neighbor < roads; neighbor++) {
                if(!visited[neighbor] && weightOfRoads[current][neighbor] != -1) {
                    int* numVehicles = vehicleCheck(roadName[current], roadName[neighbor]);
                    if(numVehicles[0] < 2) { // Avoid congested roads
                        visited[neighbor] = true;
                        queue.enqueue(neighbor, path + roadName[neighbor]);
                    }
                }
            }
        }
        return ""; 
    }

public:
    Graph() {
        assignNames();
        assignWeights();
        loadFromCSV("road_network.csv");
        vehicleFromCSV("vehicles.csv");
        signalsFromCSV("traffic_signals.csv");
        emergencyCSV("emergency_vehicles.csv");
        blockRoadsCSV("road_closures.csv");
    }

    void shortestPath(int start, int end, string& p) {
        int dist[roads], parent[roads];
        bool visited[roads] = {false};

        // Initialize distances and parents
        for(int i = 0; i < roads; i++) {
            dist[i] = intMax;
            parent[i] = -1;
        }

        dist[start] = 0;

        for(int count = 0; count < (roads - 1); count++) {
            int u = -1;

            // Find the vertex with the minimum distance
            for(int i = 0; i < roads; i++) {
                if(!visited[i] && (u == -1 || (dist[i] < dist[u]))) {
                    u = i;
                }
            }

            if(dist[u] == intMax) {
                break; // No more reachable nodes
            }

            visited[u] = true;

            // Update the distances of the neighbors
            for(int v = 0; v < roads; v++) {
                if(weightOfRoads[u][v] != -1 && weightOfRoads[u][v] != -2  && (dist[u] + weightOfRoads[u][v] < dist[v])) {
                    dist[v] = dist[u] + weightOfRoads[u][v];
                    parent[v] = u;
                }
            }
        }

        // Reconstruct the path
        int current = end;
        while(current != -1) {
            p = string(1, roadName[current]) + p;
            current = parent[current];
        }
    }

    int* vehicleCheck(char ch1, char ch2) {
        int num = 0, lvl = 1;
        for(int i = 0; i < car_index; i++) {
            string str = cars[i]->get_current_location();
            string emergency_check = cars[i]->getName();

            if(str[0] == ch1 && str[1] == ch2) {
                if(emergency_check[0] == 'V') {
                    num += 1;
                }
                else {
                    if(cars[i]->get_emergency_type() == "High") {
                        lvl = 3;
                    }
                    else {   
                        lvl = 2;
                    }  
                    num += 1;  
                }     
            }
        }

        int* return_arr = new int[2];
        return_arr[0]=num;
        return_arr[1]=lvl;
        return return_arr;
    }

    // Read road network from a CSV file
    void loadFromCSV(string fileName) {
        ifstream file(fileName);

        string line;
        int line1 = 0;
        while(getline(file, line)) {
            if(line1 < 1) {
                line1++;
                continue;
            }

            stringstream ss(line);
            string start, end, travelTimeStr;
            int travelTime;
            
            // Read intersection1, intersection2, and travel time
            getline(ss, start, ',');
            getline(ss, end, ',');
            getline(ss, travelTimeStr);

            travelTime = stoi(travelTimeStr); // Convert travel time to integer

            // Add road to the graph
            addRoad(start[0], end[0], travelTime);
        }
        file.close();
    }

    // Read vehicles from a CSV file
    void vehicleFromCSV(string fileName) {
        ifstream file(fileName);

        string line;
        int line1 = 0;
        while(getline(file, line)) {
            if(line1 < 1) {
                line1++;
                continue;
            }

            stringstream ss(line);
            string start, end, name;

            if(!getline(ss, name, ',') || !getline(ss, start, ',') || !getline(ss, end)) {
                cout << "Invalid line in vehicle CSV: " << line << endl;
                continue;
            }

            // Check if start and end points are valid
            int startIdx = findIndex(start[0]);
            int endIdx = findIndex(end[0]);

            if(startIdx == -1 || endIdx == -1) {
                cout << "Invalid intersection in vehicle CSV: " << start[0] << " to " << end[0] << endl;
                continue;
            }

            if(car_index < max_vehicles) {
                cars[car_index] = new Vehicle(name, start[0], end[0], "none");
                string path = "";
                shortestPath(startIdx, endIdx, path);
                cars[car_index]->set_path(path);
                car_index++;
            }
            else {
                cout << "Maximum vehicle limit reached!" << endl;
            }
        }
        file.close();
    }

    // Read initial green time of every signal from a CSV file
    void signalsFromCSV(string fileName) {
        ifstream file(fileName);

        string line;
        int line1 = 0;
        while(getline(file, line)) {
            if(line1 < 1) {
                line1++;
                continue;
            }

            stringstream ss(line);
            string intersect,time;
            int Time;
            
            // Read intersection and it's signal green time
            getline(ss, intersect, ',');
            getline(ss, time);
            

            Time = stoi(time); // Convert signal green time to integer

            int i = findIndex(intersect[0]);
            intersectionsGreenTime[i] = Time;
        }
        file.close();
    }

    // Read Emergency Vehicles from a CSV file
    void emergencyCSV(string fileName) {
        ifstream file(fileName);

        string line;
        int line1 = 0;
        while(getline(file, line)) {
            if(line1 < 1) {
                line1++;
                continue;
            }

            stringstream ss(line);
            string start, end, name, e_type;

            if(!getline(ss, name, ',') || !getline(ss, start, ',') || !getline(ss, end, ',') || !getline(ss, e_type)) {
                continue;
            }

            // Check if start and end points are valid
            int startIdx = findIndex(start[0]);
            int endIdx = findIndex(end[0]);

            if(startIdx == -1 || endIdx == -1) {
                continue;
            }

            if(car_index < max_vehicles) {
                cars[car_index] = new Vehicle(name, start[0], end[0], e_type);
                string path = "";
                shortestPath(startIdx, endIdx, path);
                cars[car_index]->set_path(path);
                car_index++;
            }
            else {
                cout << "Maximum vehicle limit reached!" << endl;
            }
        }
        file.close();
    }

    // Read Blocked Raods from a CSV file
    void blockRoadsCSV(string fileName) {
        ifstream file(fileName);

        string line;
        int line1 = 0;
        while(getline(file, line)) {
            if(line1 < 1) {
                line1++;
                continue;
            }

            stringstream ss(line);
            string start,end,status;

            if(!getline(ss, start, ',') || !getline(ss, end, ',') || !getline(ss, status)) {
                continue;
            }
            
            // Read raod intersections to block
            getline(ss, start, ',');
            getline(ss, end, ',');

            int a = findIndex(start[0]);
            int b = findIndex(end[0]);
            
            weightOfRoads[a][b] = -2;
        }
        file.close();
    }

    // Continuously monitor the number of vehicles on each road
    void congestionMonitoring() {
        for(int i = 0; i < roads; i++) {
            for(int j = 0; j < roads; j++) {
                if(weightOfRoads[i][j] == -1) {
                    continue;
                }
                else {
                    int* num = vehicleCheck(roadName[i],roadName[j]);
                    if(num[0] != 0) {
                        cout << roadName[i] << " to " << roadName[j] << " -> Vehicles: " << num[0] << endl;
                    }
                }
            }    
        }
    }

    // Function to Block a Road dynamically
    void blockRoad(char a, char b) {
        int strt = findIndex(a);
        int end = findIndex(b);
        weightOfRoads[strt][end] = -2;
        cout << "Road Blocked Successfully\n";
    }

    // Function to display Blocked Roads
    void display_blockages (){
        for(int i = 0; i < 26; i++) {
            for(int j = 0; j < 26; j++) {
                if(weightOfRoads[i][j] == -2) {
                    cout << roadName[i] << " to " << roadName[j] << " is blocked." << endl;
                }
            }
        }
    }

    // Function to setup the queue
    string queue_setting() {
        for(int i = 0; i < roads; i++) {
            for(int j = 0; j < roads; j++) {
                if(weightOfRoads[i][j] == -1) {
                    continue;
                }
                else {
                    int* num = vehicleCheck(roadName[i], roadName[j]);
                    if(num[0] != 0) {
                        string rdName = string(1, roadName[i]) + roadName[j];
                        signal_duration.enqueue(rdName, num[0], num[1]);
                    }
                }
            }    
        }

        signal_duration.arrange();
        string ss = "";
        int i = 0;
        
        while(!signal_duration.isEmpty()) {
            string str = signal_duration.dequeue();
            if(i == 0) {
                ss = str;
            }
            if(!str.empty() && (str.length() >= 3)) {
                char end = str[1];
                int traffic = stoi(str.substr(2));
                int endIndex = findIndex(end);
                    
                intersectionsGreenTime[endIndex] = 15 + (traffic * 10); 
            }
            i++;
        }
        return ss;
    }

    // Helper Function to find all paths between two intersections
    void findAllPaths1(int current, int dest, bool visited[], int path[], int& pathIndex, int pathWeight) {
        visited[current] = true;
        path[pathIndex] = current;
        pathIndex++;

        if(current == dest) {
            cout << "Path: ";
            for(int i = 0; i < pathIndex; i++) {
                cout << char(path[i] + 'A') << " ";
            }
            cout << "| Total Weight: " << pathWeight << endl;
        } 
        else {
            for(int i = 0; i < roads; i++) {
                if(weightOfRoads[current][i] != -1 && weightOfRoads[current][i] != -2 && !visited[i]) {
                    findAllPaths1(i, dest, visited, path, pathIndex, pathWeight + weightOfRoads[current][i]);
                }
            }
        }

        // Backtrack
        pathIndex--;
        visited[current] = false;
    }

    // Function to find all paths between two intersections
    void findAllPaths(char start, char end) {
        bool visited[roads] = {false};
        int path[roads];
        int pathIndex = 0;

        findAllPaths1(findIndex(start), findIndex(end), visited, path, pathIndex, 0);
    }

    // Fucntion to reroute the traffic on congested roads
    void Rerouting() {
        cout << "\nCongestions: \n";
        for(int i = 0; i < roads; i++) {
            for(int j = 0; j < roads; j++) {
                if(weightOfRoads[i][j] == -1) {
                    continue;
                }

                int* numVehicles = vehicleCheck(roadName[i], roadName[j]);

                if(numVehicles[0] > 2) {
                    cout << "Congestion detected on road " << roadName[i] << " to " << roadName[j] << endl;
                    // Reroute vehicles on this road
                    for(int k = 0; k < car_index; k++) {
                        string loc = cars[k]->get_current_location();

                        if(loc[0] == roadName[i] && loc[1] == roadName[j]) {
                            rerouteVehicle(cars[k]); // Reroute the traffic
                        }
                    }
                }
            }
        }
    }

    // Display Traffic Signal Status
    void display_signals(char rd) {
        bool signalsDisplayed = false;
        int j = findIndex(rd);

        for(int i = 0; i < roads; i++) {
            if(i == j) {
                cout << "Intersection " << roadName[i] << " Green Time: " << intersectionsGreenTime[i] << "s (Open Right Now) " << endl;
            }
            else {
                cout << "Intersection " << roadName[i] << " Green Time: " << intersectionsGreenTime[i] << "s" << endl;
            }
            signalsDisplayed = true;
        }

        if(!signalsDisplayed) {
            cout << "No active signals to display." << endl;
        }
    }

    // Add a road (directed edge) to the graph
    void addRoad(char a, char b, int w) {
        int road1 = findIndex(a);
        int road2 = findIndex(b);
        weightOfRoads[road1][road2] = w;
    }

    // Remove a road (directed edge) from the graph
    void removeRoad(char a, char b) {
        int road1 = findIndex(a);
        int road2 = findIndex(b);
        weightOfRoads[road1][road2] = -1;
    }
    
    // Display the graph structure
    void display() {
        for(int i = 0; i < roads; i++) {
            cout << roadName[i] <<" -> ";

            for(int j = 0; j < roads; j++) {
                if(weightOfRoads[i][j] == -1) {
                    continue;
                }
                else {
                    cout << "("<< roadName[j] << ", " << weightOfRoads[i][j] << ") ";
                }
            }
            cout << endl;
        }
    }

    // Update the vehicles' locations
    void update_vehicle_locations(char rd) {
        for(int i = 0; i < car_index; i++) {
            char ch = cars[i]->get_current_location()[1];
            if(ch == rd) {
                cars[i]->update_location(weightOfRoads);  // Update location based on path and road weights
            }
        }
    }

    ~Graph() {
        for(int i = 0; i < car_index; i++) {
            delete cars[i];
        }
    }
};

int main() {
    Graph map;

    int choice = 0, pressed = 1;
    int r = 0;
    int current = 0;
    int signal_time = 0;
    char rd;

    while(true) {
        if(choice == 0 && pressed == 1) {
            system("cls");
            cout << "------ Simulation Dashboard ------\n";
            cout << "1. Display City Traffic Network\n";
            cout << "2. Display Traffic Signal Status\n";
            cout << "3. Display Congestion Status\n";
            cout << "4. Display Blocked Roads\n";
            cout << "5. Handle Emergency Vehicle Routing\n";
            cout << "6. Block Road due to Accident\n";
            cout << "7. Simulate Vehicle Routing\n";
            cout << "8. Exit Simulation\n\n";
            cout << "Enter your choice: ";
        }

        if(_kbhit()) { // Check if a key is pressed
            char key = _getch(); // Get the pressed key
            pressed = 1;
        
            if(choice == 0) { // Check if the input is given on the dashboard
                choice = key - '0'; // Convert char to integer
            }
            else {
                choice = 0; // To display the dashboard again
                continue;
            }
            
            if(choice == 1) {
                system("cls");
                cout << "------ City Traffic Network ------\n";
                map.display(); // Display the city traffic network
                cout<<"\nPress any key to return...\n";
            }
            else if(choice == 2) {
                system("cls");
                cout << "------ Traffic Signal Status ------\n";
                map.display_signals(rd); // Display the Traffic Signal Status
                cout<<"\nPress any key to return...\n";
            }
            else if(choice == 3) {
                system("cls");
                cout << "------ Congestion Status ------\n";
                map.congestionMonitoring(); // Display congestion status
                map.Rerouting(); // Display congested raods and reroute the traffic
                cout<<"\nPress any key to return...\n";
            }
            else if(choice == 4) {
                system("cls");
                cout << "------ Blocked Roads ------\n";
                map.display_blockages();
                cout << "\nPress any key to return...\n";
            }
            else if(choice == 5) {
                system("cls");
                char a, b;
                cout << "Enter start intersection for emergency vehicle: ";
                cin >> a;
                cout << "Enter end intersection for emergency vehicle: ";
                cin >> b;
                cout << "Emergency Vehicle is being routed...\n";
                string path = "";
                map.shortestPath(a - 'A', b - 'A', path);
                cout << "Emergency Vehicle path: ";
                for(int i = 0; i < path.length(); i++) {
                    cout << path[i] << " ";
                }
                cout << "\n\nPress any key to return...\n";
            }
            else if(choice == 6) {
                system("cls");
                char a, b;
                cout << "Enter start point of road to block: ";
                cin >> a;
                cout << "Enter end point of road to block: ";
                cin >> b;
                map.blockRoad(a, b);
                cout << "\nPress any key to return...\n";
            }
            else if(choice == 7) {
                system("cls");
                char a, b;
                cout << "Simulating vehicle routing...\n";
                cout << "Enter starting intersection: ";
                cin >> a;
                cout << "Enter ending intersection: ";
                cin >> b;
                map.findAllPaths(a, b);
                cout << "\nPress any key to return...\n";
            }
            else if(choice == 8) {
                system("cls");
                cout << "Exiting Simulation...\n";
                break; // Exit the loop to end the simulation
            }
            else {
                choice = 0; // To display the dashboard again
                continue;
            }
        }
        else {
            pressed = 0; // Don't repeatedly display dashboard
        }
        
        r += 1000;
        if(r % 1000 == 0) { // Updates vehicle location after every second
            map.update_vehicle_locations(rd);
        }

        if(current >= signal_time) { // To check if the green time for a signal is over
            string str = map.queue_setting();
            rd = str[1];
            int traffic;

            if(str.length() == 3) {
                traffic = stoi(str.substr(2)); // Extract the traffic count as an integer
            }
            else {
                traffic = 0;
            }

            signal_time = 15 + traffic * 10;
            current = 0;
        }
        current += 1;

        Sleep(1000);
    }

    return 0;
}