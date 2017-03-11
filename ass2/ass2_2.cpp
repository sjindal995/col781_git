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

class object
{
    public:
        mat4 worldTrans;
        vec3 scaleR;
        vec3 translateR;
        vec3 rAxisR;
        GLfloat rAngleR;
        int objid;  //0 for body, 1 for head, 2 for upperlimb front, 3 for forelimb front, 4 -> upperlimb back, 5-> forel
        vector<object*> children;       
        object(int);
        //~object();
        void draw(float);
        mat4 getTransform(float);
    
};

object::object(int id){
    worldTrans = mat4(1.0f);
    scaleR = vec3(1.0f,1.0f,1.0f);
    translateR = vec3(0.0f, 0.0f, 0.0f);
    rAxisR = vec3(1.0,1.0,1.0f);
    rAngleR = 0.0f;
    objid = id;
}

mat4 object::getTransform(float k){
    if( objid==0){
        mat4 tr(1.0f);
        tr = glm::translate(tr, vec3(-k/10,2.0f*abs(sin(glm::radians(k*50))), 0.0f));
        // tr = glm::rotate(tr, k, vec3(0.0f, 1.0f, 0.0f));
		return tr;
    }
    else if(objid==2){
    
        mat4 tr(1.0f);
        tr = glm::translate(tr, vec3(0.0f, 0.0f, -0.25f)  );
        // float angle = 0.523599f+k;
        // if(angle > 0.523599f*2)
        tr = glm::rotate(tr, 0.523599f*cos(glm::radians(2*k*50)), vec3(1.0f,0.0f, 0.0f ));
        tr = glm::translate(tr, vec3(0.0f, 0.0f, 0.25f)  );
        return tr;
    }
    else if (objid==4){
        mat4 tr(1.0f);
        tr = glm::translate(tr, vec3(0.0f, 0.0f, 0.25f)  );
        tr = glm::rotate(tr, -0.523599f*cos(glm::radians(2*k*50)), vec3(1.0f,0.0f, 0.0f ));
        tr = glm::translate(tr, vec3(0.0f, 0.0f, -0.25f)  );
        return tr;

    }
    else{
        return mat4(1.0f);
        
    }
    
}

