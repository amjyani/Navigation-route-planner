# City Navigator — Project Explanation & Viva Guide

## 1. Project Overview

**City Navigator** is a console-based C++ application that models a road
network as a **graph** and finds routes between cities using two classic
graph traversal algorithms: **Breadth-First Search (BFS)** and
**Depth-First Search (DFS)**.

- **Cities** are represented as **nodes (vertices)**.
- **Roads** connecting cities are represented as **edges**, each carrying
  a **weight** (distance in kilometers).
- The graph is **undirected** (a road between A and B can be travelled
  in both directions) and **weighted** (distances are attached to edges),
  but the two search algorithms themselves are the classic *unweighted*
  versions — they decide *which* path to take based on connectivity, not
  distance. The weights are only added up afterward to report the total
  trip length.

The core idea of the project is to demonstrate the difference between
BFS and DFS when solving the same real-world problem: "get me from city
A to city B."

---

## 2. Problem Statement

Given a network of cities connected by roads of varying length, find a
route from a **start city** to a **destination city**:

1. Using **BFS**, which is guaranteed to return the path with the
   **fewest number of stops (edges)** — the graph-theory definition of
   "shortest path" in an unweighted sense.
2. Using **DFS**, which returns **some valid path** that exists, but
   gives no guarantee that it is the shortest or shortest-distance route.

Comparing the two outputs on the same start/destination pair is what
makes the project instructive.

---

## 3. Data Structures Used

| Structure | STL Type | Purpose |
|---|---|---|
| Graph (adjacency list) | `unordered_map<string, vector<Edge>>` | Maps each city to a list of `{neighbor, distance}` pairs. This is the standard way to store a sparse graph. |
| BFS frontier | `std::queue<string>` | BFS must process nodes in the order they were *discovered* (FIFO), which is exactly what a queue provides. |
| DFS frontier | `std::stack<string>` | DFS dives as deep as possible before backtracking (LIFO), which is exactly what a stack provides. |
| Visited set | `unordered_set<string>` | O(1) average-time lookup to avoid revisiting a city and to prevent infinite loops in cyclic graphs. |
| Parent map | `unordered_map<string, string>` | Records "which city did we come from?" for every city we reach, so the final path can be reconstructed by walking backwards from the destination to the start. |

### Why an adjacency list instead of an adjacency matrix?

A road network is a **sparse graph** — each city connects to only a
handful of others, not to every other city. An adjacency list uses
O(V + E) memory, while a matrix would waste O(V²) memory storing mostly
zeros. For 18 cities this barely matters, but it is the technically
correct choice and scales to large maps (e.g., a real country's road
network).

---

## 4. The `Edge` and `CityGraph` structures

```cpp
struct Edge {
    std::string to;
    int distanceKm;
};
```

Each edge just stores the neighboring city's name and the distance to
it.

`CityGraph` wraps the adjacency list and exposes three key operations:

- `addCity` — registers a city node (even with no roads yet).
- `addRoad` — adds an edge in both directions (since roads are two-way),
  automatically registering both endpoints as cities.
- `neighbors` — returns all cities directly reachable from a given
  city, which is what both BFS and DFS use to expand a node.

---

## 5. BFS — Breadth-First Search

### Idea
BFS explores the graph **level by level**. It first visits all cities
one road away from the start, then all cities two roads away, and so
on. Because it expands outward in "rings," the **first time** it
reaches the destination, it is guaranteed to have done so using the
**minimum possible number of edges**.

### Why a queue?
A queue is FIFO (first-in, first-out). When a city is discovered, it is
pushed to the back of the queue. Cities are processed from the front,
in the exact order they were discovered. This ordering is what forces
BFS to finish an entire "ring" of distance-*k* cities before moving on
to distance-*(k+1)* cities.

