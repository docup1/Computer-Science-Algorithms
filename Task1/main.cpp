#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h> // Стандартный заголовок для macOS
#include <vector>
#include <cmath>
#include <iostream>

// Шейдеры
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main() { gl_Position = vec4(aPos, 0.0, 1.0); }";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() { FragColor = vec4(1.0, 0.84, 0.0, 1.0); }"; // Золотой цвет

// Функция генерации для Triangle List
std::vector<float> generateStarList(int rays, float outerR, float innerR) {
    std::vector<float> v;
    float angleStep = M_PI / rays;
    for (int i = 0; i < 2 * rays; ++i) {
        // Треугольник: Центр (0,0) -> Текущая точка -> Следующая точка
        v.push_back(0.0f); v.push_back(0.0f); // Центр
        
        float r1 = (i % 2 == 0) ? outerR : innerR;
        float a1 = i * angleStep - M_PI / 2;
        v.push_back(cos(a1) * r1); v.push_back(sin(a1) * r1);

        float r2 = ((i + 1) % 2 == 0) ? outerR : innerR;
        float a2 = (i + 1) * angleStep - M_PI / 2;
        v.push_back(cos(a2) * r2); v.push_back(sin(a2) * r2);
    }
    return v;
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Star: Triangle List", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // Компиляция шейдеров
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);

    // Данные (6 лучей)
    int rays = 6; 
    std::vector<float> vertices = generateStarList(rays, 0.7f, 0.3f);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.15f, 0.1f, 0.2f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glBindVertexArray(VAO);
        
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}