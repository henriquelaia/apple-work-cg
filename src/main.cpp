#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <shader.hpp>
#include <objloader.hpp>

// Variáveis Globais
GLFWwindow* window;

GLuint lightingShaderID;
GLuint lampShaderID;

GLuint MatrixID;
GLuint ViewID;
GLuint ModelID;
GLuint LightColorID;
GLuint ViewPosID;
GLuint ObjectColorID;
GLuint IsFlashlightID;
GLuint LightPosID;
GLuint LightDirID;
GLuint CutOffID;
GLuint OuterCutOffID;

GLuint appleVAO, appleVBO;
GLuint lightVAO, lightVBO, lightEBO;

std::vector<glm::vec3> appleVertices;
std::vector<glm::vec2> appleUVs;
std::vector<glm::vec3> appleNormals;
std::vector<float> sphereVertices;
std::vector<unsigned int> sphereIndices;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float cameraYaw   = -90.0f;
float cameraPitch =  0.0f;
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;
float fov   =  45.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
bool flashlightOn = false;
bool fKeyPressed = false;
bool autoRotate = false;
bool rKeyPressed = false;
bool spaceKeyPressed = false;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void generateSphere(float radius, int sectorCount, int stackCount, std::vector<float>& vertices, std::vector<unsigned int>& indices);

/*
  Ponto de entrada da aplicação.
  Responsável por inicializar o GLFW e GLEW, configurar a janela e o contexto OpenGL,
  carregar os modelos 3D (Maçã) e shaders, e executar o ciclo principal de renderização (Game Loop).
  Também gere a limpeza de recursos ao fechar a aplicação.
 */
