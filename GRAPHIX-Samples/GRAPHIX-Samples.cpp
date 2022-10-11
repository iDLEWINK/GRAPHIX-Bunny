/*The order matters. glad should be above GLFW*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// For 3D
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <string>
#include <iostream>

// GLM Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float mod_x = 0;
float z_mod = -5.0f;
float y_mod = 0.0f;
void Key_Callback(GLFWwindow* window,
    int key, //KeyCode
    int scanCode, //ScanCode
    int action, // Press or Release
    int mods // Modifier keys
) 
{
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        mod_x += 0.1f;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        mod_x -= 0.1f;
    }

    if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
        y_mod -= 0.3f;
    }

    if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
        y_mod += 0.3f;
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float screenWidth = 640.f;
    float screenHeight = 640.f;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    /*The order MATTERS*/
    // Initialize GLAD - must always come AFTER glfwMakeContext
    gladLoadGL();

    /* TEXTURES HERE */
    stbi_set_flip_vertically_on_load(true); // For image flip; Loads it in an upright manner.    
    int img_width, img_height, color_channels;
    /* Loaded texture */
    unsigned char* tex_bytes = stbi_load("3D/ayaya.png", // Texture path
                                                &img_width, // Fill width
                                                &img_height, // Fill height
                                                &color_channels, // Number of color channels
                                                0);

    /* TEXTURE SETUP */
    GLuint texture; // Create variable
    glGenTextures(1, &texture); // Generate 1 texture and assign it to texture variable
    glActiveTexture(GL_TEXTURE0); // Only one texture - but can have up to GL_TEXTURE60. You process succeeding textures individually if you have more than 1 on you wish to apply on an object
                                  // Tell OpenGL we're modifying texture at index 0
    glBindTexture(GL_TEXTURE_2D, texture); // Bind it for now

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA, //GL_RGB = jpegs or pngs w/o alphas; GL_RGBA = pngs or images w/ alpha; some pngs does not have an alpha channel
        img_width,
        img_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE, // Type of our loaded image
        tex_bytes // loaded texture in bytes
    );

    /* Assign the loaded texture; Generate the mipmaps to the current texture */
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes); // Free from memory

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, screenWidth, screenHeight);

    glfwSetKeyCallback(window, Key_Callback);

    //All of these are conversions for vert
    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertString = vertBuff.str();
    const char* v = vertString.c_str();

    //All of these are conversions for frag
    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragString = fragBuff.str();
    const char* f = fragString.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    std::string path = "3D/myCube.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    /* 8 points in a cube, each point is its own UV */
    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &materials,
        &warning,
        &error,
        path.c_str()
    );

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    /* Declare the vertices (x, y, z)*/
    GLfloat vertices[]{
        0.f, 0.5f, 0.f,     // Vertex 0
        -0.5f, -0.5f, 0.f,  // Vertex 1
        0.5f, -0.5f, 0.f    // Vertex 2
    };

    // EBO IMPLEMENTATION; Latter part of the lecture
    GLuint indices[]{
        0, 1, 2 // Triangle 1
    };

    /*Initialized VAO and VBO has an ID of unsigned integer - will be called by glGenVertexArrays*/
    /*EBO is now used*/
    GLuint VAO, VBO, EBO, VBO_UV;

    // Generate and assign ID to VAO
    glGenVertexArrays(1, &VAO);
    // Generate and assign ID to VBO
    glGenBuffers(1, &VBO);
    // Generate and assign ID to EBO
    glGenBuffers(1, &EBO);
    // Generate and assign ID to VBO_UV
    glGenBuffers(1, &VBO_UV);


    glUseProgram(shaderProgram);

    // Assigns or binds VAO - any calls after this will automatically point to VAO; Tells OpenGL we're working with that specified VAO
    glBindVertexArray(VAO);
    // Converts VBO to an array buffer that stores our vertex position; Tells OpenGL we're working with this VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Assigns data to VBO
    glBufferData(
        GL_ARRAY_BUFFER, // What data is in the buffer
        //sizeof(vertices), // Size of the whole buffer in bytes
        //sizeof(attributes.vertices) * attributes.vertices.size(), // alternative; sizeof(GL_FLOAT) * attributes.vertices.size()
        sizeof(GL_FLOAT) * attributes.vertices.size(),
        //vertices, // Vertex Array        
        &attributes.vertices[0],
        GL_STATIC_DRAW //The position won't update therefore it's static
    );

    // Tell OpenGL how to interpret the array
    glVertexAttribPointer(
        0, // Position - SPECIAL INDEX 0, 1, and 2; See notes below in line 64
        3, // X, Y, Z
        GL_FLOAT, // Type of ARRAY
        GL_FALSE, // Should we normalize this?
        //3 * sizeof(float), // How big the 3 points are in bytes? Size of Vertex data ; alternative sizeof(GL_FLOAT)
        3 * sizeof(GL_FLOAT),
        (void*)0
    );

    // We tell OpenGL we are working with our EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        //sizeof(indices),
        sizeof(GLuint) * mesh_indices.size(),
        //indices,
        mesh_indices.data(),
        GL_STATIC_DRAW
    );

    /* We're currently working with texture */
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(GL_ARRAY_BUFFER,
            sizeof(GL_FLOAT) * (sizeof(UV) / sizeof(UV[0])), // GL_FLOAT * length of the array
            &UV[0],
            GL_STATIC_DRAW
        );

    glVertexAttribPointer(
        2, // Tex Coords or UVs are usually at index 2. Index 2 is just for convention btw we could use 1 or 5 or 6
        2, // U and V
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(GL_FLOAT),
        (void*)0
    );



    // Because we used 0 in the glVertexAttribPointer just above; Subject to change 0, 1, 2
    // SPECIAL INDICES
    // 0 is for the Position of the vertex
    // 1 is for the Normals
    // 2 is for the Textures
    glEnableVertexAttribArray(0);

    /* Enable 2 for the texture or for our UV / Tex Coords*/
    glEnableVertexAttribArray(2);

    // We're done modifying - bind it to 0
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // We're done modifying - bind it to 0
    glBindVertexArray(0); 
    // We're done modifying - bind it to 0
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glm::mat4 identity_matrix = glm::mat4(1.0f); // 4x4 identity matrix

    //***********************TRANSLATION
    float x, y, z;
    x = y = z = 0.0f;

    /*
    glm::mat4 translation = glm::translate(identity_matrix, 
        glm::vec3(x, y, z)
    );
    */

    //************************SCALE
    float scale_x, scale_y, scale_z;
    scale_x = scale_y = scale_z = 2.0f; // 1 is neither scaled up or shrunked

    /*
    glm::mat4 scale = glm::scale(identity_matrix,
        glm::vec3(scale_x, scale_y, scale_z)
    );
    */

    //*************************ROTATION
    float rot_x, rot_y, rot_z;
    rot_x = rot_y = rot_z = 0.0f; //But there should be at least 1 that's greater than 0. Not all should be 0.
    rot_y = 1.0f;
    float theta = 90.f; // Rotate the model by 90 degrees

    /*
    glm::mat4 rotation = glm::rotate(identity_matrix,
        glm::radians(theta), // Convert from degrees to radians
        //glm::vec3(rot_x, rot_y, rot_z) // Normalized Axis Vector
        // Normalized Vector = Magnitude / Length = 1
        glm::normalize(glm::vec3(rot_x, rot_y, rot_z)) // OPTIONAL: if you wanna make sure that it's normalized
    );
    */

    // float mod_x = 0;

    /*
    glm::mat4 projection_matrix = glm::ortho(-1.0f, 1.0f, // L -> R || L < R
                                            -1.0f, 1.0f, // B -> T || B < T
                                            -2.0f, 2.0f); // Znear --> ZFar != 0  || Znear < ZFar
    */
    z = -1.0f;
    
    glm::mat4 projection_matrix = glm::perspective(
        glm::radians(60.f), // FOV in degrees
        screenHeight / screenWidth, // Aspect Ratio
        0.1f, // Z Near
        100.f // Z Far
    );

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
        //theta = mod_x;
        z = z_mod;
        theta += 0.25f;

        /* Camera View Matrix */
        glm::vec3 cameraPos = glm::vec3(0, 0, 10.f);
        glm::mat4 cameraPosMatrix = glm::translate(glm::mat4(1.0f), cameraPos * -1.0f);

        /* Necessary elements for the vectors */
        glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
        glm::vec3 cameraCenter = glm::vec3(0, y_mod, 0);

        /* Three camera vectors */
        glm::vec3 F = cameraCenter - cameraPos;
        F = glm::normalize(F);
        glm::vec3 R = glm::cross(F, WorldUp); // Normalized already so we don't need to normalize anymore. But we can normalize to it again to make sure.
        glm::vec3 U = glm::cross(R, F);

        glm::mat4 cameraOrientationMatrix = glm::mat4(1.0f); // Double-sided array

        // 1st COL Matrix
        // 2nd ROW Matrix
        // Matrix[col][row]
        // NOTICE THAT IT'S THE OPPOSITE
        cameraOrientationMatrix[0][0] = R.x;
        cameraOrientationMatrix[1][0] = R.y;
        cameraOrientationMatrix[2][0] = R.z;

        cameraOrientationMatrix[0][1] = U.x;
        cameraOrientationMatrix[1][1] = U.y;
        cameraOrientationMatrix[2][1] = U.z;

        cameraOrientationMatrix[0][2] = -F.x;
        cameraOrientationMatrix[1][2] = -F.y;
        cameraOrientationMatrix[2][2] = -F.z;
      
        glm::mat4 viewMatrix = cameraOrientationMatrix * cameraPosMatrix;
        // glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraCenter, WorldUp); <----- VERY SHORT SHORTCUT

        glm::mat4 transformation_matrix = glm::mat4(1.0f); // Creates your base identity matrix

        //Translation
        transformation_matrix = glm::translate(transformation_matrix,
            glm::vec3(x, y, z));

        //Multiply resulting matrix with scale matrix
        transformation_matrix = glm::scale(transformation_matrix,
            glm::vec3(scale_x, scale_y, scale_z));

        //Multiply it with rotation matrix
        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(theta),
            glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));

        /*
        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(theta),
            glm::normalize(glm::vec3(1.0f, 0.0f, rot_z)));        
        */
        /* DRAW THE TEXTURE */
        GLuint tex0Address = glGetUniformLocation(shaderProgram, "tex0"); // Get the address
        glBindTexture(GL_TEXTURE_2D, texture); // Call OpenGL we're using that texture
        glUniform1i(tex0Address, 0); // Comes from GL_TEXTURE0

        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        unsigned int transformationLoc = glGetUniformLocation(shaderProgram, "transform"); // transform is the variable from sample.vert
        glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

        //Controls if to the left or to the right
        //mod_x += 0.001f;

        /*** REMOVE ANIMATION CODES FOR NOW
        unsigned int xLoc = glGetUniformLocation(shaderProgram, "x");
        glUniform1f(xLoc, mod_x);
        */

        /*
        * SIMPLE IMPLEMENTATION
        * 
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();
        */
      
        glBindVertexArray(VAO);

        // We don't need this anymore
        /* glDrawArrays(GL_TRIANGLES, 0, 3); */

        //glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}