### Pseudocode
```
BFS(start, goal):
    queue = [start]
    visited = {start}
    parent = {}

    while queue not empty:
        current = queue.pop_front()
        if current == goal: break
        for each neighbor of current:
            if neighbor not in visited:
                visited.add(neighbor)
                parent[neighbor] = current
                queue.push_back(neighbor)

    reconstruct path by walking parent[] from goal back to start
```

### Code mapping
```cpp
std::queue<std::string> frontier;
frontier.push(start);
visited.insert(start);

while (!frontier.empty()) {
    std::string current = frontier.front();
    frontier.pop();
    ...
    for (const auto& edge : g.neighbors(current)) {
        if (!visited.count(edge.to)) {
            visited.insert(edge.to);
            parent[edge.to] = current;
            frontier.push(edge.to);
        }
    }
}
```

### Complexity
- **Time:** O(V + E) — every vertex and edge is examined at most once.
- **Space:** O(V) — for the queue, visited set, and parent map.

### Guarantee
BFS gives the **shortest path by hop count** (fewest stops) in any
unweighted or equally-weighted graph. It does **not** automatically
minimize total distance unless all edges have equal weight — a subtlety
worth mentioning in the viva (see Section 8).

---

## 6. DFS — Depth-First Search

### Idea
DFS explores as **deep as possible** along one branch before
backtracking. It commits to a neighbor, then that neighbor's neighbor,
and so on, only backing up when it hits a dead end.

### Why a stack?
A stack is LIFO (last-in, first-out). The most recently discovered city
is explored next, which is exactly the "go deep first" behavior DFS
needs. (The project uses an **iterative** DFS with `std::stack` rather
than the more common recursive version, to explicitly show the LIFO
structure requested by the assignment.)

### Pseudocode
```
DFS(start, goal):
    stack = [start]
    visited = {}
    parent = {}

    while stack not empty:
        current = stack.pop_back()
        if current in visited: continue
        visited.add(current)
        if current == goal: break
        for each neighbor of current (in reverse order):
            if neighbor not in visited:
                if neighbor not already assigned a parent:
                    parent[neighbor] = current
                stack.push_back(neighbor)

    reconstruct path by walking parent[] from goal back to start
```

### Complexity
- **Time:** O(V + E)
- **Space:** O(V)

### Guarantee
DFS guarantees only that **a path will be found if one exists**. It
gives **no guarantee of shortest path** — the route it returns depends
entirely on the order neighbors happen to be stored in the adjacency
list. This is why, in the sample run, DFS often returns a noticeably
longer route than BFS between the same two cities.

---

## 7. Path Reconstruction (shared by both algorithms)

Both BFS and DFS populate a `parent` map as they explore: `parent[X] = Y`
means "we reached city X directly from city Y." Once the destination is
found, `buildResult()`:

1. Starts at the goal city.
2. Repeatedly looks up its parent and prepends it to a list.
3. Stops when it reaches the start city.
4. Reverses the list to get start → destination order.
5. Walks the final path and sums the edge weight between each
   consecutive pair to report total distance in km.

This is the standard technique for extracting an actual path (not just
"is it reachable?") out of a BFS/DFS traversal.

---

## 8. BFS vs DFS — Key Comparison Table

| Aspect | BFS | DFS |
|---|---|---|
| Data structure | Queue (FIFO) | Stack (LIFO) |
| Exploration pattern | Level by level (breadth) | Branch by branch (depth) |
| Shortest path (fewest stops)? | **Yes, guaranteed** | No guarantee |
| Shortest path (minimum distance)? | Only if all edges have equal weight | No |
| Memory usage | Can be higher (stores a whole frontier "ring") | Can be lower (only stores current branch) |
| Typical use case | Shortest hops, social network "degrees of separation", shortest route in unweighted maps | Maze solving, topological sort, cycle detection, exhaustive search |

