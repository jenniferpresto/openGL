//
//  main.cpp
//  openGLTransformations
//
//  Created by Jennifer on 7/9/15.
//  Copyright (c) 2015 Jennifer. All rights reserved.
//


/*
 
 Note: GLFW must be installed, and dynamic library must be linked under Target's Build Phases.
 Add usr/include/lib in Header Search Paths.
 
 To use SOIL, must install, including library, and explicitly link .c files from the library under Build Phases > Compile Sources
 See stack overflow question here: http://stackoverflow.com/questions/12184499/xcode-4-0-2-soil-library-link-errors-osx-10-6-8
 
 */

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <thread>

// includes for GLM (OpenGL Math)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLSL(src) "#version 150 core\n" #src

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// shader sources
const char * vertexSrc = GLSL(
                              uniform vec2 mousePos;
                              
                              in vec2 inPos;
                              in vec2 inVel;
                              
                              out vec2 outPos;
                              out vec2 outVel;
                            
                              // generate a pseudo random direction based on particle's current position
                              // http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
                              float getRandomFloat(vec2 currentPos) {
                                  return fract(sin(dot(currentPos.xy, vec2(12.9898, 78.233))) * 43758.5453);
                                  float a = 12.9898;
                                  float b = 78.233;
                                  float c = 43758.5453;
                                  float dt = dot(currentPos.xy, vec2(a, b));
                                  float sn = mod(dt, 3.14);
                                  return fract(sin(sn) * c);
                              }
                              
                              vec2 getDir(float value)
                              {
                                  vec2 vel;
                                  value *= 3.1415926535897932384626433832795 * 2.0f;
                                  //vel.x = cos(randFloat) * 0.13;
                                  //vel.y = sin(randFloat) * 0.13;
                                  vel.x = cos(value);
                                  vel.y = sin(value);
                                  return vel;
                              }
                              
                              
                              float mapFloat(float value, float inputMin, float inputMax, float outputMin, float outputMax)
                              {
                                  const float Epsilon = 0.0000001;
                                  if (abs(inputMin - inputMax) < Epsilon) {
                                      return outputMin;
                                  }
                                  
                                  float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
                                  // clamp result
                                  if (outVal > outputMax) {
                                      outVal = outputMax;
                                  }
                                  if (outVal < outputMin) {
                                      outVal = outputMin;
                                  }
                                  return outVal;
                              }
                              
                              void main() {
                                  vec2 gravity = vec2(0.0, -0.0005);
                                  float max = 0.05;
                                  float maxSquared = max * max;
                                  
                                  outPos = inPos;
                                  outVel = inVel;
                                  outVel += gravity;
                                  
                                  // limit speed
                                  float speedSquared = dot(outVel, outVel);
                                  if (speedSquared > maxSquared) {
                                      vec2 norm = normalize(outVel);
                                      outVel *= max;
                                  }
                                  
                                  outPos += outVel;
                                  
                                  // keep in frame
                                  if (inPos.y < -1.0) {
                                      outPos.x = 0.0;
                                      outPos.y = 0.0;
                                      
                                      // reset velocity and position
                                      
                                      // use position to generate random number for velocity direction
                                      float newFloat = getRandomFloat(inPos);
                                      
                                      // use a different vector to generate a random number for velocity length
                                      vec2 altVec;
                                      if (inPos.x != 0.0 && inPos.y != 0.0) {
                                          altVec = inPos + vec2(1.0/inPos.x, 1.0/inPos.y);
                                      } else {
                                          altVec = inPos;
                                      }
                                      
                                      float newSpeed = getRandomFloat(altVec);
                                      newSpeed = mapFloat(newSpeed, 0.0, 1.0, 0.06, 0.15); // make min output number smaller; why does that happen?
                                      
                                      outVel = getDir(newFloat);
                                      outVel *= newSpeed;
                                      outPos = mousePos;
                                      //outPos += outVel;
                                  }
                                  
                                  gl_Position = vec4(outPos, 0.0, 1.0);
                              }
                              );

