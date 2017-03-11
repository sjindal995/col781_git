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

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void do_movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
bool keys[1024];
GLfloat deltaTime = 0.0f;   // Time between current frame and last frame
GLfloat lastFrame = 0.0f;   // Time of last frame
GLfloat lastX = 400, lastY = 300;
GLfloat yaw0   = -90.0f; // Yaw0 is initialized to -90.0 degrees since a yaw0 of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch0 =   0.0f;
GLfloat fov = 45.0f;
vec3 nextPos = vec3(5.0f,0.0f,0.0f);
vec3 currentPos = vec3(0.0f,0.0f,0.0f);
vec3 direction = vec3(1.0f,0.0f,0.0f);
bool unconvertedClick = false;

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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    GLfloat xz_plane[] = {
        -50.0f,  -0.5f, -50.0f,  1.0f, 1.0f, 0.2f,
         50.0f,  -0.5f, -50.0f,  1.0f, 1.0f, 0.2f,
         50.0f,  -0.5f,  50.0f,  1.0f, 1.0f, 0.2f,
         50.0f,  -0.5f,  50.0f,  1.0f, 1.0f, 0.2f,
        -50.0f,  -0.5f,  50.0f,  1.0f, 1.0f, 0.2f,
        -50.0f,  -0.5f, -50.0f,  1.0f, 1.0f, 0.2f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        // glm::vec3( 2.0f,  5.0f, -15.0f), 
        // glm::vec3(-1.5f, -2.2f, -2.5f),  
        // glm::vec3(-3.8f, -2.0f, -12.3f),  
        // glm::vec3( 2.4f, -0.4f, -3.5f),  
        // glm::vec3(-1.7f,  3.0f, -7.5f),  
        // glm::vec3( 1.3f, -2.0f, -2.5f),  
        // glm::vec3( 1.5f,  2.0f, -2.5f), 
        // glm::vec3( 1.5f,  0.2f, -1.5f), 
        // glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // // Color attribute
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    // glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    GLuint VAO2, VBO2;
    glGenVertexArrays(1,&VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(xz_plane), xz_plane, GL_STATIC_DRAW);

    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // // TexCoord attribute
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    // glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO


    // Load and create a texture 
    GLuint texture1;
    GLuint texture2;
    // ====================
    // Texture 1
    // ====================
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image("textures/container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
    // ===================
    // Texture 2
    // ===================
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps
    image = SOIL_load_image("textures/awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    bool going_down = false;

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
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


        // Bind Textures using texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);  
        
        // Activate shader
        ourShader.Use();     

        mat4 view;
        mat4 projection;
        mat4 mvp;

        
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(radians(fov), GLfloat(WIDTH) / GLfloat(HEIGHT), 0.1f, 100.0f);

        GLint mvp_loc = glGetUniformLocation(ourShader.Program, "mvp");

        if(unconvertedClick){
            unconvertedClick=false;
            vec4 temp = vec4(nextPos[0],nextPos[2],0.0f,1.0f);
            temp = inverse(projection * view) * temp;
            cout << temp[0] << " , " << temp[1] << " , " << temp[2] << " , " << temp[3] << endl;
            // nextPos[0] = temp[0];
            // nextPos[1] = temp[1];
            // nextPos[2] = temp[2];
        }

        // Create transformations
        glm::mat4 transform;
        GLfloat radius = 5;
        // GLfloat angle = (GLfloat)glfwGetTime() * radians(25.0f);
        GLfloat x_translate = 0.0f;
        GLfloat y_translate = 0.0f;
        GLfloat z_translate = 0.0f;

        x_translate = currentPos[0] + deltaTime*direction[0];
        y_translate = abs(sin(glfwGetTime()));
        z_translate = currentPos[2] + deltaTime*direction[2];;

    
        if(going_down && (y_translate - currentPos[1] > 0)){
            direction = normalize(nextPos - currentPos);
            x_translate = currentPos[0] + deltaTime*direction[0];
            y_translate = abs(sin(glfwGetTime()));
            z_translate = currentPos[2] + deltaTime*direction[2];;
        }

        if(y_translate - currentPos[1] > 0){
            going_down = false;
        }
        else{
            going_down = true;
        }
        currentPos = vec3(x_translate, y_translate, z_translate);

        transform = glm::translate(transform, glm::vec3(x_translate, y_translate, z_translate));
        // transform = glm::rotate(transform, (GLfloat)glfwGetTime() * radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // // Get matrix's uniform location and set matrix
        // GLint transformLoc = glGetUniformLocation(ourShader.Program, "transform");
        // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        for(GLuint i = 0; i < 1; i++)
        {
            mat4 model;
            model = glm::translate(model, cubePositions[i]);
            model *= transform;
            mvp = projection*view*model;
            vec4 pos = projection * view * vec4(0.5f,0.5f,0.5f,1.0f);
            int winX = (int)((( pos[0] + 1 ) / 2.0) *
                                   width );
            int winY = (int)((( 1 - pos[1] ) / 2.0) *
                                   height );
            cout << winX << " , " << winY << endl;
            glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);


        mat4 mvp1 = projection*view;
        // Draw container
        glBindVertexArray(VAO2);

        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp1));
        glDrawArrays(GL_TRIANGLES, 0, 36);
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
    GLfloat cameraSpeed = 0.05f;
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
  GLfloat cameraSpeed = 5.0f*deltaTime;
  if(keys[GLFW_KEY_W])
    cameraPos += cameraSpeed * cameraFront;
  if(keys[GLFW_KEY_S])
    cameraPos -= cameraSpeed * cameraFront;
  if(keys[GLFW_KEY_A])
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if(keys[GLFW_KEY_D])
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        nextPos[0] = lastX;
        nextPos[2] = lastY;
        unconvertedClick = true;
    }
}