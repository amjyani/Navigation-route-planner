# City Navigator

A C++ graph-based navigation application that finds routes between
cities using **BFS** and **DFS**, built with the STL (`unordered_map`,
`vector`, `queue`, `stack`, `unordered_set`).

- **BFS** (`std::queue`) → guarantees the route with the **fewest stops**.
- **DFS** (`std::stack`) → returns **a valid route**, not necessarily the shortest.

A sample 18-city U.S. road network is preloaded so the app works out
of the box.

## Project structure

```
city-navigator/
├── src/
│   └── city_navigator.cpp   # full source code
├── docs/
│   └── City_Navigator_Explanation.md   # full project write-up / viva guide
├── Makefile
├── .gitignore
└── README.md
```

## Build & run

**Using g++ directly:**
```bash
g++ -std=c++17 -O2 -o city_navigator src/city_navigator.cpp
./city_navigator
```

**Using make:**
```bash
make      # builds the city_navigator binary
make run  # builds (if needed) and runs it
make clean
```

**In VS Code:**
1. Open this folder in VS Code (`File > Open Folder...`).
2. Install the "C/C++" extension (Microsoft) if you haven't already.
3. Open a terminal in VS Code (`` Ctrl+` ``) and run `make run`, or use
   the Run/Debug button with the C++ extension configured.

## Features

- Interactive menu: view the road network, list cities, find a route
  with BFS, find a route with DFS, compare BFS vs DFS on the same
  query, or add a new road at runtime.
- Reports the route, number of stops, total distance, and nodes
  explored for each search.

## Data structures used

| Purpose | STL structure |
|---|---|
| Graph (adjacency list) | `unordered_map<string, vector<Edge>>` |
| BFS frontier | `std::queue<string>` |
| DFS frontier | `std::stack<string>` |
| Visited tracking | `unordered_set<string>` |
| Path reconstruction | `unordered_map<string, string>` (parent pointers) |

## Documentation

See [`docs/City_Navigator_Explanation.md`](docs/City_Navigator_Explanation.md)
for a full explanation of the algorithms, complexity analysis, a
BFS-vs-DFS comparison, and a viva Q&A prep guide.

## License

Add a license of your choice (e.g. MIT) if this is for public
submission alongside coursework requirements.
