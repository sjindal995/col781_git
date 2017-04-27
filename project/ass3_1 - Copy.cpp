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

glm::vec3 cameraPos   = glm::vec3(0.0f, 10.0f,  30.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
bool keys[1024];
GLfloat deltaTime = 0.0f;   // Time between current frame and last frame
GLfloat lastFrame = 0.0f;   // Time of last frame
GLfloat lastX = 400, lastY = 300;
GLfloat yaw0   = -90.0f; // Yaw0 is initialized to -90.0 degrees since a yaw0 of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch0 =   0.0f;
GLfloat fov = 45.0f;
int numFaces = 20;
int trunkobjs =40;
GLfloat skew = 0.95f;
vec3 scaleglob =vec3(skew, skew, skew);

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
        void draw(float,int);
        // mat4 getTransform(float);
        float base_ht;
        float ht;
        float total_length;
    
};

vector<object> objs;

object::object(int id){
    worldTrans = mat4(1.0f);
    scaleR = vec3(1.0f,1.0f,1.0f);
    translateR = vec3(0.0f, 0.0f, 0.0f);
    rAxisR = vec3(1.0,1.0,1.0f);
    rAngleR = 0.0f;
    objid = id;
    base_ht = 0.0;
    ht = 0.0;
    total_length = 0.0;
}


float c2 = 0.221875;
float c4 = 0.754029;
float damp = 0.9;

void object::draw(float k, int step){
    if(step > 360){
        step %= 360;
        damp = pow(damp, step/360);
    }
    //cout << "aaaaaaaaaaaa: " << objs[0].children[0]->children[0]->objid << endl;
    cout << "objid: " << objid  << ", base: " << base_ht << ", ht: " << ht << endl;
    float x0 = base_ht/total_length;
    float x1 = (base_ht + ht)/total_length;
    float delta = (asin(c2*pow(x1,2) + c4*pow(x1,4)) - asin(c2*pow(x0,2) + c4*pow(x0,4)))*sin(glm::radians(GLfloat(step)))*damp;
    cout << delta << ", " << sin(glm::radians(GLfloat(90)))<<  endl;
    mat4 model = worldTrans;
    GLfloat yskew = scaleR.y;
    model = glm::translate(model, vec3(translateR.x, translateR.y*yskew, translateR.z) );
    if(rAngleR!=0.0f){
        model = glm::translate(model, vec3(0.0f, -0.5*yskew, 0.0f));
        model = glm::rotate(model, rAngleR + delta, rAxisR);
        model = glm::translate(model, vec3(0.0f, 0.5f*yskew, 0.0f));
    }
    //mat4 transform = getTransform(k);
    //model*=transform;
    mat4 temp = model;
    model = glm::scale(model, scaleR);
    //model = glm::scale(model, vec3(0.5f, 0.5f, 0.5f));
    // GLfloat angle = radians(20.0f) * i*k; 
    mat4 mvp = projection*view*model;
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));
    glDrawArrays(GL_TRIANGLES, 0, 6*numFaces);
    for(int i=0; i< children.size(); i++){
        //cout << "children: " << i << ", objid: " << children[i]->objid << endl;
        children[i]->worldTrans *=temp;
        children[i]->scaleR = vec3(children[i]->scaleR.x*scaleR.x, children[i]->scaleR.y*scaleR.y, children[i]->scaleR.z*scaleR.z);
        
        children[i]->base_ht = base_ht + ht;
        children[i]->ht = ht*skew;

        children[i]->draw(children[i]->objid, step);
        children[i]->worldTrans /=temp;
        children[i]->scaleR = vec3(children[i]->scaleR.x/scaleR.x, children[i]->scaleR.y/scaleR.y, children[i]->scaleR.z/scaleR.z); 
    }
    // cout << "drawn" << endl;
}