> **Important nuance for the viva:** because the roads in this project
> have *different* distances, BFS's "shortest path" guarantee is about
> **number of stops**, not necessarily the **lowest total kilometers**.
> Finding the true minimum-distance route on a weighted graph requires
> **Dijkstra's algorithm** (a natural "what would you add next?"
> follow-up question — see Section 10).

---

## 9. Sample Walkthrough

Given the preloaded network, a query from **Boston → Denver**:

- **BFS** returns: `Boston -> Albany -> Buffalo -> Cleveland -> Chicago -> St. Louis -> Kansas City -> Denver` (7 stops).
- **DFS** returns: `Boston -> New York -> Philadelphia -> Washington -> Pittsburgh -> Cleveland -> Chicago -> St. Louis -> Kansas City -> Denver` (9 stops).

Both are valid routes — you can indeed drive from Boston to Denver
either way — but BFS found the one with fewer stops because it
explores outward level by level, while DFS happened to commit early to
the New York branch and only reached Denver after a longer detour.

---

## 10. Likely Viva Questions & Model Answers

**Q1. Why does BFS use a queue and DFS use a stack?**
BFS must process nodes in the order discovered (FIFO) to guarantee it
finishes exploring all nodes at distance *k* before moving to distance
*k+1*. DFS needs to keep going deeper into the most recently discovered
branch (LIFO), which is what a stack naturally does.

**Q2. Does BFS give the shortest distance or the shortest number of hops?**
Shortest number of hops (edges), not necessarily shortest total
distance, unless all edges are equal weight. This project reports both
so the distinction is visible.

**Q3. What is the time complexity of BFS and DFS?**
Both are O(V + E), because in the worst case every vertex is
enqueued/pushed once and every edge is examined once.

**Q4. Why use `unordered_map`/`unordered_set` instead of `map`/`set`?**
`unordered_map`/`unordered_set` use hashing for average O(1)
insert/lookup, versus O(log n) for the tree-based `map`/`set`. Since we
only need fast membership checks and don't need cities in sorted order
during traversal, the unordered versions are faster.

**Q5. What happens if the destination is unreachable?**
The traversal finishes without ever assigning a parent to the goal
city, so `buildResult()` detects that and reports "No route found."

**Q6. Why is the graph undirected?**
Because in this model, roads can be travelled in both directions —
adding an edge always adds it symmetrically (`a → b` and `b → a`)
unless a one-way road is explicitly requested.

**Q7. How would you find the actual shortest-distance (not
shortest-hop) route?**
Use **Dijkstra's algorithm**, which replaces the plain queue with a
**priority queue** ordered by cumulative distance instead of discovery
order.

**Q8. How would you detect if the graph has disconnected components
(a city with no road to anywhere)?**
Run BFS/DFS from every unvisited node in turn and count how many
separate traversals it takes to cover all cities; each traversal
corresponds to one connected component.

**Q9. What's the space complexity?**
O(V) for the visited set, parent map, and frontier (queue or stack),
plus O(V + E) for the graph itself.

**Q10. Could this graph have a cycle, and does that cause infinite
loops?**
Yes, the sample data has cycles (e.g., Cleveland connects to multiple
cities that eventually loop back). The `visited` set is exactly what
prevents infinite loops — a city is never re-expanded once visited.

---

## 11. Limitations & Possible Extensions

- **No true weighted shortest path** — could add Dijkstra's algorithm
  or A* (with a heuristic like straight-line distance) for
  minimum-distance routing.
- **No persistence** — cities/roads reset each run; could be extended
  to load/save from a CSV or JSON file.
- **No cycle/connectivity report** — could add a feature to list
  disconnected regions of the map.
- **Console-only** — could be wrapped in a GUI or web frontend for a
  more polished demo.

---

## 12. One-Sentence Summary (for the opening of your viva)

> "This project models a road network as a weighted, undirected graph
> using an adjacency list, then compares BFS (queue-based, guarantees
> the fewest-stop route) against DFS (stack-based, finds a valid but
> not necessarily shortest route) to navigate between cities."
