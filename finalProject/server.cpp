#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> 
#include "std_lib_facilities.h"
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <pthread.h>
//HOW TO RUN: g++ server.cpp 
// ./a.out <db name> <port>

map<int, pthread_mutex_t> socket_map;
pthread_mutex_t sock_mutex = PTHREAD_MUTEX_INITIALIZER;
map<int, vector<int>> vertices; // map with vertices and their adjecencies

void* get_request(void* arg);

// In the BFS method we store vertices with "to" and "from" info to insert in the visited map.
struct Vertex {
    int from;
    int to;
    Vertex(int f, int t) : from(f), to(t) {}
    Vertex() : from(-1), to(-1) {}
};

// Return the shortestPath from the provided visited map.
vector<int> getPath(map<int, int> visited, int start, int target) {
    int temp = target;
    vector<int> shortestPath;
    while (temp != start &&  visited.find(temp) != visited.end()) {
        shortestPath.push_back(temp);
        temp = visited[temp];
    }
    if (temp == start)  {
        shortestPath.push_back(start);
    } 
    else {
        shortestPath.clear();
    } 
    return shortestPath;
}

// BFS algorithm to search the shortest path in a graph.
// IN this BFS we use map to check the vertex we visited, then with getPath method,
// we can construct our path iterating through visited map.
vector<int> BFS(map<int, vector<int>> Graph, int start, int target) {
        vector<int> path;
        
        if (Graph.find(start) == Graph.end() || Graph.find(target) == Graph.end()) {
            return path;
        }

        queue<Vertex> q;
        map<int, int> visited;
        Vertex start_point = Vertex(start, start);
        q.push(start_point);
        visited[start] = start;
        while (!q.empty()) {
            Vertex temp = q.front();
            q.pop();
            int current = temp.to;
            auto iterator = Graph.find(current);
            if (iterator != Graph.end()) {
                for (int n : iterator->second) {
                    Vertex newVert;
                    if (visited.find(n) == visited.end()) { // n isn't in visited
                        newVert = Vertex(current, n);
                        q.push(newVert);
                    }
                }
            }
            while ((visited.find(q.front().to) != visited.end()) && !q.empty()) { // next in q is visited already
                q.pop();
            }
            if (!q.empty()) {
                visited[q.front().to] = q.front().from;
            }
        }
        return getPath(visited,start,target);
    }

// Cache class to check and store last 10 clients request
struct CacheServer {
    vector<int> request;
    vector<int> response;
};

// Cache to store the last 10 requests and responses
list<CacheServer> requestCache;
pthread_mutex_t cacheMutex = PTHREAD_MUTEX_INITIALIZER;

// Function to add an entry to the cache.
void addToCache(const vector<int>& request, const vector<int>& response) {
    pthread_mutex_lock(&cacheMutex);
    if (requestCache.size() >= 10) {
        requestCache.pop_front(); // Remove oldest entry if cache is full
    }
    requestCache.push_back({request, response});
    pthread_mutex_unlock(&cacheMutex);
}

// Function to retrieve a response from the cache
bool getFromCache(const vector<int>& request, vector<int>& response) {
    pthread_mutex_lock(&cacheMutex);
    for (const auto& entry : requestCache) {
        if (entry.request == request) {
            response = entry.response;
            pthread_mutex_unlock(&cacheMutex);
            return true;
        }
    }
    pthread_mutex_unlock(&cacheMutex);
    return false;
}

int main(int argc, char **argv) {

    ifstream data_base(argv[1]);
    string line;

    while (getline(data_base, line)) {
        istringstream iss(line);
        int l, r;
        iss >> l >> r;
        vertices[l].push_back(r);
        vertices[r].push_back(l);
    }

    // Open a TCP/IP socket.
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));

    bind(sockfd, (sockaddr*)&addr, sizeof(addr)); 
    listen(sockfd, 10); 

    for(;;) {
        int connfd = accept(sockfd, NULL, NULL);
        int* client_sock = new int;
        *client_sock = connfd;
        pthread_mutex_lock(&sock_mutex);
        if (socket_map.find(connfd) == socket_map.end()) {
            pthread_mutex_t new_sock_mutex = PTHREAD_MUTEX_INITIALIZER;
            socket_map.insert(pair<int, pthread_mutex_t> (connfd, new_sock_mutex));
        }
        pthread_mutex_unlock(&sock_mutex);

        pthread_t com_thread;
        pthread_create(&com_thread, NULL, get_request, client_sock);
    }
}

void* get_request(void *arg) {
    int connfd = *(int*) arg;
    char buffer[256];
    int byte_recv = recv(connfd, buffer, sizeof(buffer), 0); 
    string buffer_str(buffer); // Convert buffer to string
    size_t space_pos = buffer_str.find(' '); // Fint position of first space
    string v1_str=buffer_str.substr(0,space_pos); // Make first string 
    string v2_str=buffer_str.substr(space_pos+1); // Make second string
    int v1 = stoi(v1_str); // Make first int
    int v2 = stoi(v2_str); // Make second int
    
    // Check if the request is aleardy asked in last 10 times(saved in cache).
    vector<int> request = {v1, v2};
    vector<int> retrievedResponse;
    bool found = getFromCache(request, retrievedResponse);
    
    // If we got true in found it - means we need to write the retreved response to client
    if(found){
        ostringstream oss;
        for (auto it = retrievedResponse.rbegin(); it != retrievedResponse.rend(); ++it) 
            oss << *it << " "; 
        
        string response1 = oss.str();
        const char* response = response1.c_str();
        send(connfd, response, strlen(response), 0);
    } 
    else { // Start dealing with BFS
        vector<int> shortestPath = BFS(vertices, v1, v2);
        if (shortestPath.size() > 1) { // if path exist
            addToCache({v1,v2}, shortestPath);

            ostringstream oss;
            for (auto it = shortestPath.rbegin(); it != shortestPath.rend(); ++it) {
                oss << *it << " ";
            }
            
            string response1 = oss.str();
            const char* response = response1.c_str();
            send(connfd, response, strlen(response), 0);
        } 
        else { // Meaning we got empty path from bfs due to one or more of the vertices not existing in the graph.
            string errorResponse = "Path doesn't exist!";
            const char* errorResponse_cstr = errorResponse.c_str();
            size_t errorResponse_len = strlen(errorResponse_cstr);
            ssize_t bytes_sent = send(connfd, errorResponse_cstr, errorResponse_len, 0);
        }
    }
    return NULL;
}