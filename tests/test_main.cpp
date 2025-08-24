#include "triangulation_utils.h"
#include <iostream>

int main() {
    // Puntos base
    std::vector<Weighted_point> base_points = {
        {Point(0,0,0), 0},
        {Point(1,0,0), 0},
        {Point(0,1,0), 0},
        {Point(0,0,1), 0},
        {Point(1,1,1), 0}
    };

    // Triangulación 1
    Regular_triangulation rt1;
    rt1.insert(base_points.begin(), base_points.end());
    auto tris1  = extract_triangles(rt1, base_points);
    auto edges1 = extract_edges(rt1, base_points);
    auto cells1 = extract_cells(rt1, base_points);

    std::cout << "=== Triangulación con pesos = 0 ===\n";
    std::cout << "Triángulos:\n"; print_triangles(tris1);
    std::cout << "Aristas:\n"; print_edges(edges1);
    std::cout << "Celdas:\n"; print_cells(cells1);

    // Triangulación 2 con peso grande en último punto
    std::vector<Weighted_point> mod_points = base_points;
    mod_points[4] = Weighted_point(Point(1,1,1), 100);

    Regular_triangulation rt2;
    rt2.insert(mod_points.begin(), mod_points.end());
    auto tris2  = extract_triangles(rt2, mod_points);
    auto edges2 = extract_edges(rt2, mod_points);
    auto cells2 = extract_cells(rt2, mod_points);

    std::cout << "\n=== Triangulación con peso grande en punto 4 ===\n";
    std::cout << "Triángulos:\n"; print_triangles(tris2);
    std::cout << "Aristas:\n"; print_edges(edges2);
    std::cout << "Celdas:\n"; print_cells(cells2);

    // Comparaciones
    std::cout << "\n=== Comparación de triángulos ===\n";
    compare_triangulations(tris1, tris2);

    std::cout << "\n=== Comparación de aristas ===\n";
    compare_edges(edges1, edges2);

    std::cout << "\n=== Comparación de celdas ===\n";
    compare_cells(cells1, cells2);

    return 0;
}