void object::draw(float k){
    mat4 model = worldTrans;
    model = glm::translate(model, translateR);

    if(rAngleR!=0.0f)
        model = glm::rotate(model, rAngleR, rAxisR);
    mat4 transform = getTransform(k);
    model*=transform;
    mat4 temp = model;
    model = glm::scale(model, scaleR);
    //model = glm::scale(model, vec3(0.5f, 0.5f, 0.5f));
    // GLfloat angle = radians(20.0f) * i*k; 
    mat4 mvp = projection*view*model;
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    for(int i=0; i< children.size(); i++){
        children[i]->worldTrans *=temp;
        children[i]->draw(k);
        children[i]->worldTrans /=temp;
    }
}


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void do_movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
bool keys[1024];
GLfloat deltaTime = 0.0f;   // Time between current frame and last frame
GLfloat lastFrame = 0.0f;   // Time of last frame
GLfloat lastX = 400, lastY = 300;
GLfloat yaw0   = -90.0f; // Yaw0 is initialized to -90.0 degrees since a yaw0 of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch0 =   0.0f;
GLfloat fov = 45.0f;

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
        -50.0f,  -3.0f, -50.0f,  1.0f, 1.0f, 0.2f,
         50.0f,  -3.0f, -50.0f,  1.0f, 1.0f, 0.2f,
         50.0f,  -3.0f,  50.0f,  1.0f, 1.0f, 0.2f,
         50.0f,  -3.0f,  50.0f,  1.0f, 1.0f, 0.2f,
        -50.0f,  -3.0f,  50.0f,  1.0f, 1.0f, 0.2f,
        -50.0f,  -3.0f, -50.0f,  1.0f, 1.0f, 0.2f
    };

    object obj1(0);
    obj1.scaleR = vec3(1.0f, 0.75f, 0.75f);
    //obj1.rAxisR = vec3(0.0f, 0.0f, -1.0f);
    //obj1.rAngleR = 45.0f;
    object obj2(1);
    obj2.scaleR = vec3(0.5f, 0.5f, 0.5f);
    obj2.translateR = vec3(-0.5f, 0.375f, 0.0f);
    obj2.rAxisR = vec3(0.0f, 0.0f, -1.0f);
    obj2.rAngleR = glm::radians(-30.0f);
    obj1.children.push_back(&obj2);
    object obj3(2);
    obj3.scaleR = vec3(0.1f, 0.1f,0.5f);
    obj3.translateR = vec3(-0.3f, -0.3f, 0.625f);
    object obj4(4);
    obj4.scaleR = vec3(0.1f, 0.1f,0.5f);
    obj4.translateR = vec3(-0.3f, -0.3f, -0.625f);
    
    obj1.children.push_back(&obj3);
    obj1.children.push_back(&obj4);
    
    object obj5(3);
    obj5.scaleR = vec3(0.1f, 0.5f,0.1f);
    obj5.translateR = vec3(0.0f, -0.2f, 0.2f);
    obj3.children.push_back(&obj5);
    object obj6(3);
    obj6.scaleR = vec3(0.1f, 0.5f,0.1f);
    obj6.translateR = vec3(0.0f, -0.2f, -0.2f);
    obj4.children.push_back(&obj6);
    object obj7(2);
    obj7.scaleR = obj3.scaleR;
    obj7.translateR = vec3(0.4f, -0.3f, 0.625f);
    object obj8(4);
    obj8.scaleR = obj3.scaleR;
    obj8.translateR = vec3(0.4f, -0.3f, -0.625f);
    obj1.children.push_back(&obj7);
    obj1.children.push_back(&obj8);
    object obj9 = obj5;
    obj9.objid = 5;
    obj9.translateR = vec3(0.0f, -0.2f, 0.2f);
    obj7.children.push_back(&obj9);
    object obj10 = obj6;
    obj10.objid = 5;
    obj10.translateR = vec3(0.0f, -0.2f, -0.2f);
    obj8.children.push_back(&obj10);
    object obj11(6) ;
    obj11.scaleR = vec3(0.5f,0.1f,0.1f);
    obj11.translateR = vec3(0.2f, -0.2f, 0.0f );
    obj9.children.push_back(&obj11);
    object obj12(6) ;
    obj12.scaleR = vec3(0.5f,0.1f,0.1f);
    obj12.translateR = vec3(0.2f, -0.2f, 0.0f );
    obj10.children.push_back(&obj12);
    //eyes
    object obj13(7);
    obj13.scaleR = vec3(0.1f, 0.1f, 0.1f);
    obj13.translateR = vec3(-0.15f, 0.3f, 0.15f);
    object obj14(7);
    obj14.scaleR = obj13.scaleR;
    obj14.translateR = vec3(-0.15f, 0.3f, -0.15f);
    obj2.children.push_back(&obj13);
    obj2.children.push_back(&obj14);


    // obj3






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
    unsigned char* image = SOIL_load_image("textures/grass.png", &width, &height, 0, SOIL_LOAD_RGB);
    // unsigned char* image = SOIL_load_image("textures/frog.jpg", &width, &height, 0, SOIL_LOAD_RGB);
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
    image = SOIL_load_image("textures/frog1.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);


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


        // Bind Textures using texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);  
        
        // Activate shader
        ourShader.Use();     

        // mat4 view;
        // mat4 projection;
        mat4 mvp;

        // glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        // glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        // glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
        // glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
        // glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        // glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
        // GLfloat radius = 10.0f;
        // GLfloat camX = sin(glfwGetTime()) * radius;
        // GLfloat camZ = cos(glfwGetTime()) * radius;
        // view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        // // model = glm::rotate(model, (GLfloat)glfwGetTime() * radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(radians(fov), GLfloat(WIDTH) / GLfloat(HEIGHT), 0.1f, 100.0f);

        mvp_loc = glGetUniformLocation(ourShader.Program, "mvp");

        // Create transformations
        glm::mat4 transform;
        // transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        // transform = glm::rotate(transform, (GLfloat)glfwGetTime() * radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // Get matrix's uniform location and set matrix
        GLint transformLoc = glGetUniformLocation(ourShader.Program, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        // for(GLuint i = 0; i < 10; i++)
        // for(GLuint i = 0; i < 4; i++)
        // {

        //     model = glm::translate(model, cubePositions[i]);
            
        //     if(cubeRotnAngles[i]!=0.0f)
        //     model = glm::rotate(model, cubeRotnAngles[i], cubeRotnAxes[i]);
        //     model = glm::scale(model, cubeScales[i]);
        //     //model = glm::scale(model, vec3(0.5f, 0.5f, 0.5f));
        //     // GLfloat angle = radians(20.0f) * i*k; 
        //     mvp = projection*view*model;
        //     glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));

        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }
        //obj1.translateR = vec3(-k , 0.0f, 0.0f);
        obj1.draw(k);
        //obj.translateR += vec3(0.0f, )
        // obj3.rAxisR = vec3(1.0f, 0.0f, 0.0f);
        // obj3.rAngleR += 0.001f;
        //obj5.rAxisR = vec3(0.0f, 0.0f, -1.0f);
        //obj5.rAngleR += 0.001f;
        //obj1.rAngleR = (GLfloat)glfwGetTime() * radians(5.0f);
        glBindVertexArray(0);
        
        mat4 mvp1 = projection*view;
        // Draw container
        glBindVertexArray(VAO2);

        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp1));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // // Draw container
        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0);

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