int main()
{
    if (!glfwInit()) {
        fprintf(stderr, "Falha ao inicializar GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    srand(static_cast <unsigned> (time(0)));

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Apple CG Assignment", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    lightingShaderID = LoadShaders("shaders/2.1.basic_lighting.vs", "shaders/2.1.basic_lighting.fs");
    lampShaderID = LoadShaders("shaders/2.1.lamp.vs", "shaders/2.1.lamp.fs");

    bool res = loadOBJ("src/Apple.obj", appleVertices, appleUVs, appleNormals);
    if(!res) {
        std::cout << "Failed to load Apple.obj" << std::endl;
    }

    glGenVertexArrays(1, &appleVAO);
    glGenBuffers(1, &appleVBO);

    glBindVertexArray(appleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, appleVBO);
    
    std::vector<float> appleData;
    for(size_t i=0; i<appleVertices.size(); i++){
        appleData.push_back(appleVertices[i].x);
        appleData.push_back(appleVertices[i].y);
        appleData.push_back(appleVertices[i].z);
        
        if(i < appleNormals.size()) {
            appleData.push_back(appleNormals[i].x);
            appleData.push_back(appleNormals[i].y);
            appleData.push_back(appleNormals[i].z);
        } else {
            appleData.push_back(0.0f); appleData.push_back(0.0f); appleData.push_back(1.0f);
        }
    }

    glBufferData(GL_ARRAY_BUFFER, appleData.size() * sizeof(float), appleData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    generateSphere(0.2f, 36, 18, sphereVertices, sphereIndices);

    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glGenBuffers(1, &lightEBO);

    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(lightingShaderID);

        glUniform3fv(glGetUniformLocation(lightingShaderID, "lightColor"), 1, &lightColor[0]);
        glUniform3fv(glGetUniformLocation(lightingShaderID, "viewPos"), 1, &cameraPos[0]);
        glUniform3f(glGetUniformLocation(lightingShaderID, "objectColor"), 1.0f, 0.0f, 0.0f);
        glUniform1i(glGetUniformLocation(lightingShaderID, "isFlashlight"), flashlightOn);

        if (flashlightOn) {
            glUniform3fv(glGetUniformLocation(lightingShaderID, "lightPos"), 1, &cameraPos[0]);
            glUniform3fv(glGetUniformLocation(lightingShaderID, "lightDir"), 1, &cameraFront[0]);
            glUniform1f(glGetUniformLocation(lightingShaderID, "cutOff"), glm::cos(glm::radians(12.5f)));
            glUniform1f(glGetUniformLocation(lightingShaderID, "outerCutOff"), glm::cos(glm::radians(17.5f)));
        } else {
            glUniform3fv(glGetUniformLocation(lightingShaderID, "lightPos"), 1, &lightPos[0]);
            glUniform3f(glGetUniformLocation(lightingShaderID, "lightDir"), 0.0f, 0.0f, 0.0f);
        }

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        
        glUniformMatrix4fv(glGetUniformLocation(lightingShaderID, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightingShaderID, "view"), 1, GL_FALSE, &view[0][0]);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(5.0f)); 
        
        if (autoRotate) {
             model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        
        glUniformMatrix4fv(glGetUniformLocation(lightingShaderID, "model"), 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(appleVAO);
        glDrawArrays(GL_TRIANGLES, 0, appleVertices.size());


        if (!flashlightOn) {
            glUseProgram(lampShaderID);
            
            glUniform3fv(glGetUniformLocation(lampShaderID, "lightColor"), 1, &lightColor[0]);
            glUniformMatrix4fv(glGetUniformLocation(lampShaderID, "projection"), 1, GL_FALSE, &projection[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(lampShaderID, "view"), 1, GL_FALSE, &view[0][0]);
            
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            glUniformMatrix4fv(glGetUniformLocation(lampShaderID, "model"), 1, GL_FALSE, &model[0][0]);

            glBindVertexArray(lightVAO);
            glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &appleVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &appleVBO);
    glDeleteBuffers(1, &lightVBO);
    glDeleteBuffers(1, &lightEBO);
    glDeleteProgram(lightingShaderID);
    glDeleteProgram(lampShaderID);

    glfwTerminate();
    return 0;
}


/*
  Processa o input do teclado para controlar o movimento da câmara,
  alternar modos (lanterna, rotação automática) e controlar a posição/cor da luz.
  Fecha a aplicação se a tecla ESC for pressionada.
 */
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fKeyPressed) {
        flashlightOn = !flashlightOn;
        fKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
        fKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rKeyPressed) {
        autoRotate = !autoRotate;
        rKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
        rKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spaceKeyPressed) {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        lightColor = glm::vec3(r, g, b);
        spaceKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        spaceKeyPressed = false;
    }

    if (!flashlightOn) {
        float lightSpeed = 2.5f * deltaTime;
        
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            float x = lightPos.x;
            float z = lightPos.z;
            lightPos.x = x * cos(lightSpeed) - z * sin(lightSpeed);
            lightPos.z = x * sin(lightSpeed) + z * cos(lightSpeed);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            float x = lightPos.x;
            float z = lightPos.z;
            lightPos.x = x * cos(-lightSpeed) - z * sin(-lightSpeed);
            lightPos.z = x * sin(-lightSpeed) + z * cos(-lightSpeed);
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            lightPos *= 0.99f; // Closer
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            lightPos *= 1.01f; // Further
        }
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) lightColor = glm::vec3(1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) lightColor = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) lightColor = glm::vec3(0.0f, 0.0f, 1.0f);
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) lightColor = glm::vec3(1.0f, 1.0f, 0.0f);

}

/*

  Callback executado quando a janela é redimensionada.
  Ajusta o viewport do OpenGL para corresponder às novas dimensões da janela.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/*
Callback executado quando o rato é movido.
 Calcula o deslocamento do rato e atualiza a orientação da câmara (yaw e pitch).
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cameraYaw += xoffset;
    cameraPitch += yoffset;

    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    front.y = sin(glm::radians(cameraPitch));
    front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraFront = glm::normalize(front);
}

/*

 Callback executado quando a roda do rato é usada.
 Ajusta o zoom da câmara (FOV).
 */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

/*

 Gera a geometria de uma esfera (vértices e índices) para ser usada como visualização da fonte de luz.
 Calcula as coordenadas 3D, normais e coordenadas de textura com base no raio e na resolução (setores e stacks).
 Os dados são armazenados nos vetores passados por referência.
 */
void generateSphere(float radius, int sectorCount, int stackCount, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float x, y, z, xy;                              
    float nx, ny, nz, lengthInv = 1.0f / radius;   

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}
