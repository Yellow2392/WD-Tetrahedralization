# Conforming Weighted Delaunay Triangulation

Experimental implementation in **C++** based on:

> **Conforming Weighted Delaunay Triangulations**
> Marc Alexa, 2020.

The project aims to study and progressively implement the approach proposed in the paper for constructing **conformal weighted Delaunay tetrahedralizations** from a triangular input surface.

The project uses **CGAL** for calculating the weighted Delaunay triangulation and **OpenGL** for the interactive visualization of the generated geometric structure.

## Description

Alexa's work proposes using a **weighted Delaunay triangulation** to construct a *tetrahedralization* that incorporates the triangles of an input surface as part of its structure.

The main idea is to find a set of weights associated with the vertices of the surface:

$$
X = {x_1, x_2, ..., x_n}
$$

along with their weights:

$$
W = {w_1, w_2, ..., w_n}
$$

so that the resulting weighted Delaunay triangulation contains the desired geometric elements of the input surface.

The method proposed in the paper formulates the necessary conditions as a **linear incremental feasibility problem**, and subsequently uses quadratic optimization to select the weights and auxiliary variables.

In simplified terms, the process can be viewed as follows:

```text
Triangular entrance surface
            │
            ▼
       X Positions
            │
            ▼
   Initialize weights w = 0
            │
            ▼
  Build Weighted Delaunay
      Triangulation
            │
            ▼
  Are there any triangles 
 missing from the surface?
       │         │
      Yes        No
       │         │
       ▼         ▼
      Find      Finish
    violated
  restriction
       │
       ▼
Add restriction
       │
       ▼
Solve feasibility
    problem
       │
       ▼
 Update weights
       │
       └───────────────┐
                       │
                       ▼
                     Repeat
```

The ultimate goal is to obtain a **conforming weighted Delaunay tetrahedralization** that respects the input surface without inserting new vertices (*Steiner points*).

## Current Status

The project is currently in an early implementation stage.

The infrastructure implemented so far includes:

* Loading vertices from `.OFF` files.
* Constructing weighted points (`Weighted_point`) using CGAL.
* Constructing a `Regular_triangulation_3`.
* Extracting finite elements from the triangulation:
* Vertices.
* Edges.
* Triangles.
* Tetrahedral cells.


* Interactively visualizing the resulting triangulation.
* Comparing geometric structures between different triangulations.

The current implementation allows experimenting with the structure of a **weighted/regular Delaunay triangulation**, but **it does not yet fully implement the algorithm described in the paper**.

In particular, the core components of the method are pending:

* Defining the surface triangles as constraints.
* Detecting surface triangles that do not belong to the current triangulation.
* Computing *witness points*.
* Constructing the linear inequalities associated with each constraint.
* Incrementally managing the set of active constraints.
* Solving the linear feasibility problem.
* Optimizing weights through the proposed quadratic problem.
* Iteratively updating weights.
* Verifying the Gabriel property.
* Handling numerical tolerances and *slack variables*.

## Current Implementation

### 1. Model Loading

Input models use the `.OFF` format.

Currently, the file is primarily used to extract vertex positions:

```text
models/
├── sphere.off
├── bunny.off
└── ...

```

The current implementation does not directly use the faces defined in the `.OFF` file to build the triangulation. Vertices are used as input points for CGAL.

> **Note:** To reproduce the algorithm from the paper, it will also be necessary to use the connectivity of the input surface's triangular faces, as these triangles represent the set of constraints to be enforced.

### 2. Weighted Points

Each vertex is represented using a CGAL `Weighted_point`:

```cpp
typedef K::Weighted_point_3 Weighted_point;

```

Currently, points are initialized using a global weight:

```cpp
float globalWeight = 0.0;

```

Therefore, in the current state, all vertices have the exact same weight.

This is equivalent to starting the process from:

$$w = 0$$

which corresponds to the initial state used by the algorithm in the paper.

The next step in the implementation will be allowing individual vertex weights to be modified by the optimization process.

### 3. Weighted Delaunay Triangulation

The triangulation is constructed via:

```cpp
Regular_triangulation rt;
rt.insert(pts.begin(), pts.end());

```

CGAL's `Regular_triangulation_3` structure represents the weighted Delaunay triangulation corresponding to the provided points and weights.

From this structure, the following elements are extracted:

```text
Weighted Delaunay Triangulation
           │
           ├── Vertices
           ├── Edges
           ├── Triangular faces
           └── Tetrahedral cells

```

These structures are stored using indices corresponding to the original vertices.

## Triangulation Representation

