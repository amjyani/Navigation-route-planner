/*
 * City Navigator
 * ---------------
 * A graph-based navigation application that models cities as nodes and
 * roads as weighted edges, then finds routes between cities using:
 *   - BFS (Breadth-First Search) -> guarantees the path with the FEWEST
 *     stops (hops), the way an unweighted "shortest path" is defined.
 *   - DFS (Depth-First Search)   -> finds *a* valid path (not guaranteed
 *     shortest), useful for comparison and for exploring connectivity.
 *
 * Data structures used:
 *   - unordered_map<string, vector<Edge>>  : adjacency list (the graph)
 *   - std::queue<string>                   : BFS frontier
 *   - std::stack<string> (via vector)      : DFS frontier (iterative)
 *   - unordered_map for visited/parent tracking
 *
 * Build:
 *   g++ -std=c++17 -O2 -o city_navigator city_navigator.cpp
 *
 * Run:
 *   ./city_navigator
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <sstream>

// ---------------------------------------------------------------------
// Graph representation
// ---------------------------------------------------------------------
struct Edge {
    std::string to;
    int distanceKm;   // edge weight, used only to REPORT total distance,
                       // not to influence BFS/DFS traversal order
};

class CityGraph {
public:
    // Add a city so it shows up even before any roads are added.
    void addCity(const std::string& city) {
        adj.emplace(city, std::vector<Edge>{});
    }

    // Roads are two-way by default (undirected graph).
    void addRoad(const std::string& a, const std::string& b, int km, bool twoWay = true) {
        addCity(a);
        addCity(b);
        adj[a].push_back({b, km});
        if (twoWay) adj[b].push_back({a, km});
    }

    bool hasCity(const std::string& city) const {
        return adj.find(city) != adj.end();
    }

    const std::vector<Edge>& neighbors(const std::string& city) const {
        static const std::vector<Edge> empty;
        auto it = adj.find(city);
        return it == adj.end() ? empty : it->second;
    }

    std::vector<std::string> allCities() const {
        std::vector<std::string> cities;
        cities.reserve(adj.size());
        for (auto& kv : adj) cities.push_back(kv.first);
        std::sort(cities.begin(), cities.end());
        return cities;
    }

    void printGraph() const {
        std::cout << "\n--- Road Network (" << adj.size() << " cities) ---\n";
        for (const auto& city : allCities()) {
            std::cout << city << ":\n";
            for (const auto& e : adj.at(city)) {
                std::cout << "   -> " << e.to << "  (" << e.distanceKm << " km)\n";
            }
        }
    }

private:
    std::unordered_map<std::string, std::vector<Edge>> adj;
};

// ---------------------------------------------------------------------
// Path result helper
// ---------------------------------------------------------------------
struct PathResult {
    bool found = false;
    std::vector<std::string> path;
    int totalDistanceKm = 0;
    int nodesExplored = 0;
};

// Reconstruct path from a parent map, then compute total distance by
// looking up the edge weight between consecutive cities.
static PathResult buildResult(const CityGraph& g,
                               const std::unordered_map<std::string, std::string>& parent,
                               const std::string& start,
                               const std::string& goal,
                               int nodesExplored) {
    PathResult result;
    result.nodesExplored = nodesExplored;

    if (start != goal && parent.find(goal) == parent.end()) {
        result.found = false;
        return result;
    }

    std::vector<std::string> path;
    std::string cur = goal;
    path.push_back(cur);
    while (cur != start) {
        cur = parent.at(cur);
        path.push_back(cur);
    }
    std::reverse(path.begin(), path.end());

    int total = 0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        for (const auto& e : g.neighbors(path[i])) {
            if (e.to == path[i + 1]) {
                total += e.distanceKm;
                break;
            }
        }
    }

    result.found = true;
    result.path = path;
    result.totalDistanceKm = total;
    return result;
}

// ---------------------------------------------------------------------
// BFS: shortest path by NUMBER OF STOPS (classic unweighted shortest path)
// Uses std::queue, as requested.
// ---------------------------------------------------------------------
PathResult findRouteBFS(const CityGraph& g, const std::string& start, const std::string& goal) {
    std::queue<std::string> frontier;
    std::unordered_set<std::string> visited;
    std::unordered_map<std::string, std::string> parent;
    int explored = 0;

    frontier.push(start);
    visited.insert(start);

    while (!frontier.empty()) {
        std::string current = frontier.front();
        frontier.pop();
        explored++;

        if (current == goal) break;

        for (const auto& edge : g.neighbors(current)) {
            if (visited.find(edge.to) == visited.end()) {
                visited.insert(edge.to);
                parent[edge.to] = current;
                frontier.push(edge.to);
            }
        }
    }

    return buildResult(g, parent, start, goal, explored);
}

// ---------------------------------------------------------------------
// DFS: finds A path (not necessarily shortest), iterative with a stack.
// ---------------------------------------------------------------------
PathResult findRouteDFS(const CityGraph& g, const std::string& start, const std::string& goal) {
    std::stack<std::string> frontier;
    std::unordered_set<std::string> visited;
    std::unordered_map<std::string, std::string> parent;
    int explored = 0;

    frontier.push(start);

    while (!frontier.empty()) {
        std::string current = frontier.top();
        frontier.pop();

        if (visited.count(current)) continue;
        visited.insert(current);
        explored++;

        if (current == goal) break;

        // Push neighbors in reverse so traversal order feels natural
        const auto& neighbors = g.neighbors(current);
        for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
            if (!visited.count(it->to)) {
                if (!parent.count(it->to)) parent[it->to] = current;
                frontier.push(it->to);
            }
        }
    }

    return buildResult(g, parent, start, goal, explored);
}

static void printResult(const std::string& label, const PathResult& r) {
    std::cout << "\n[" << label << "]\n";
    if (!r.found) {
        std::cout << "  No route found.\n";
        std::cout << "  Nodes explored: " << r.nodesExplored << "\n";
        return;
    }
    std::cout << "  Route: ";
    for (size_t i = 0; i < r.path.size(); ++i) {
        std::cout << r.path[i];
        if (i + 1 < r.path.size()) std::cout << " -> ";
    }
    std::cout << "\n";
    std::cout << "  Stops: " << (r.path.size() - 1)
              << " | Total distance: " << r.totalDistanceKm << " km"
              << " | Nodes explored: " << r.nodesExplored << "\n";
}

// ---------------------------------------------------------------------
// Sample data so the app is useful immediately
// ---------------------------------------------------------------------
void loadSampleNetwork(CityGraph& g) {
    g.addRoad("New York", "Boston", 306);
    g.addRoad("New York", "Philadelphia", 151);
    g.addRoad("Philadelphia", "Washington", 224);
    g.addRoad("Boston", "Albany", 273);
    g.addRoad("Albany", "Buffalo", 460);
    g.addRoad("Buffalo", "Cleveland", 306);
    g.addRoad("Cleveland", "Chicago", 552);
    g.addRoad("Washington", "Pittsburgh", 391);
    g.addRoad("Pittsburgh", "Cleveland", 214);
    g.addRoad("Chicago", "Detroit", 452);
    g.addRoad("Detroit", "Cleveland", 271);
    g.addRoad("Chicago", "St. Louis", 483);
    g.addRoad("St. Louis", "Kansas City", 402);
    g.addRoad("Kansas City", "Denver", 963);
    g.addRoad("Denver", "Salt Lake City", 837);
    g.addRoad("Salt Lake City", "Las Vegas", 685);
    g.addRoad("Las Vegas", "Los Angeles", 435);
    g.addRoad("Denver", "Los Angeles", 1610);
}

// ---------------------------------------------------------------------
// Simple interactive menu
// ---------------------------------------------------------------------
void printMenu() {
    std::cout << "\n===== City Navigator =====\n"
              << "1. Show road network\n"
              << "2. List all cities\n"
              << "3. Find route (BFS - fewest stops)\n"
              << "4. Find route (DFS - a valid path)\n"
              << "5. Compare BFS vs DFS for a route\n"
              << "6. Add a road\n"
              << "0. Exit\n"
              << "Choice: ";
}

int main() {
    CityGraph graph;
    loadSampleNetwork(graph);

    std::cout << "Welcome to City Navigator!\n"
              << "A sample U.S. road network has been preloaded.\n";

    while (true) {
        printMenu();
        std::string choiceLine;
        if (!std::getline(std::cin, choiceLine)) break;
        int choice = -1;
        try { choice = std::stoi(choiceLine); } catch (...) { choice = -1; }

        if (choice == 0) {
            std::cout << "Safe travels!\n";
            break;
        } else if (choice == 1) {
            graph.printGraph();
        } else if (choice == 2) {
            std::cout << "\nCities:\n";
            for (const auto& c : graph.allCities()) std::cout << "  - " << c << "\n";
        } else if (choice == 3 || choice == 4 || choice == 5) {
            std::string start, goal;
            std::cout << "Start city: ";
            std::getline(std::cin, start);
            std::cout << "Destination city: ";
            std::getline(std::cin, goal);

            if (!graph.hasCity(start) || !graph.hasCity(goal)) {
                std::cout << "One or both cities are not in the network.\n";
                continue;
            }

            if (choice == 3) {
                printResult("BFS - fewest stops", findRouteBFS(graph, start, goal));
            } else if (choice == 4) {
                printResult("DFS - a valid path", findRouteDFS(graph, start, goal));
            } else {
                printResult("BFS - fewest stops", findRouteBFS(graph, start, goal));
                printResult("DFS - a valid path", findRouteDFS(graph, start, goal));
            }
        } else if (choice == 6) {
            std::string a, b;
            int km;
            std::cout << "City A: ";
            std::getline(std::cin, a);
            std::cout << "City B: ";
            std::getline(std::cin, b);
            std::cout << "Distance (km): ";
            std::string kmLine;
            std::getline(std::cin, kmLine);
            try { km = std::stoi(kmLine); } catch (...) { km = 0; }
            graph.addRoad(a, b, km);
            std::cout << "Road added: " << a << " <-> " << b << " (" << km << " km)\n";
        } else {
            std::cout << "Invalid choice, try again.\n";
        }
    }

    return 0;
}
