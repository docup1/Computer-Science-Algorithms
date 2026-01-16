#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <iostream>
#include <vector>
#include <cmath>

// --- ШЕЙДЕРЫ ---
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    out vec2 NDC_Pos;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        NDC_Pos = aPos; 
        TexCoord = aTexCoord;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;
    in vec2 NDC_Pos;
    uniform sampler2D ourTexture;
    uniform int mode; 
    void main() {
        if (mode == 0) {
            // Задача 1: NDC -> Color (0.5*xy + 0.5)
            FragColor = vec4(NDC_Pos * 0.5 + vec2(0.5), 0.0, 1.0);
        } else if (mode == 1) {
            // Задача 2: UV координаты
            FragColor = vec4(TexCoord, 0.0, 1.0);
        } else {
            // Задача 3: Текстура
            FragColor = texture(ourTexture, TexCoord);
        }
    }
)";

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Lab 2: Texture & NDC", NULL, NULL);
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

    // Данные: Позиция (x,y) и UV (u,v)
    float vertices[] = {
        -0.8f, -0.8f,  0.0f, 0.0f,
         0.8f, -0.8f,  1.0f, 0.0f,
         0.0f,  0.8f,  0.5f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- СОЗДАНИЕ ПРОЦЕДУРНОЙ ТЕКСТУРЫ (Задача 3) ---
    int width = 64, height = 64;
    std::vector<unsigned char> pixels(width * height * 3);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Рисуем шахматную доску сине-зеленого цвета
            unsigned char c = ((x / 8 + y / 8) % 2 == 0) ? 255 : 100;
            pixels[(y * width + x) * 3 + 0] = 0;   // R
            pixels[(y * width + x) * 3 + 1] = c;   // G
            pixels[(y * width + x) * 3 + 2] = 150; // B
        }
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    int currentMode = 0;
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) currentMode = 0;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) currentMode = 1;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) currentMode = 2;

        glUseProgram(prog);
        glUniform1i(glGetUniformLocation(prog, "mode"), currentMode);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}