void addBranch(object* parent, int noObjs, vec3 translate, vec3 axis, GLfloat angle, GLfloat scale ){
    object* par = parent;

    for(int kk=0; kk<noObjs; kk++){
        // object obj(offset+kk);
        object obj(0);
        if(kk==0) {
            obj.scaleR = vec3(scaleglob.x * scale, scaleglob.y, scaleglob.z*scale);
            // obj.translateR = vec3(0.3f, 0.3f, 0.0f);
            obj.translateR = translate;
            obj.rAxisR = axis;
            obj.rAngleR = angle;
        }
        else {
            obj.scaleR = scaleglob;
            obj.translateR = vec3(0.0f, 0.9f, 0.0f);
            obj.rAxisR = vec3(0.0f, 0.0f, 1.0f);
            obj.rAngleR = glm::radians((12.0f * (float(rand())/float(RAND_MAX))) - 6.0f);
            //obj.rAngleR = glm::radians(-25.0f);
        }
        // cout << "here0" << endl;
        objs.push_back(obj);
        // cout << "here1: " << objs.size() << endl;
        par->children.push_back(&objs[objs.size()-1]);    
        // cout << "here2" << endl;
        // cout<<kk<<" par "<<par->children.size()<<endl;
        // if(par->children.size() > 0){
        //     cout << "aaaaaaaaa: " << par->objid << ", " << par->children[0]->objid << endl; 
        // }
        par = &objs[objs.size()-1];
        // cout << "here3: " << endl;
    }
}


float getTotalLength(float init_ht, float y_scale, int n_objs){
    float res = 0.0f;
    float cur_ht = init_ht;
    for(int i=0;i<n_objs;i++){
        res += cur_ht;
        cur_ht *= y_scale;
    }
    return res;
}