const char * fragmentSrc = GLSL(
                                out vec4 outColor;
                                void main() {
                                    outColor = vec4(1.0, 0.0, 0.0, 1.0);
                                }
                                );



//  separate function to compile shader
GLuint createShader(GLenum type, const GLchar* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    // debug shader
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    char buffer[512];
    std::cout << "Shader status: (0 bad, 1 good): " << status << std::endl;
    if (status == 0) {
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        std::cout << "Shader compile log: " << buffer << std::endl;
    }
    
    return shader;
};

float randomOne(glm::vec2 co)
{
    return glm::fract(sin(dot(co, glm::vec2(12.9898,78.233))) * 43758.5453);
};

float randomTwo(glm::vec2 co)
{
    float a = 12.9898f;
    float b = 78.233f;
    float c = 43758.5453f;
    float dt= dot(co, glm::vec2(a,b));
    float sn= glm::mod(dt, 3.14f);
    return glm::fract(sin(sn) * c);
}

glm::vec2 normalizeMousePos(float x, float y)
{
    glm::vec2 pos;
    float normX = x / (float)WINDOW_WIDTH;
    normX *= 2.0f;
    normX -= 1.0f;

    float normY = y / (float)WINDOW_HEIGHT;
    normY *= 2.0f;
    normY -= 1.0f;
    normY *= -1.0f;
    
    normX = glm::clamp(normX, -1.0f, 1.0f);
    normY = glm::clamp(normY, -1.0f, 1.0f);

    pos.x = normX;
    pos.y = normY;
    
    return pos;
};

float clamp(float n, float lower, float upper)
{
    return std::max(lower, std::min(n, upper));
};

//  added this later
void OnError(int errorCode, const char* msg)
{
    throw std::runtime_error(msg);
}

