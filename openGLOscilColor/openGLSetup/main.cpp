//
//  main.cpp
//  openGLSetup
//
//  Created by Jennifer on 5/18/15.
//  Copyright (c) 2015 Jennifer. All rights reserved.
//


/*
 
 Note: GLFW must be installed, and dynamic library must be linked under Target's Build Phases.
 Add usr/include/lib in Header Search Paths.
 
 */

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
//#include <thread>
#include <math.h>

#define GLSL(src) "#version 150 core\n" #src

// one-third of TWO_PI
#define PI_INCREMENT 2.094395102393195

// shader sources
const char * vertexSource = GLSL(
                                 in vec2 position;
                                 in vec3 color;
                                 //in float color;
                                 in float identifier;
                                 
                                 uniform float timeTop;
                                 uniform float timeLeft;
                                 uniform float timeRight;
                                 
                                 out vec3 Color; // need same name as in frag shader
                                 //out float Color;
                                 
                                 void main() {
                                     Color = color;
                                     
                                     int vertexID;
                                     vertexID = int(floor(identifier));
                                     
                                     switch (vertexID) {
                                         case 1:
                                             Color.r *= timeTop;
                                             break;
                                             
                                         case 2:
                                             Color.g *= timeRight;
                                             break;
                                             
                                         case 3:
                                             Color.b *= timeLeft;
                                             break;
                                             
                                         default:
                                             break;
                                     }
                                     
                                     gl_Position = vec4(position.x, position.y, 0.0, 1.0);
                                 }
);

const char * fragmentSource = GLSL(
                                   uniform vec3 triangleColor;
                                   in vec3 Color; // need same name as in vertex shader
                                   //in float Color;
                                   
                                   out vec4 outColor;
                                   void main() {
                                       //outColor = vec4(1.0, 0.4, 0.3, 1.0);
                                       //outColor = vec4(triangleColor, 1.0);
                                       //outColor = vec4(1.0 - Color, 1.0);
                                       outColor = vec4(Color, 1.0);
                                   }
);


int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    GLFWwindow* window = glfwCreateWindow(1024, 768, "OpenGL", nullptr, nullptr); // Windowed
    
    glfwMakeContextCurrent(window);
    
    //  initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    
    // create vertex array object
    // saving links between attributes and vbo
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    //  create vertex buffer object; copy data into it
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // one triangle: position, color, identifier
    float vertices[] = {
        0.0f, 0.5f,     1.0f, 0.0f, 0.0f,   1.1f,    // Vertex 1: red
        0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   2.1f,   // Vertex 2: green
        -0.5f, -0.5f,   0.5f, 0.0f, 1.0f,   3.1f     // Vertex 3: blue
    };
    
    //// two triangles make a rectangle
    //float vertices[] = {
    //    -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,     // Vertex 1: red      top left
    //    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,    // Vertex 2: green    top right
    //    0.5f, -0.5f, 0.0f, 0.0f, 1.0f,     // Vertex 3: blue    bottom right
    //    
    //    0.5f, -0.5f, 0.2f, 0.3f, 0.4f,      //                  bottom right
    //    -0.5f, -0.5f, 0.4f, 0.2f, 0.8f,     //                  bottom left
    //    -0.5f, 0.5f, 0.5f, 0.6f, 0.1f       //                  top left
    //};
    
    // use a rectangle with an element buffer
    //float vertices[] = {
    //    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
    //    0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
    //    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
    //    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
    //};

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copies to graphics card
    
    // create element buffer
    GLuint ebo;
    glGenBuffers(1, &ebo);
    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    
    // create and compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    // and the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    
    //  debugging stuff
    GLint vertStatus, fragStatus;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertStatus);
    std::cout << "vertex shader status (0: bad, 1: good): " << vertStatus << std::endl;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragStatus);
    std::cout << "fragment shader status (0: bad, 1: good): " << fragStatus << std::endl;
    char buffer[512];
    if (vertStatus == 0) {
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        std::cout << "compile log buffer (vertex shader) is: " << buffer << std::endl;
    }
    if (fragStatus == 0) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
        std::cout << "compile log buffer (fragment shader) is: " << buffer << std::endl;
    }

    // combining shaders into a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //since only one output in frag shader, not actually necessary
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    // linking shader
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    
    // specify layout of vertex data
    // positions
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    //glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0); // input, num values in input, type, whether normalized, stride (bytes between), offset (bytes before)
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    
    // colors
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2*sizeof(float)));
    
    // vertex identifiers
    GLint idAttrib = glGetAttribLocation(shaderProgram, "identifier");
    glEnableVertexAttribArray(idAttrib);
    glVertexAttribPointer(idAttrib, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5*sizeof(float)));
    
    // get location of fragment shader uniform
    //GLint uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    
    // get location of vertex shader uniform
    GLint uniTime1 = glGetUniformLocation(shaderProgram, "timeTop");
    GLint uniTime2 = glGetUniformLocation(shaderProgram, "timeRight");
    GLint uniTime3 = glGetUniformLocation(shaderProgram, "timeLeft");
    
    auto t_start = std::chrono::high_resolution_clock::now();
    
    //glUniform3f (uniColor, 1.0f, 0.0f, 0.0f);
    
    
    while (!glfwWindowShouldClose(window)) {
        
        auto t_now = std::chrono::high_resolution_clock::now();
        float time_passed = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        //std::cout << "Time is : " << time << std::endl;
        
        //float r = (sin(time_passed * 3.0f) + 1.0f) / 2.0f;
        //float g = (sin(time_passed * 4.0f) + 1.0f) / 2.0f;
        //float b = (sin(time_passed * 5.0f) + 1.0f) / 2.0f;
        
        //float totalColorValue = r + b + g;
        
        /*
        if (totalColorValue > 1.98) {
            std::cout << "almost all white: " << totalColorValue << ", time: " << time << std::endl;
        } else if (totalColorValue < 0.1 ) {
            std::cout << "almost all black: " << totalColorValue << ", time: " << time << std::endl;
        }
         */

        float topTime = (sin(time_passed * 3.0f) + 1.0f) / 2.0f;
        float rightTime = (sin(time_passed * 4.0f) + 1.0f) / 2.0f;
        float leftTime = (sin(time_passed * 5.0f) + 1.0f) / 2.0f;
        
        //glUniform3f(uniColor, r, g, b);
        glUniform1f(uniTime1, topTime);
        glUniform1f(uniTime2, rightTime);
        glUniform1f(uniTime3, leftTime);
        
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glDrawArrays(GL_TRIANGLES, 0, 3); // type of primitive, how many vertices to skip, number of vertices to process
        //glDrawArrays(GL_TRIANGLES, 0, 6); // type of primitive, how many vertices to skip, number of vertices to process (this draws 2 triangles)
        //glDisableVertexAttribArray(0);
        
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // type of primitive, number indices to draw, type of element data, offset

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    
    
    glfwTerminate();
    
    return 0;
}
