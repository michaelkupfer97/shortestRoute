# Shortest Route Finder

## Overview
This project is a multi-threaded client-server application that calculates the shortest route between two points based on a database of connected nodes. The application supports multiple concurrent users and utilizes caching to optimize performance by storing the last 10 queries.

## Key Features
- **Shortest Path Calculation**: Implements the **Breadth-First Search (BFS)** algorithm to determine the shortest path between two nodes in a graph.
- **Multi-User Support**: Handles multiple simultaneous user requests efficiently using a multi-threaded server.
- **Caching**: Maintains a cache of the last 10 queries to improve response time for frequently requested routes.
- **Database Integration**: Dynamically retrieves node connection data from a database.

## Technologies and Tools
- **Programming Language**: Java
- **Libraries**: 
  - **Java Collections Framework** for graph representation.
  - **ExecutorService** for managing multi-threading.
- **Data Handling**: SQLite or a similar database for storing the graph structure.
- **Design Patterns**: Implemented caching mechanisms for optimal performance.

## How It Works
1. **Database**: Contains data on which nodes are connected.
2. **User Input**: Clients input two nodes (start and end points).
3. **Shortest Path Calculation**: The server processes the request using BFS and retrieves the shortest path.
4. **Caching**: If the query exists in the cache, the result is returned immediately. Otherwise, it's calculated and added to the cache.
5. **Concurrency**: Multiple users can query the server simultaneously without impacting performance.

## Setup and Usage
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/michaelkupfer97/shortestRoute
   cd shortestRoute/finalProject

Run the Server: Compile and start the server to handle incoming client requests.

bash
javac Server.java
java Server

Run the Client: Connect to the server and query the shortest path.

bash
javac Client.java
java Client

Database: Ensure the database (graph.db) is configured with node connection data.

## Example Query
Input: Start Node: A, End Node: F
Output: Shortest Path: A → C → F

## Files
Server.java: Manages multi-threaded client requests and performs the BFS algorithm.
Client.java: Provides a user interface for sending queries to the server.
Cache.java: Implements the caching mechanism for efficient query handling.

## Future Improvements
Extend caching functionality to handle more advanced caching strategies like LRU.
Add support for weighted graphs to calculate the shortest path based on weights.
Improve the database schema for larger datasets.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
This project is licensed under the MIT License.

## Contact
For any questions, feel free to contact me at michaelkulhs@gmail.com.