// The MAIN function, from here we start the application and run the game loop
int main()
{
    objs.reserve(1000);
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
    GLfloat vertices[5*6*numFaces];
    GLfloat rad60 = glm::radians(60.0f);
    GLfloat theta = 0.0f;
    GLfloat delta = 360.0f/numFaces;
    //GLfloat skew = 0.95f;
    for(int i=0;i<numFaces;i++){
        vertices[30*i] = 1.0f*cos(glm::radians(theta));
        vertices[30*i+1] = -0.5f;
        vertices[30*i+2] = -1.0f*sin(glm::radians(theta));
        vertices[30*i+3] = 0.0f;
        vertices[30*i+4] =0.0f;


        vertices[30*i+5] = skew*1.0f*cos(glm::radians(theta));
        vertices[30*i+6] = 0.5f;
        vertices[30*i+7] = skew*-1.0f*sin(glm::radians(theta));
        vertices[30*i+8] = 0.0f;
        vertices[30*i+9] = 1.0f;


        vertices[30*i+10] = skew*1.0f*cos(glm::radians(theta+delta));
        vertices[30*i+11] = 0.5f;
        vertices[30*i+12] = skew*-1.0f*sin(glm::radians(theta+delta));
        vertices[30*i+13] = 1.0f;
        vertices[30*i+14] = 1.0f;

        vertices[30*i+15] = skew*1.0f*cos(glm::radians(theta+delta));
        vertices[30*i+16] = 0.5f;
        vertices[30*i+17] = skew*-1.0f*sin(glm::radians(theta+delta));
        vertices[30*i+18] = 1.0f;
        vertices[30*i+19] = 1.0f;

        vertices[30*i+20] = 1.0f*cos(glm::radians(theta+delta));
        vertices[30*i+21] = -0.5f;
        vertices[30*i+22] = -1.0f*sin(glm::radians(theta+delta));
        vertices[30*i+23] = 1.0f;
        vertices[30*i+24] = 0.0f;


        vertices[30*i+25] = 1.0f*cos(glm::radians(theta));
        vertices[30*i+26] = -0.5f;
        vertices[30*i+27] = -1.0f*sin(glm::radians(theta));
        vertices[30*i+28] = 0.0f;
        vertices[30*i+29] = 0.0f;


        theta += delta;
    }

    // for(int i=0;i<6*numFaces;i++){
    //     cout << vertices[3*i] << "," << vertices[3*i+1] << "," << vertices[3*i+2] << endl;
    // }


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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // // // Color attribute
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    // glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    //texture code
    GLuint texture1;
    // GLuint texture2;
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
    unsigned char* image = SOIL_load_image("textures/bark.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    // unsigned char* image = SOIL_load_image("textures/frog.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
    // // ===================
    // // Texture 2
    // // ===================
    // glGenTextures(1, &texture2);
    // glBindTexture(GL_TEXTURE_2D, texture2);
    // // Set our texture parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // // Set texture filtering
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // // Load, create texture and generate mipmaps
    // image = SOIL_load_image("textures/frog1.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    // glGenerateMipmap(GL_TEXTURE_2D);
    // SOIL_free_image_data(image);
    // glBindTexture(GL_TEXTURE_2D, 0);




    //object obj2;
    float trunk_length = getTotalLength(1.0f,skew,trunkobjs);
    for(int kk=0; kk<trunkobjs; kk++){
        object obj(kk);
        // object obj(0);
        obj.scaleR = scaleglob;
        obj.translateR = vec3(0.0f, 0.9f, 0.0f);
        obj.rAxisR =vec3(0.0f, 0.0f, 1.0f);
        obj.rAngleR = glm::radians( (12.0f * (float(rand())/float(RAND_MAX))) - 6.0f);
        obj.base_ht = 0.0f;
        obj.ht = 1.0f;
        obj.total_length = trunk_length;
        if(kk==0) obj.rAngleR = 0.0f;
        // obj 
        // if(kk!=0) objs[kk-1].children.push_back()
        // obj2 = obj;
        objs.push_back(obj);
    }
    for(int kk=0; kk<trunkobjs-1; kk++){
        (&objs[kk])->children.push_back(&objs[kk+1]);
    }
    
    // object* branchnode = &objs[10];
    // int offset = objs.size();
    // int branch1_objs = 20;
    // for(int kk=0; kk<branch1_objs; kk++){
    //     // object obj(offset+kk);
    //     object obj(0);
    //     if(kk==0) {
    //         obj.scaleR = vec3(scaleglob.x * 0.5f, scaleglob.y, scaleglob.z*0.5f);
    //         obj.translateR = vec3(0.3f, 0.3f, 0.0f);
    //         obj.rAxisR = vec3(0.5f, 0.0f, 1.0f);
    //         obj.rAngleR = glm::radians(-45.0f);
    //     }
    //     else {
    //         obj.scaleR = scaleglob;
    //         obj.translateR = vec3(0.0f, 0.9f, 0.0f);
    //         obj.rAxisR = vec3(0.0f, 0.0f, 1.0f);
    //         obj.rAngleR = glm::radians((12.0f * (float(rand())/float(RAND_MAX))) - 6.0f);
    //         //obj.rAngleR = glm::radians(-25.0f);
    //     }
    //     // cout << "here0" << endl;
    //     objs.push_back(obj);
    //     // cout << "here1: " << objs.size() << endl;
        
    // }
    // (&objs[10])->children.push_back(&objs[offset]);
    // for(int kk=0; kk<branch1_objs-1; kk++){
    //     (&objs[offset + kk])->children.push_back(&objs[offset + kk+1]);
    // }

    
    // addBranch(&objs[15], 25, vec3(0.3f, 0.2f, 0.0f), vec3(0.2f, 0.2f, 0.5f), glm::radians(-30.0f), 0.3f);
    // addBranch(&objs[50], 16, vec3(-0.1f, 0.3f, 0.0f), vec3(0.3f, 0.0f, 1.0f), glm::radians(45.0f), 0.5f);
    
    // addBranch(&objs[20], 20, vec3(-0.1f, 0.3f, 0.0f), vec3(0.4f, 0.3f, 1.0f), glm::radians(45.0f), 0.5f);
    

	// for(int i=0; i<objs.size(); i++) cout<<i<<" "<<objs[i].children.size()<<endl;    
    // Game loop
    float k=0.001;
    int step = 0;
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

        //use texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);
        // glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);  

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
        objs[0].draw(0,step);
        step+=1;
   //      glm::mat4 mvp2 = mvp;
   //      for(int kk=0; kk<5; kk++){
   //          glUniformMatrix4fv(mvp_loc
			// , 1, GL_FALSE, glm::value_ptr(mvp2));
   //          mvp2 = glm::translate(mvp2, vec3(0.0f, 0.8f, 0.0f));    
   //          // GLfloat angle = float(rand())/float(RAND_MAX);
			// GLfloat angle=1.0f;    
   //          // mvp2 = glm::rotate(mvp2,glm::radians(5.0f*angle), vec3(0.0f, 0.0f, 1.0f)); 
			// mvp2 = glm::scale(mvp2, vec3(0.95f, 1.0f, 0.95f));       
   //      	glDrawArrays(GL_TRIANGLES, 0, 6*numFaces);
   //      }

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
