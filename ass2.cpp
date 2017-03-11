#define GLEW_STATIC
 
// third-party libraries
#include <windows.h>
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <bits/stdc++.h>
#include "loadShader.cpp"
#include "loadTexture.cpp"
#include "controls.cpp"

GLuint gVAO = 0;
GLuint gVBO = 0;
GLuint colorbuffer;
GLuint uvbuffer;
GLuint programId;
GLFWwindow* window = NULL;
GLFWmonitor* monitor = NULL;
 
using namespace glm;
using namespace std;

const glm::vec2 SCREEN_SIZE(800, 600);

static void LoadCubeTexture(){
		glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);

    // Two UV coordinatesfor each vertex. They were created with Blender. You'll learn shortly how to do this yourself.
    static const GLfloat g_uv_buffer_data[] = {
        0.000059f, 1.0f-0.000004f,
        0.000103f, 1.0f-0.336048f,
        0.335973f, 1.0f-0.335903f,
        1.000023f, 1.0f-0.000013f,
        0.667979f, 1.0f-0.335851f,
        0.999958f, 1.0f-0.336064f,
        0.667979f, 1.0f-0.335851f,
        0.336024f, 1.0f-0.671877f,
        0.667969f, 1.0f-0.671889f,
        1.000023f, 1.0f-0.000013f,
        0.668104f, 1.0f-0.000013f,
        0.667979f, 1.0f-0.335851f,
        0.000059f, 1.0f-0.000004f,
        0.335973f, 1.0f-0.335903f,
        0.336098f, 1.0f-0.000071f,
        0.667979f, 1.0f-0.335851f,
        0.335973f, 1.0f-0.335903f,
        0.336024f, 1.0f-0.671877f,
        1.000004f, 1.0f-0.671847f,
        0.999958f, 1.0f-0.336064f,
        0.667979f, 1.0f-0.335851f,
        0.668104f, 1.0f-0.000013f,
        0.335973f, 1.0f-0.335903f,
        0.667979f, 1.0f-0.335851f,
        0.335973f, 1.0f-0.335903f,
        0.668104f, 1.0f-0.000013f,
        0.336098f, 1.0f-0.000071f,
        0.000103f, 1.0f-0.336048f,
        0.000004f, 1.0f-0.671870f,
        0.336024f, 1.0f-0.671877f,
        0.000103f, 1.0f-0.336048f,
        0.336024f, 1.0f-0.671877f,
        0.335973f, 1.0f-0.335903f,
        0.667969f, 1.0f-0.671889f,
        1.000004f, 1.0f-0.671847f,
        0.667979f, 1.0f-0.335851f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
}

static void LoadCubeColor(){
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    // One color for each vertex. They were generated randomly.
    static const GLfloat g_color_buffer_data[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    // 2nd attribute buffer : colors
    // glEnableVertexAttribArray(1);
    // glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    // glVertexAttribPointer(
    //     1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    //     3,                                // size
    //     GL_FLOAT,                         // type
    //     GL_FALSE,                         // normalized?
    //     0,                                // stride
    //     (void*)0                          // array buffer offset
    // );
}


static void LoadCube() {
 
    // make and bind the VAO
   glGenVertexArrays(1, &gVAO);
   glBindVertexArray(gVAO);
   // make and bind the VBO
   glGenBuffers(1, &gVBO);
   glBindBuffer(GL_ARRAY_BUFFER, gVBO);
   // Put the three triangle verticies into the VBO
 
   // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    static const GLfloat vertexData[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    // // connect the xyz to the "vert" attribute of the vertex shader
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // // unbind the VBO and VAO
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
 
}

 
static void LoadTriangle() {
 
    // make and bind the VAO
   glGenVertexArrays(1, &gVAO);
   glBindVertexArray(gVAO);
   // make and bind the VBO
   glGenBuffers(1, &gVBO);
   glBindBuffer(GL_ARRAY_BUFFER, gVBO);
   // Put the three triangle verticies into the VBO
 
    GLfloat vertexData[] = {
 
        //  X     Y     Z
 
         0.0f, 0.8f, 0.0f,
 
        -0.8f,-0.8f, 0.0f,
 
         0.8f,-0.8f, 0.0f,
 
    };
 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    // connect the xyz to the "vert" attribute of the vertex shader
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // // unbind the VBO and VAO
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
 
}
 
// draws a single frame
 
static void RenderCube(mat4 transform) {
 
    // clear everything
    // glClearColor(0, 0, 0, 1); // black
 
    // glClear(GL_COLOR_BUFFER_BIT);
  // bind the VAO (the triangle)
    
    // 1rst attribute buffer : vertices
      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, gVBO);
      glVertexAttribPointer(
         0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
         3,                  // size
         GL_FLOAT,           // type
         GL_FALSE,           // normalized?
         0,                  // stride
         (void*)0            // array buffer offset
      );

      // glEnableVertexAttribArray(1);
      // glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
      // glVertexAttribPointer(
      //   1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
      //   3,                                // size
      //   GL_FLOAT,                         // type
      //   GL_FALSE,                         // normalized?
      //   0,                                // stride
      //   (void*)0                          // array buffer offset
      // );
      
      // 2nd attribute buffer : UVs
      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
      glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
      );

      // Draw the triangle !
      glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glBindVertexArray(gVAO);
   // draw the VAO
    glDrawArrays(GL_TRIANGLES, 0, 12*3);
   // unbind the VAO
    glBindVertexArray(0);
    // swap the display buffers (displays what was just drawn)
   // glfwSwapBuffers(window);
      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);

}
 
 
// draws a single frame
 
