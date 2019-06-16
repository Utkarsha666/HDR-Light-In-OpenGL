/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// AUTHOR:- Utkarsha Khanal ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// LANGUAGE:- C++ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// YEAR:- 2009 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// OPENGL 3.3 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// TOOLS USED:- GLFW, GL, SOIL //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// PURPOSE:- A SIMPLE 3D ROOM WHERE THE USER CAN ROAM /////////////////////////////////////////////////////////////////////////////////////////////
/////////// THIS EXAMPLE DEMONSTRATE THE FOLLOWING CONCEPTS TEXTURE MAPPING, DRAWING WITH OPENGL, LIGHTING, POST-SCREEN PROCESSING, HDR LIGHT /////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

//mathematics library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//image library
#include "SOIL.h"
#include "Shader.h"
#include "camera.h"

// Standard C++ Library
#include <iostream>
#include <vector>

Camera camera(glm::vec3(-0.1f, 0.1f, 2.5f));

//// Function Prototypes
void renderCube();
void renderPlane();
void drawQuad();
GLuint loadTexture(char* path);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void move();

GLuint SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 1050;
/////// This variable is for constant frame rate among different speed computer /////
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

/////// Start of the main function /////////
int main(int argc, char** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Main", nullptr, nullptr); 
    glfwMakeContextCurrent(window);
    
    if (!window) {
        std::cout << "Window creation failed: " << std::endl;
        std::cout << "You can press 'ESC' to exit " << std::endl;
    } else {
        std::cout << "Window Successfully Created: " << std::endl;
    }   // end if-else statement //

    
    //Callback functions.
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    //Enable depth test and make the anti-clockwise the front facing polygon.
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();
    
    //Shaders
    Shader myShader("Shaders/main.vert", "Shaders/main.frag");
    Shader hdrShader("Shaders/hdrShader.vert", "Shaders/hdrShader.frag");
    Shader lightBoxShader("Shaders/main.vert", "Shaders/light.frag");
    
    //Cube Texture
    GLuint wallTexture = loadTexture("Textures/Wall.jpg");
    GLuint cubeTexture = loadTexture("Textures/container.jpg");
    GLuint woodTexture = loadTexture("Textures/wood.png");
    GLuint containerDiffuse = loadTexture("Textures/container2.png");
    GLuint containerSpecular = loadTexture("Texture/container2_specular.png");
    
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    //Background clear color.
    
    // Creaate frame buffer ///
    GLuint hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    
    // create color buffer //
    GLuint colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    ///// Create a depth buffer ///
    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    //Attach Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    
    //check if Successfull
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" <<std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////// MAIN PROGRAM LOOP /////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    while(!glfwWindowShouldClose(window))
    {
        ////////////// Light Positions /////////////////////////////////////////////////////////
        std::vector<glm::vec3> lightPositions;
        lightPositions.push_back(glm::vec3(0.0f, -1.5, -9.0f));
        lightPositions.push_back(glm::vec3(0.0f, 0.7f, 1.0f));
        lightPositions.push_back(glm::vec3(0.5f, 2.0f, -5.0f));
        lightPositions.push_back(glm::vec3(0.5f, 2.0f, -8.0f));
/////////////////////////////// Light Colors /////////////////////////////////////////
        std::vector<glm::vec3> lightColors;
        lightColors.push_back(glm::vec3(10.0f, 10.0f, 0.0f));
        lightColors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
        lightColors.push_back(glm::vec3(0.0f, 3.0f, 0.0f));
        lightColors.push_back(glm::vec3(0.0f, 0.0f, 3.0f));
        
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glfwPollEvents();
        move();
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        myShader.Use();
        
        // Creating the model, view and projection matrix initialize it and pass it to the shader program ///
        glm::mat4 model(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(45.0f, (GLfloat)SCREEN_HEIGHT/(GLfloat)SCREEN_WIDTH, 0.1f, 1000.0f);
     
        for (GLuint i = 0; i < lightPositions.size(); i++)
        {
            glUniform3fv(glGetUniformLocation(myShader.Program, ("lights[" + std::to_string(i) + "].lightPos").c_str()), 1, &lightPositions[i][0]);
            glUniform3fv(glGetUniformLocation(myShader.Program, ("lights[" + std::to_string(i) + "].lightColor").c_str()), 1, &lightColors[i][0]);
        }
        
         //////////////////////////////////////
        ///// This draws the room wall ///////
        /////////////////////////////////////
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        renderPlane();
        
        model = glm::scale(model, glm::vec3(0.5f, 0.02f, 1.0f));
        // Pass the value to the shaders
        glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        //That cube texture
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        // and finally draw the cube on the screen.
        renderCube();
        
        
        // This draws the table stand //////
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.27, 0.0f));
        model = glm::scale(model, glm::vec3(0.03f, 1.5f, 0.02f));
        glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        renderCube();
        
        //// This draws cube on top of the table //////////////////////////////////////
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.12f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindTexture(GL_TEXTURE_2D, containerDiffuse);
        glActiveTexture(GL_TEXTURE0);
        renderCube();
        
        /// Draw lamp cube //
        lightBoxShader.Use();
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f, 0.1f, 0.1f));
        model = glm::translate(model, glm::vec3(0.0f, -9.2f, -99.4f));
        glUniformMatrix4fv(glGetUniformLocation(lightBoxShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        renderCube(); 
       
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.Use();
        glUniform1i(glGetUniformLocation(hdrShader.Program, "exposure"), 0.2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        drawQuad();
       
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}                   // End of main function

//////////////////////////////////////////////////////////
// This function just renders the plane on the screen ///
/////////////////////////////////////////////////////////
void renderPlane()
{
    GLfloat planeVertices[] = {
                    //floor
                    3.0f, -1.0f, -10.0f,         0.0f, 1.0f, 0.0f,          2.0f, 2.0f,
                   -3.0f, -1.0f, -10.0f,         0.0f, 1.0f, 0.0f,           0.0f, 2.0f,
                   -3.0f, -1.0f,  3.0f,          0.0f, 1.0f, 0.0f,           0.0f, 0.0f,
                   -3.0f, -1.0f,  3.0f,          0.0f, 1.0f, 0.0f,           0.0f, 0.0f,
                    3.0f, -1.0f,  3.0f,          0.0f, 1.0f, 0.0f,           2.0f, 0.0f,
                    3.0f, -1.0f, -10.0f,         0.0f, 1.0f, 0.0f,           2.0f, 2.0f,
                    //right
                    3.0f, 3.0f, -10.0f,        -1.0f, 0.0f, 0.0f,            0.0f, 2.0f,
                    3.0f, 3.0f,  3.0f,         -1.0f, 0.0f, 0.0f,            2.0f, 2.0f,
                    3.0f,-1.0f,  3.0f,         -1.0f, 0.0f, 0.0f,            2.0f, 0.0f,
                    3.0f,-1.0f,  3.0f,         -1.0f, 0.0f, 0.0f,            2.0f, 0.0f,
                    3.0f,-1.0f, -10.0f,        -1.0f, 0.0f, 0.0f,            0.0f, 0.0f,
                    3.0f, 3.0f, -10.0f,        -1.0f, 0.0f, 0.0f,            0.0f, 2.0f,
                     
                    //Back Side
                    -3.0f, 3.0f, -10.0f,         0.0f, 0.0f, 1.0f,            0.0f, 2.0f,
                     3.0f, 3.0f, -10.0f,         0.0f, 0.0f, 1.0f,            2.0f, 2.0f,
                     3.0f, -1.0f, -10.0f,        0.0f, 0.0f, 1.0f,            2.0f, 0.0f,
                     3.0f, -1.0f, -10.0f,        0.0f, 0.0f, 1.0f,            2.0f, 0.0f,
                    -3.0f, -1.0f, -10.0f,        0.0f, 0.0f, 1.0f,            0.0f, 0.0f,
                    -3.0f, 3.0f, -10.0f,         0.0f, 0.0f, 1.0f,            0.0f, 2.0f,
    };
    
    GLuint planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    
    //Now we are ready to draw the plane 
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}       //end function renderPlane

void renderCube()
{
    GLuint cubeVBO = 0 , cubeVAO = 0;
    
    if (cubeVAO == 0) {
        GLfloat cubeVertices[] = {
        /// Positions                 //Normals                     //Texture Coords
            
        //Front Face of the Cube
           -0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f,               0.0f,  0.0f,
            0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f,               1.0f,  0.0f,
            0.5f,  0.5f, 0.5f,      0.0f, 0.0f, 1.0f,               1.0f,  1.0f,
            0.5f,  0.5f, 0.5f,      0.0f, 0.0f, 1.0f,               1.0f,  1.0f,
            -0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,               0.0f,  1.0f,
            -0.5f, -0.5f,0.5f,      0.0f, 0.0f, 1.0f,               0.0f,  0.0f,
            
        //Back Side of the cube
            
            -0.5f, 0.5f, -0.5f,     0.0f, 0.0f, -1.0f,              0.0f, 1.0f, 
            0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f,              1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,              1.0f, 0.0f,
            0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,              1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,              0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f,     0.0f, 0.0f, -1.0f,              0.0f, 1.0f,
        
        //Here comes the down side
        
            -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,              0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,    0.0f, -1.0f, 0.0f,               1.0f, 1.0f,
            0.5f, -0.5f, 0.5f,     0.0f, -1.0f, 0.0f,               1.0f, 0.0f,
            0.5f, -0.5f, 0.5f,     0.0f, -1.0f, 0.0f,               1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,    0.0f, -1.0f, 0.0f,               0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,               0.0f, 1.0f,
            
            //Upside of the cube
        
            0.5f, 0.5f, -0.5f,     0.0f, 1.0f, 0.0f,                1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f,    0.0f, 1.0f, 0.0f,                0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,                0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f,     0.0f, 1.0f, 0.0f,                0.0f, 0.0f,
            0.5f, 0.5f, 0.5f,      0.0f, 1.0f, 0.0f,                0.0f, 1.0f,
            0.5f, 0.5f, -0.5f,     0.0f, 1.0f, 0.0f,                1.0f, 1.0f,
            
            //right side of the cube
            
            0.5f, 0.5f, -0.5f,     1.0f, 0.0f, 0.0f,                1.0f, 1.0f,
            0.5f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f,                0.0f, 1.0f,
            0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,                0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,                0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,                1.0f, 0.0f,
            0.5f, 0.5f, -0.5f,     1.0f, 0.0f, 0.0f,                1.0f, 1.0f,
            
            //left side of the cube
            
            -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,               1.0f, 0.0f, 
            -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,               1.0f, 1.0f, 
            -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,               0.0f, 1.0f, 
            -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,               0.0f, 1.0f, 
            -0.5f, -0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,               0.0f, 0.0f, 
            -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,               1.0f, 0.0f, 
        };
        /// Generate The vertex array and buffer object.
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid *)(6*sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }
    
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void drawQuad()
{
    GLuint quadVAO;
    GLuint quadVBO;
    
    GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // Setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

//This function loads the texture to the texture memory and returns the textureID to the caller.
GLuint loadTexture(char* path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    
    /////// return the value to the caller.
    return textureID;
}

bool keys[1024];
bool keysPressed[1024];
// Moves/alters the camera positions based on user input
void move()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);

}

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key <= 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
            keysPressed[key] = false;
        }
    }
}
/// Mouse callback function/
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

