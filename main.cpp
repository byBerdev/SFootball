// SFootball - main.cpp
// Base C++ multiplataforma: Windows / macOS / Android
// SDL2 + OpenGL + Assimp (OBJ/GLB)

#include <SDL2/SDL.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>
#include <string>


struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};


std::vector<Vertex> vertices;
std::vector<unsigned int> indices;


bool LoadModel(const std::string& file)
{
    Assimp::Importer importer;

    const aiScene* scene =
        importer.ReadFile(
            file,
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs
        );


    if(!scene || !scene->mRootNode)
    {
        std::cout << importer.GetErrorString();
        return false;
    }


    aiMesh* mesh = scene->mMeshes[0];


    for(unsigned int i=0;i<mesh->mNumVertices;i++)
    {
        Vertex v;

        v.pos.x = mesh->mVertices[i].x;
        v.pos.y = mesh->mVertices[i].y;
        v.pos.z = mesh->mVertices[i].z;


        if(mesh->HasNormals())
        {
            v.normal.x = mesh->mNormals[i].x;
            v.normal.y = mesh->mNormals[i].y;
            v.normal.z = mesh->mNormals[i].z;
        }


        vertices.push_back(v);
    }


    for(unsigned int i=0;i<mesh->mNumFaces;i++)
    {
        aiFace face = mesh->mFaces[i];

        for(unsigned int j=0;j<face.mNumIndices;j++)
            indices.push_back(face.mIndices[j]);
    }


    return true;
}



GLuint CompileShader(GLenum type,const char* source)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(
        shader,
        1,
        &source,
        nullptr
    );

    glCompileShader(shader);

    return shader;
}



GLuint CreateShaderProgram()
{
    const char* vertex = R"(

    #version 330 core

    layout(location=0) in vec3 position;

    uniform mat4 MVP;

    void main()
    {
        gl_Position = MVP *
        vec4(position,1.0);
    }

    )";


    const char* fragment = R"(

    #version 330 core

    out vec4 color;

    void main()
    {
        color =
        vec4(0.1,0.8,0.1,1);
    }

    )";


    GLuint vs =
    CompileShader(
        GL_VERTEX_SHADER,
        vertex
    );


    GLuint fs =
    CompileShader(
        GL_FRAGMENT_SHADER,
        fragment
    );


    GLuint program =
    glCreateProgram();


    glAttachShader(program,vs);
    glAttachShader(program,fs);

    glLinkProgram(program);


    glDeleteShader(vs);
    glDeleteShader(fs);


    return program;
}



int main()
{
    SDL_Init(SDL_INIT_VIDEO);


    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_MAJOR_VERSION,
        3
    );

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_MINOR_VERSION,
        3
    );


    SDL_Window* window =
    SDL_CreateWindow(
        "SFootball 26",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_OPENGL
    );


    SDL_GLContext context =
    SDL_GL_CreateContext(window);


#ifndef __APPLE__
    glewInit();
#endif


    glEnable(GL_DEPTH_TEST);


    GLuint shader =
    CreateShaderProgram();



    GLuint VAO,VBO,EBO;


    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);



    LoadModel(
        "assets/models/player.glb"
    );


    glBindVertexArray(VAO);


    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size()*sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW
    );


    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        EBO
    );


    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size()*sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );


    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)0
    );


    glEnableVertexAttribArray(0);



    glm::vec3 camera(
        0,
        2,
        6
    );


    bool running=true;


    while(running)
    {
        SDL_Event event;


        while(SDL_PollEvent(&event))
        {
            if(event.type==SDL_QUIT)
                running=false;


            if(event.type==SDL_KEYDOWN)
            {
                if(event.key.keysym.sym==SDLK_ESCAPE)
                    running=false;
            }
        }



        glClearColor(
            0.2,
            0.5,
            0.9,
            1
        );


        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );



        glm::mat4 projection =
        glm::perspective(
            glm::radians(45.0f),
            1280.0f/720.0f,
            0.1f,
            100.0f
        );


        glm::mat4 view =
        glm::lookAt(
            camera,
            glm::vec3(0,0,0),
            glm::vec3(0,1,0)
        );


        glm::mat4 model =
        glm::mat4(1.0f);


        glm::mat4 MVP =
        projection *
        view *
        model;



        glUseProgram(shader);


        glUniformMatrix4fv(
            glGetUniformLocation(
                shader,
                "MVP"
            ),
            1,
            GL_FALSE,
            &MVP[0][0]
        );


        glBindVertexArray(VAO);


        glDrawElements(
            GL_TRIANGLES,
            indices.size(),
            GL_UNSIGNED_INT,
            0
        );


        SDL_GL_SwapWindow(window);
    }



    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteBuffers(1,&EBO);


    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}
