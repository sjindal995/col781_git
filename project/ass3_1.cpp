#include <bits/stdc++.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GL/glfw3.h>

// Other Libs
#include <GL/SOIL.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "loadShader.cpp"

using namespace std;
using namespace glm;

mat4 projection;
mat4 view;
GLint mvp_loc ;


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void do_movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
bool keys[1024];
GLfloat deltaTime = 0.0f;   // Time between current frame and last frame
GLfloat lastFrame = 0.0f;   // Time of last frame
GLfloat lastX = 400, lastY = 300;
GLfloat yaw0   = -90.0f; // Yaw0 is initialized to -90.0 degrees since a yaw0 of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch0 =   0.0f;
GLfloat fov = 45.0f;
int numFaces = 6;

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Build and compile our shader program
    Shader ourShader("shaders/vertexshader.glsl", "shaders/fragmentshader.glsl");


    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[3*6*numFaces];
    GLfloat rad60 = glm::radians(60.0f);
    GLfloat theta = 0.0f;
    GLfloat delta = 360.0f/numFaces;
    for(int i=0;i<numFaces;i++){
        vertices[18*i] = 1.0f*cos(glm::radians(theta));
        vertices[18*i+1] = -0.5f;
        vertices[18*i+2] = -1.0f*sin(glm::radians(theta));

        vertices[18*i+3] = 1.0f*cos(glm::radians(theta));
        vertices[18*i+4] = 0.5f;
        vertices[18*i+5] = -1.0f*sin(glm::radians(theta));

        vertices[18*i+6] = 1.0f*cos(glm::radians(theta+delta));
        vertices[18*i+7] = 0.5f;
        vertices[18*i+8] = -1.0f*sin(glm::radians(theta+delta));

        vertices[18*i+9] = 1.0f*cos(glm::radians(theta+delta));
        vertices[18*i+10] = 0.5f;
        vertices[18*i+11] = -1.0f*sin(glm::radians(theta+delta));

        vertices[18*i+12] = 1.0f*cos(glm::radians(theta+delta));
        vertices[18*i+13] = -0.5f;
        vertices[18*i+14] = -1.0f*sin(glm::radians(theta+delta));

        vertices[18*i+15] = 1.0f*cos(glm::radians(theta));
        vertices[18*i+16] = -0.5f;
        vertices[18*i+17] = -1.0f*sin(glm::radians(theta));

        theta += delta;
    }

    for(int i=0;i<6*numFaces;i++){
        cout << vertices[3*i] << "," << vertices[3*i+1] << "," << vertices[3*i+2] << endl;
    }


	// GLfloat vertices[] = {
	//    1.0f, -1.0f, 0.0f,
 //       -1.0f, -1.0f, 0.0f,
	//    -1.0f,  1.0f, 0.0f,

	//    -1.0f,  1.0f, 0.0f,	   
	// 	1.0f, -1.0f, 0.0f,
	// 	1.0f,  1.0f, 0.0f,
	// };

    // GLfloat vertices1[] = {
    //     1.0,-0.5,0.0,
    //     1.0,0.5,0.0,
    //     0.5,0.5,-0.866025,
    //     0.5,0.5,-0.866025,
    //     0.5,-0.5,-0.866025,
    //     1.0,-0.5,0.0
    // };

    // GLfloat vertices2[18];
    // for(int i=0;i<6;i++){
    //     vertices2[(3*i)+0] = vertices[(3*i) + 0];
    //     vertices2[(3*i)+1] = vertices[(3*i) + 1];
    //     vertices2[(3*i)+2] = vertices[(3*i) + 2];
    // }
    
    
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // // // Color attribute
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    // glEnableVertexAttribArray(1);
    // TexCoord attribute
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    // glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    
    // Game loop
    float k=0.001;
    while (!glfwWindowShouldClose(window))
    {
        k=k+ 0.005f;
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        do_movement();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Activate shader
        ourShader.Use();     

        // mat4 view;
        // mat4 projection;
        mat4 mvp;

        
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(radians(fov), GLfloat(WIDTH) / GLfloat(HEIGHT), 0.1f, 100.0f);

        mvp_loc = glGetUniformLocation(ourShader.Program, "mvp");

        // Create transformations
        // transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        // transform = glm::rotate(transform, (GLfloat)glfwGetTime() * radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // Get matrix's uniform location and set matrix
        glBindVertexArray(VAO);
    	glm::mat4 transform;
    	mvp = projection*view*transform;
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
    	glDrawArrays(GL_TRIANGLES, 0, 6*numFaces);
        glBindVertexArray(0);
        
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    GLfloat cameraSpeed = 0.1f;
    if(key == GLFW_KEY_W)
        cameraPos += cameraSpeed * cameraFront;
    if(key == GLFW_KEY_S)
        cameraPos -= cameraSpeed * cameraFront;
    if(key == GLFW_KEY_A)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(key == GLFW_KEY_D)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(key == GLFW_KEY_UP)
        cameraPos += cameraSpeed*cameraUp;
    if(key == GLFW_KEY_DOWN)
        cameraPos -= cameraSpeed*cameraUp;
    if(key == GLFW_KEY_LEFT)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(key == GLFW_KEY_RIGHT)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(action == GLFW_PRESS)
      keys[key] = true;
    else if(action == GLFW_RELEASE)
      keys[key] = false;
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
     if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    GLfloat xoffset = GLfloat(xpos - lastX);
    GLfloat yoffset = GLfloat(lastY - ypos);
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw0 += GLfloat(xoffset);
    pitch0 += GLfloat(yoffset);

    if(pitch0 > 89.0f)
        pitch0 = 89.0f;
    if(pitch0 < -89.0f)
        pitch0 = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw0)) * cos(glm::radians(pitch0));
    front.y = sin(glm::radians(pitch0));
    front.z = sin(glm::radians(yaw0)) * cos(glm::radians(pitch0));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  if(fov >= 1.0f && fov <= 45.0f)
    fov -= yoffset;
  if(fov <= 1.0f)
    fov = 1.0f;
  if(fov >= 45.0f)
    fov = 45.0f;
}

void do_movement()
{
  // Camera controls
  GLfloat cameraSpeed = 10.0f*deltaTime;
  if(keys[GLFW_KEY_W])
    cameraPos += cameraSpeed * cameraFront;
  if(keys[GLFW_KEY_S])
    cameraPos -= cameraSpeed * cameraFront;
  if(keys[GLFW_KEY_A])
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if(keys[GLFW_KEY_D])
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