int main(int argc, const char * argv[]) {

    std::cout << "Hello, World!\n";
    //auto t_start = std::chrono::high_resolution_clock::now();
    
    //  getting everything set up
    glfwInit();
    
    //  added this later
    glfwSetErrorCallback(OnError);
    
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    glewInit();
    

    //  print out OpenGL version
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    //  create and bind the vao
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    //  array of values
    const int particleCount = 75000;
    GLfloat positionData[particleCount * 4]; // two slots for position, two for velocity
    
    //  generate random values
    unsigned int time_ui = (unsigned int)( time(NULL) );
    srand(time_ui);
    
    for (int i = 0; i < particleCount; i++) {
        
        //  position is completely random
        float randNum = (float)(rand()) / (float)(RAND_MAX);
        randNum *= 2.0;
        randNum -= 1.0;
        float randNum2 = (float)(rand()) / (float)(RAND_MAX);
        randNum2 *= 2.0;
        randNum2 -= 1.0;

        //  velocity is normalized vector
        float randNum3 = (float)(rand()) / (float)(RAND_MAX);
        randNum3 *= M_PI * 2.0f;
        
//        positionData[(i*4) + 0] = randNum;            // pos.x
//        positionData[(i*4) + 1] = randNum2;           // pos.y

        positionData[(i*4) + 0] = 0.0f;                 // pos.x
        positionData[(i*4) + 1] = 0.0f;                 // pos.y

        positionData[(i*4) + 2] = cos(randNum3) * 0.03; // vel.x
        positionData[(i*4) + 3] = sin(randNum3) * 0.03; // vel.y
    }
    
    std::cout << "Checking array" << std::endl;
    
//    for (int i = 0; i < particleCount; i++) {
//        std::cout << "    particle [" << i << "]" << std::endl;
//        std:: cout << "      pos.x: " << positionData[(i*4) + 0] << std::endl;
//        std:: cout << "      pos.y: " << positionData[(i*4) + 1] << std::endl;
//        std:: cout << "      vel.x: " << positionData[(i*4) + 2] << std::endl;
//        std:: cout << "      vel.y: " << positionData[(i*4) + 3] << std::endl;
//    }
    
    //  create two buffers to ping-pong back and forth with position data
    GLuint positionBufferA, positionBufferB;
    glGenBuffers(1, &positionBufferA);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferA);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positionData), positionData, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &positionBufferB);
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positionData), 0, GL_STREAM_DRAW); // don't initialize immediately
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    

    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSrc);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    
    // before linking program, specify which output attributes we want to capture into a buffer
    const GLchar * feedbackVaryings[] = {"outPos", "outVel"};
    glTransformFeedbackVaryings(program, 2, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
    
    glBindFragDataLocation(program, 0, "outColor");
    // now link and activate the program
    glLinkProgram(program);
    glUseProgram(program);
    
    GLint posAttrib = glGetAttribLocation(program, "inPos");
    GLint velAttrib = glGetAttribLocation(program, "inVel");
    GLint mousePosUniform = glGetUniformLocation(program, "mousePos");
    
    std::cout << "posAttrib: " << posAttrib << std::endl;
    std::cout << "velAttrib: " << velAttrib << std::endl;
    
    glm::vec2 testVector = glm::vec2(-0.4f, -0.5f);
    
    std::cout << "Main function: test randomness: " << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << "Vector: " << testVector.x << ", " << testVector.y << std::endl;
        float resultOne = randomOne(testVector);
        float resultTwo = randomTwo(testVector);
        std::cout << "    randOne: " << resultOne << std::endl;
        std::cout << "    randTwo: " << resultTwo << std::endl;
        testVector.x += 0.01;
    }
    
    
    while (!glfwWindowShouldClose(window)) {
        
        glfwPollEvents();
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        //glClearColor(122.f/255, 24.f/255, 47.f/255, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //  disable the rasterizer
        glEnable(GL_RASTERIZER_DISCARD);
        
        //  specify the souce buffer
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferA);
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        glEnableVertexAttribArray(velAttrib);
        glVertexAttribPointer(velAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        
        //  specify target buffer
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, positionBufferB);
        
        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, particleCount);
        glEndTransformFeedback();
        glDisableVertexAttribArray(posAttrib);
        glDisableVertexAttribArray(velAttrib);
        
        glFlush();

        std::swap(positionBufferA, positionBufferB);

        glDisable(GL_RASTERIZER_DISCARD);
        
        //  get the mouse position
        double mouseX;
        double mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
        //  normalize the mouse position
        glm::vec2 normMousePos = normalizeMousePos((float)(mouseX), (float)(mouseY));
        
        float mousePosArray[] = {normMousePos.x, normMousePos.y};
        glUniform2fv(mousePosUniform, 1, mousePosArray);
        
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferA);
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glEnableVertexAttribArray(velAttrib);
        glVertexAttribPointer(velAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glDrawArrays(GL_POINTS, 0, particleCount);
        glDisableVertexAttribArray(posAttrib);
        glDisableVertexAttribArray(velAttrib);
        
        //std::cout << "mouse position: " << "(" << mouseX << ", " << mouseY << ")" << std::endl;
        
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();

//    glDeleteFramebuffers(1, &frameBuffer);
//    glDeleteRenderbuffers(1, &rboDepthStencil);
//    glDeleteTextures(1, &texColorBuffer);
//    
//    glDeleteTextures(1, &texPuppy);
//    glDeleteTextures(1, &texKitten);
//    
    glDeleteProgram(program);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

//    glDeleteProgram(sceneShaderProgram);
//    glDeleteShader(sceneFragmentShader);
//    glDeleteShader(sceneVertexShader);
//    
    glDeleteBuffers(1, &positionBufferA);
    glDeleteBuffers(1, &positionBufferB);

    glDeleteVertexArrays(1, &vao);
    
    std::cout << "Done deleting" << std::endl;
    return 0;
}

