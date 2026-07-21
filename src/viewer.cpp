#include "triangulation_utils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <tuple>

float angleX = 0.5f, angleY = 0.5f, zoom = 3.0f; //ajustes de cámara

//Flags de renderización
bool showPoints = true;
bool showEdges = true;
bool showFaces = false;
bool showCells = false;

//Datos de geometria
std::vector<Point> vertices;
std::vector<Edge> edges;
std::set<Triangle> triangles;
std::set<Cell> cells;

// Archivo cargado
std::string currentFile = "models/sphere.off";
static char offFilename[128] = "sphere.off";

// Peso global
float globalWeight = 0.0;

// Cargar OFF solo como puntos
std::vector<Weighted_point> load_points_from_off(const std::string& filename, float default_weight = 0.0f) {
    std::ifstream input(filename);
    std::vector<Weighted_point> pts;
    if (!input) {
        std::cerr << "No se pudo abrir el archivo: " << filename << "\n";
        return pts;
    }

    std::string header;
    while (input >> header) {
        if (header == "OFF") break;
        if (header[0] == '#') { 
            std::string dummy;
            std::getline(input, dummy);
            continue;
        }
    }

    if (header != "OFF") {
        std::cerr << "Formato no valido: " << filename << "\n";
        return pts;
    }

    int nv, nf, ne;
    input >> nv >> nf >> ne;

    for (int i=0; i<nv; i++) {
        double x,y,z;
        input >> x >> y >> z;
        pts.push_back(Weighted_point(Point(x,y,z), default_weight));
    }

    return pts;
}

// reconstruir triangulación con puntos y pesos 
void rebuild_triangulation(const std::vector<Weighted_point>& pts) {
    Regular_triangulation rt;
    rt.insert(pts.begin(), pts.end());

    auto edge_set = extract_edges(rt, pts);
    auto tri_set  = extract_triangles(rt, pts);
    auto cell_set = extract_cells(rt, pts);

    vertices.clear();
    edges.clear();
    triangles.clear();
    cells.clear();

    for (auto &wp : pts) vertices.push_back(wp.point());
    for (auto &e : edge_set) edges.push_back(e);
    for (auto &t : tri_set) triangles.insert(t);
    for (auto &c : cell_set) cells.insert(c);

    std::cout << "Archivo cargado: " << currentFile
              << " (" << vertices.size() << " vértices, "
              << edges.size() << " aristas, "
              << triangles.size() << " triángulos, "
              << cells.size() << " celdas)\n";
}

// dibujando escena
void drawScene(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    double fovy = 45.0;
    double near = 0.1, far = 100.0;
    double top = tan(fovy * M_PI / 360.0) * near;
    double bottom = -top;
    double right = top * aspect;
    double left = -right;
    glFrustum(left, right, bottom, top, near, far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Cámara orbital
    glTranslatef(0, 0, -zoom);
    glRotatef(angleX * 57.3f, 1, 0, 0);
    glRotatef(angleY * 57.3f, 0, 1, 0);

    // Dibujar aristas
    if (showEdges) {
        glColor3f(0,1,0);
        glBegin(GL_LINES);
        for (auto &e : edges) {
            auto p1 = vertices[e.first];
            auto p2 = vertices[e.second];
            glVertex3f(p1.x(), p1.y(), p1.z());
            glVertex3f(p2.x(), p2.y(), p2.z());
        }
        glEnd();
    }

    // Dibujar puntos
    if (showPoints) {
        glPointSize(6.0f);
        glColor3f(1,0,0);
        glBegin(GL_POINTS);
        for (auto &p : vertices)
            glVertex3f(p.x(), p.y(), p.z());
        glEnd();
    }

    // Dibujar caras triangulares
    if (showFaces) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0,1,0.3f); 
        glBegin(GL_TRIANGLES);
        for (auto &t : triangles) {
            auto p1 = vertices[std::get<0>(t)];
            auto p2 = vertices[std::get<1>(t)];
            auto p3 = vertices[std::get<2>(t)];
            glVertex3f(p1.x(), p1.y(), p1.z());
            glVertex3f(p2.x(), p2.y(), p2.z());
            glVertex3f(p3.x(), p3.y(), p3.z());
        }
        glEnd();
    }

    // Dibujar celdas
    if (showCells) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1,1,0,0.15f); 
        for (auto &c : cells) {
            auto p0 = vertices[std::get<0>(c)];
            auto p1 = vertices[std::get<1>(c)];
            auto p2 = vertices[std::get<2>(c)];
            auto p3 = vertices[std::get<3>(c)];
            glBegin(GL_TRIANGLES);
            glVertex3f(p0.x(), p0.y(), p0.z()); glVertex3f(p1.x(), p1.y(), p1.z()); glVertex3f(p2.x(), p2.y(), p2.z());
            glVertex3f(p0.x(), p0.y(), p0.z()); glVertex3f(p1.x(), p1.y(), p1.z()); glVertex3f(p3.x(), p3.y(), p3.z());
            glVertex3f(p0.x(), p0.y(), p0.z()); glVertex3f(p2.x(), p2.y(), p2.z()); glVertex3f(p3.x(), p3.y(), p3.z());
            glVertex3f(p1.x(), p1.y(), p1.z()); glVertex3f(p2.x(), p2.y(), p2.z()); glVertex3f(p3.x(), p3.y(), p3.z());
            glEnd();
        }
    }
}

int main() {
    // Cargar OFF inicial
    auto pts = load_points_from_off(currentFile, globalWeight);
    rebuild_triangulation(pts);

    // Inicializar GLFW/GL
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangulation Viewer", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Inicializar ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawScene(w, h);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Controles");
        ImGui::SliderFloat("Rot X", &angleX, -3.14f, 3.14f);
        ImGui::SliderFloat("Rot Y", &angleY, -3.14f, 3.14f);
        ImGui::SliderFloat("Zoom", &zoom, 0.5f, 10.0f);
        ImGui::Checkbox("Mostrar puntos", &showPoints);
        ImGui::Checkbox("Mostrar aristas", &showEdges);
        ImGui::Checkbox("Mostrar caras (triángulos)", &showFaces);
        ImGui::Checkbox("Mostrar volumen (celdas)", &showCells);

        ImGui::Separator();
        ImGui::InputText("Archivo OFF", offFilename, IM_ARRAYSIZE(offFilename));
        if (ImGui::Button("Cargar OFF")) {
            std::string path = std::string("models/") + std::string(offFilename);
            auto pts = load_points_from_off(path, globalWeight);

            if (!pts.empty()) {
                Regular_triangulation rt;
                rt.insert(pts.begin(), pts.end());

                // limpiar estructuras
                vertices.clear();
                edges.clear();
                triangles.clear();
                cells.clear();

                for (auto& wp : pts) vertices.push_back(wp.point());
                auto edge_set = extract_edges(rt, pts);
                auto tri_set  = extract_triangles(rt, pts);
                auto cell_set = extract_cells(rt, pts);

                edges.assign(edge_set.begin(), edge_set.end());
                triangles = tri_set;
                cells = cell_set;

                std::cout << "Archivo cargado: " << path
                        << " (" << vertices.size() << " vértices, "
                        << edges.size() << " aristas, "
                        << triangles.size() << " triángulos, "
                        << cells.size() << " celdas)\n";
            }
        }

        ImGui::Text("Vertices: %zu", vertices.size());
        ImGui::Text("Aristas: %zu", edges.size());
        ImGui::Text("Caras: %zu", triangles.size());
        ImGui::Text("Celdas: %zu", cells.size());

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
