#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Regular_triangulation_3.h>
#include <set>
#include <tuple>
#include <vector>
#include <iostream>
#include <algorithm>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Regular_triangulation_vertex_base_3<K> Vb;
typedef CGAL::Regular_triangulation_cell_base_3<K> Cb;
typedef CGAL::Triangulation_data_structure_3<Vb, Cb> Tds;
typedef CGAL::Regular_triangulation_3<K, Tds> Regular_triangulation;
typedef K::Point_3 Point;
typedef K::Weighted_point_3 Weighted_point;

//Triangulos
typedef std::tuple<int,int,int> Triangle;

inline Triangle make_triangle(int a, int b, int c) {
    std::vector<int> v = {a,b,c};
    std::sort(v.begin(), v.end());
    return {v[0], v[1], v[2]};
}

inline std::set<Triangle> extract_triangles(Regular_triangulation &rt, const std::vector<Weighted_point>& points) {
    std::set<Triangle> triangles;
    for (auto fit = rt.finite_facets_begin(); fit != rt.finite_facets_end(); fit++) {
        auto cell = fit->first;
        int idx = fit->second;

        std::vector<Point> facePoints;
        for (int v = 0; v < 4; v++) {
            if (v != idx) {
                facePoints.push_back(cell->vertex(v)->point().point());
            }
        }

        std::vector<int> faceIdx;
        for (auto& fp : facePoints) {
            for (int i = 0; i < (int)points.size(); i++) {
                if (fp == points[i].point()) {
                    faceIdx.push_back(i);
                    break;
                }
            }
        }

        if (faceIdx.size() == 3) {
            triangles.insert(make_triangle(faceIdx[0], faceIdx[1], faceIdx[2]));
        }
    }
    return triangles;
}

inline void print_triangles(const std::set<Triangle>& tris) {
    for (auto [a,b,c] : tris) {
        std::cout << "  (" << a << "," << b << "," << c << ")\n";
    }
}

inline void compare_triangulations(const std::set<Triangle>& t1, const std::set<Triangle>& t2, std::ostream& os = std::cout) {
    os << "Triangulos en la primera pero no en la segunda:\n";
    for (auto& tri : t1) {
        if (t2.find(tri) == t2.end()) {
            auto [a,b,c] = tri;
            os << "  (" << a << "," << b << "," << c << ")\n";
        }
    }

    os << "Triangulos en la segunda pero no en la primera:\n";
    for (auto& tri : t2) {
        if (t1.find(tri) == t1.end()) {
            auto [a,b,c] = tri;
            os << "  (" << a << "," << b << "," << c << ")\n";
        }
    }
}

//Aristas
typedef std::pair<int,int> Edge;

inline Edge make_edge(int a, int b) {
    if (a > b){ 
        std::swap(a, b);
    }
    return {a, b};
}

inline std::set<Edge> extract_edges(Regular_triangulation &rt, const std::vector<Weighted_point>& points) {
    std::set<Edge> edges;
    for (auto eit = rt.finite_edges_begin(); eit != rt.finite_edges_end(); eit++) {
        auto cell = eit->first;
        int i = eit->second;
        int j = eit->third;

        Point p1 = cell->vertex(i)->point().point();
        Point p2 = cell->vertex(j)->point().point();

        int idx1=-1, idx2=-1;
        for (int k=0; k<(int)points.size(); k++) {
            if (points[k].point() == p1) idx1 = k;
            if (points[k].point() == p2) idx2 = k;
        }

        if (idx1 >= 0 && idx2 >= 0) {
            edges.insert(make_edge(idx1, idx2));
        }
    }
    return edges;
}

inline void print_edges(const std::set<Edge>& edges) {
    for (auto [a,b] : edges) {
        std::cout << "  (" << a << "," << b << ")\n";
    }
}

inline void compare_edges(const std::set<Edge>& e1, const std::set<Edge>& e2, std::ostream& os = std::cout) {
    os << "Aristas en la primera pero no en la segunda:\n";
    for (auto& e : e1) {
        if (e2.find(e) == e2.end()) {
            os << "  (" << e.first << "," << e.second << ")\n";
        }
    }

    os << "Aristas en la segunda pero no en la primera:\n";
    for (auto& e : e2) {
        if (e1.find(e) == e1.end()) {
            os << "  (" << e.first << "," << e.second << ")\n";
        }
    }
}

//Celdas
typedef std::tuple<int,int,int,int> Cell;

inline Cell make_cell(int a, int b, int c, int d) {
    std::vector<int> v = {a,b,c,d};
    std::sort(v.begin(), v.end());
    return {v[0], v[1], v[2], v[3]};
}

inline std::set<Cell> extract_cells(Regular_triangulation &rt, const std::vector<Weighted_point>& points) {
    std::set<Cell> cells;

    for (auto cit = rt.finite_cells_begin(); cit != rt.finite_cells_end(); cit++) {
        std::vector<int> idx;
        for (int i=0; i<4; i++) {
            Point p = cit->vertex(i)->point().point();
            for (int k=0; k<(int)points.size(); k++) {
                if (points[k].point() == p) {
                    idx.push_back(k);
                    break;
                }
            }
        }
        if (idx.size() == 4) {
            cells.insert(make_cell(idx[0], idx[1], idx[2], idx[3]));
        }
    }
    return cells;
}

inline void print_cells(const std::set<Cell>& cells) {
    for (auto [a,b,c,d] : cells) {
        std::cout << "  (" << a << "," << b << "," << c << "," << d << ")\n";
    }
}

inline void compare_cells(const std::set<Cell>& c1, const std::set<Cell>& c2, std::ostream& os = std::cout) {
    os << "Celdas en la primera pero no en la segunda:\n";
    for (auto& c : c1) {
        if (c2.find(c) == c2.end()) {
            auto [a,b,c_,d] = c;
            os << "  (" << a << "," << b << "," << c_ << "," << d << ")\n";
        }
    }

    os << "Celdas en la segunda pero no en la primera:\n";
    for (auto& c : c2) {
        if (c1.find(c) == c1.end()) {
            auto [a,b,c_,d] = c;
            os << "  (" << a << "," << b << "," << c_ << "," << d << ")\n";
        }
    }
}
