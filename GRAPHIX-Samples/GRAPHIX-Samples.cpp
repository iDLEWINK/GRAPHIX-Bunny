/*The order matters. glad should be above GLFW*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// For 3D
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <string>
#include <iostream>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

    std::string path = "3D/bunny.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;

    tinyobj::attrib_t attributes;

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
    GLuint VAO, VBO, EBO;

    // Generate and assign ID to VAO
    glGenVertexArrays(1, &VAO);
    // Generate and assign ID to VBO
    glGenBuffers(1, &VBO);
    // Generate and assign ID to EBO
    glGenBuffers(1, &EBO);
    
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

    // Tell OpenGL how to interpret that data
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

    // Because we used 0 in the glVertexAttribPointer just above; Subject to change 0, 1, 2
    // SPECIAL INDICES
    // 0 is for the Position of the vertex
    // 1 is for the Normals
    // 2 is for the Textures
    glEnableVertexAttribArray(0);

    // We're done modifying - bind it to 0
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // We're done modifying - bind it to 0
    glBindVertexArray(0); 
    // We're done modifying - bind it to 0
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    float mod_x = 0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        mod_x += 0.001f;

        unsigned int xLoc = glGetUniformLocation(shaderProgram, "x");
        glUniform1f(xLoc, mod_x);

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