Geometric elements are represented using vertex indices.

### Edges

```cpp
typedef std::pair<int, int> Edge;

```

Example:

```text
(2, 5)

```

Represents an edge between vertices 2 and 5.

### Triangles

```cpp
typedef std::tuple<int, int, int> Triangle;

```

Example:

```text
(1, 4, 7)

```

Represents a triangle formed by vertices 1, 4, and 7.

### Cells

```cpp
typedef std::tuple<int, int, int, int> Cell;

```

Example:

```text
(0, 2, 5, 8)

```

Represents a tetrahedral cell formed by vertices 0, 2, 5, and 8.

Indices for each element are sorted before storage to prevent duplicates.

## Visualization

The project includes an OpenGL-based 3D viewer.

The interface allows controlling:

* Camera rotation.
* Zoom.
* Vertex visualization.
* Edge visualization.
* Triangular face visualization.
* Tetrahedral cell visualization.
* Loading new `.OFF` files.

The visualization allows inspecting the structure generated by CGAL and serves as a supportive tool for development and debugging.

## Current Architecture

The project is conceptually divided into two main components:

### `triangulation_utils.h`

Contains functions related to extracting and comparing the triangulation:

```text
extract_edges()
extract_triangles()
extract_cells()

print_edges()
print_triangles()
print_cells()

compare_edges()
compare_triangulations()
compare_cells()

```

These functions convert CGAL's internal structure into an index-based representation referencing the original vertices.

### `Viewer.cpp`

Handles:

* Loading `.OFF` files.
* Creating `Weighted_point` instances.
* Constructing the triangulation with CGAL.
* Extracting edges, triangles, and cells.
* Rendering geometry using OpenGL.
* Managing the UI using ImGui.

## Technologies

* **C++**
* **CGAL** — computational geometry and weighted Delaunay triangulation.
* **OpenGL** — 3D visualization.
* **GLFW** — window creation and event handling.
* **GLEW** — OpenGL extension loading.
* **Dear ImGui** — graphical user interface.

## Relation to the Paper

The implementation references the approach presented in:

> **Conforming Weighted Delaunay Triangulations**, Marc Alexa, 2020.

The algorithm proposed in the paper can be conceptually summarized in the following steps:

1. Start with weights $w = 0$.
2. Construct the weighted Delaunay triangulation.
3. Identify surface triangles that are missing from the triangulation.
4. For each missing triangle, compute a constraint preventing interference from other vertices.
5. Add constraints to the active set.
6. Solve the feasibility problem to obtain new weights.
7. Reconstruct the weighted Delaunay triangulation.
8. Repeat until all required triangles are present or the problem is infeasible.

The current implementation mainly covers the infrastructure corresponding to the **construction and exploration of the weighted Delaunay triangulation**, which serves as the necessary foundation to later implement the complete incremental algorithm.

## Next Steps

The planned roadmap for the project is:

### Stage 1 — Geometric Infrastructure

* [x] Load `.OFF` vertices.
* [x] Construct `Weighted_point`.
* [x] Construct `Regular_triangulation_3`.
* [x] Extract edges.
* [x] Extract triangles.
* [x] Extract tetrahedral cells.
* [x] Visualize the triangulation.

### Stage 2 — Input Surface

* [ ] Load triangular faces from the `.OFF` file.
* [ ] Represent the set of constrained triangles $S$.
* [ ] Compare input triangles with triangles present in the weighted triangulation.

### Stage 3 — Incremental Algorithm

* [ ] Detect missing triangles.
* [ ] Compute *witness points*.
* [ ] Identify vertices interfering with each triangle.
* [ ] Generate linear constraints $(m,s)$.
* [ ] Maintain the active constraint set.
* [ ] Iteratively solve the feasibility problem.

### Stage 4 — Optimization

* [ ] Implement the quadratic objective function.
* [ ] Optimize weights and heights.
* [ ] Incorporate non-negativity constraints for weights.
* [ ] Implement the Gabriel condition.

### Stage 5 — Numerical Robustness

* [ ] Handle floating-point tolerances.
* [ ] Implement *slack variables*.
* [ ] Evaluate degenerate cases.
* [ ] Validate results against test cases from the paper.

### Stage 6 — Evaluation

* [ ] Test different `.OFF` models.
* [ ] Compare iteration counts.
* [ ] Analyze active constraint counts.
* [ ] Compare initial and final triangulations.
* [ ] Evaluate the quality of generated tetrahedralizations.

## Quick Start

In order to run this project:

```bash
cd build
make -j4
./viewer
```