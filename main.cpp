#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    Vec2 v[3];
    float color[3];
} Triangle;

Triangle triangles[100];
int triangleCount = 0;

Vec2 currentVertices[3];
int vertexCount = 0;

GLuint shaderProgram;
GLuint vao[100], vbo[100];

const char* vertex_shader_src =
    "#version 330 core\n"
    "layout(location=0) in vec2 position;\n"
    "layout(location=1) in vec3 color;\n"
    "out vec3 fragColor;\n"
    "void main() {\n"
    "  fragColor = color;\n"
    "  gl_Position = vec4(position.x / 400.0 - 1.0, -(position.y / 300.0 - 1.0), 0.0, 1.0);\n"
    "}\n";

const char* fragment_shader_src =
    "#version 330 core\n"
    "in vec3 fragColor;\n"
    "out vec4 outColor;\n"
    "void main() {\n"
    "  outColor = vec4(fragColor, 1.0);\n"
    "}\n";

void compileShaders() {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader_src, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader_src, NULL);
    glCompileShader(fs);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        currentVertices[vertexCount].x = (float)xpos;
        currentVertices[vertexCount].y = (float)ypos;
        vertexCount++;

        if (vertexCount == 3) {
            Triangle* tri = &triangles[triangleCount];

            for (int i = 0; i < 3; ++i) {
                tri->v[i] = currentVertices[i];
            }

            // Gera cor aleatória
            tri->color[0] = (float)(rand() % 100) / 100.0f;
            tri->color[1] = (float)(rand() % 100) / 100.0f;
            tri->color[2] = (float)(rand() % 100) / 100.0f;

            // Gera buffers
            glGenVertexArrays(1, &vao[triangleCount]);
            glGenBuffers(1, &vbo[triangleCount]);

            float data[15]; // 3 vértices × (2 pos + 3 cor)
            for (int i = 0; i < 3; ++i) {
                data[i * 5 + 0] = tri->v[i].x;
                data[i * 5 + 1] = tri->v[i].y;
                data[i * 5 + 2] = tri->color[0];
                data[i * 5 + 3] = tri->color[1];
                data[i * 5 + 4] = tri->color[2];
            }

            glBindVertexArray(vao[triangleCount]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[triangleCount]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            triangleCount++;
            vertexCount = 0;
        }
    }
}

int main() {
    srand((unsigned int)time(NULL));

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Clicando Triangulos", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    compileShaders();
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        for (int i = 0; i < triangleCount; ++i) {
            glBindVertexArray(vao[i]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