static void RenderTriangle() {
 
    // clear everything
    glClearColor(0, 0, 0, 1); // black
 
    glClear(GL_COLOR_BUFFER_BIT);
  // bind the VAO (the triangle)
    
    // 1rst attribute buffer : vertices
      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, gVBO);
      glVertexAttribPointer(
         0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
         3,                  // size
         GL_FLOAT,           // type
         GL_FALSE,           // normalized?
         0,                  // stride
         (void*)0            // array buffer offset
      );
      // Draw the triangle !
      // glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
      // glDisableVertexAttribArray(0);

    glBindVertexArray(gVAO);
   // draw the VAO
    glDrawArrays(GL_TRIANGLES, 0, 3);
   // unbind the VAO
    glBindVertexArray(0);
    // swap the display buffers (displays what was just drawn)
   glfwSwapBuffers(window);
 
}
 
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        computeMatricesFromInputs();
}
 
// the program starts here
 
void AppMain() {
 
    if(!glfwInit())
    throw std::runtime_error("glfwInit failed");
    // open a window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    window = glfwCreateWindow((int)SCREEN_SIZE.x, (int)SCREEN_SIZE.y, "test window",NULL, NULL);
    if(!window)
        throw std::runtime_error("glfwOpenWindow failed. Can your hardware handle OpenGL 4.2?");
 
 
   // GLFW settings
    glfwMakeContextCurrent(window);
    // initialise GLEW
   glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
   if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");
   // print out some info about the graphics drivers
 
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
 
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
 
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
 
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    // make sure OpenGL version 4.2 API is available
 
    if(!GLEW_VERSION_4_2)
 
        throw std::runtime_error("OpenGL 4.2 API is not available.");
 
    // create buffer and fill it with the points of the triangle
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // LoadTriangle();
    LoadCube();
    // LoadCubeColor();
    LoadCubeTexture();
    // run while the window is open
    // Create and compile our GLSL program from the shaders

    GLuint Texture = loadBMP_custom("textures/uvtemplate.bmp");
    // GLuint Texture = loadDDS("textures/uvtemplate.DDS");


    GLuint programID = LoadShaders( "shaders/vertexshader.glsl", "shaders/fragmentshader.glsl" );

    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    // Get a handle for our "MVP" uniform
    // Only during the initialisation
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
      
    int width,height;
    glfwGetWindowSize(window,&width,&height);

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
      
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
      
    // Camera matrix
    glm::mat4 View , mvp;
    while(glfwGetWindowAttrib(window,GLFW_FOCUSED)){

        View= glm::lookAt(
            // glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
            glm::vec3(0,0,4), // Camera is at (4,3,3), in World Space
            glm::vec3(0,0,0), // and looks at the origin
            glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
            );
        // Model matrix : an identity matrix (model will be at the origin)
        // glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 Model = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Use our shader
        glUseProgram(programID);

        // Send our transformation to the currently bound shader, in the "MVP" uniform
        // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(TextureID, 0);

        // draw one frame
        // RenderTriangle();
        RenderCube(mat4(1.0f));

        glm::mat4 Model2 = translate(scale(mat4(1.0f),vec3(0.25f,0.25f,0.25f)), vec3(-0.75f,1.25f,0.0f));
        Model2 = glm::rotate(Model2, 45.0f, glm::vec3(0.0, 0.0, 1.0));
//        mat4 tr = glm::translate();
//        Model2 = tr*Model2;
        mvp = Projection*View*Model2;
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Use our shader
        glUseProgram(programID);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(TextureID, 0);


        // process pending events
        // glfwPollEvents();
        RenderCube(mat4(1.0f));
        glfwSwapBuffers(window);

          

        // RenderCube(mat4(1.0f));
    }
   // clean up and exit
   glfwTerminate();
 
}
int main(int argc, char *argv[]) {
 
    try {
 
        AppMain();
 
    } catch (const std::exception& e){
 
        std::cerr << "ERROR: " << e.what() << std::endl;
 
        return EXIT_FAILURE;
 
    }
    return EXIT_SUCCESS;
 
}
