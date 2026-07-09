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

#include <vector>
#include <iostream>
#include <string>
#include <cmath>


const int WIDTH = 1280;
const int HEIGHT = 720;


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};


std::vector<Vertex> vertices;
std::vector<unsigned int> indices;



bool LoadModel(std::string path)
{
    Assimp::Importer importer;


    const aiScene* scene =
        importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_GenNormals
        );


    if(!scene || !scene->mRootNode)
    {
        std::cout<<"Erro modelo\n";
        return false;
    }


    aiMesh* mesh =
        scene->mMeshes[0];


    for(unsigned int i=0;i<mesh->mNumVertices;i++)
    {
        Vertex v;

        v.position.x =
        mesh->mVertices[i].x;

        v.position.y =
        mesh->mVertices[i].y;

        v.position.z =
        mesh->mVertices[i].z;


        vertices.push_back(v);
    }


    for(unsigned int i=0;i<mesh->mNumFaces;i++)
    {
        aiFace face =
        mesh->mFaces[i];


        for(unsigned int j=0;j<face.mNumIndices;j++)
            indices.push_back(face.mIndices[j]);
    }


    return true;
}



struct Player
{
    glm::vec3 position;
    float speed;


    Player()
    {
        position =
        glm::vec3(0,0,0);

        speed =
        0.12f;
    }


    void update()
    {
        const Uint8* keys =
        SDL_GetKeyboardState(NULL);


        if(keys[SDL_SCANCODE_W])
            position.z-=speed;


        if(keys[SDL_SCANCODE_S])
            position.z+=speed;


        if(keys[SDL_SCANCODE_A])
            position.x-=speed;


        if(keys[SDL_SCANCODE_D])
            position.x+=speed;
    }
};



struct Ball
{
    glm::vec3 position;
    glm::vec3 velocity;


    Ball()
    {
        position =
        glm::vec3(0,0.3f,0);

        velocity =
        glm::vec3(0);
    }


    void update()
    {
        position+=velocity;

        velocity*=0.97f;


        if(position.y>0.3f)
        {
            velocity.y-=0.01f;
        }


        if(position.y<0.3f)
        {
            position.y=0.3f;
            velocity.y*=-0.5f;
        }
    }


    void kick()
    {
        velocity =
        glm::vec3(0,0,-0.4f);
    }
};



GLuint shaderProgram;


GLuint createShader()
{
    const char* vs =
    R"(
    #version 330 core

    layout(location=0) in vec3 pos;

    uniform mat4 MVP;

    void main()
    {
        gl_Position =
        MVP *
        vec4(pos,1);
    }
    )";


    const char* fs =
    R"(
    #version 330 core

    out vec4 color;

    void main()
    {
        color =
        vec4(0.1,0.8,0.1,1);
    }
    )";


    GLuint v =
    glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(v,1,&vs,nullptr);
    glCompileShader(v);



    GLuint f =
    glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(f,1,&fs,nullptr);
    glCompileShader(f);



    GLuint p =
    glCreateProgram();

    glAttachShader(p,v);
    glAttachShader(p,f);

    glLinkProgram(p);


    glDeleteShader(v);
    glDeleteShader(f);


    return p;
}



int main()
{

SDL_Init(SDL_INIT_VIDEO);


SDL_Window* window =
SDL_CreateWindow(
"SFootball 26",
SDL_WINDOWPOS_CENTERED,
SDL_WINDOWPOS_CENTERED,
WIDTH,
HEIGHT,
SDL_WINDOW_OPENGL
);



SDL_GLContext context =
SDL_GL_CreateContext(window);


#ifndef __APPLE__
glewInit();
#endif


glEnable(GL_DEPTH_TEST);


shaderProgram=createShader();



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
0
);


glEnableVertexAttribArray(0);



Player player;

Ball ball;



bool running=true;


while(running)
{

SDL_Event e;


while(SDL_PollEvent(&e))
{

if(e.type==SDL_QUIT)
running=false;


if(e.type==SDL_KEYDOWN)
{
if(e.key.keysym.sym==SDLK_SPACE)
ball.kick();
}

}



player.update();

ball.update();



if(glm::distance(
player.position,
ball.position)<1.2f)
{
ball.velocity=
glm::normalize(
ball.position-player.position)
*0.15f;
}




glClearColor(
0.2,
0.5,
0.9,
1);


glClear(
GL_COLOR_BUFFER_BIT |
GL_DEPTH_BUFFER_BIT
);



glm::mat4 projection =
glm::perspective(
glm::radians(45.0f),
(float)WIDTH/HEIGHT,
0.1f,
100.0f
);



glm::mat4 view =
glm::lookAt(
player.position+
glm::vec3(0,3,6),
player.position,
glm::vec3(0,1,0)
);



glm::mat4 model =
glm::mat4(1);



glm::mat4 MVP =
projection*view*model;



glUseProgram(shaderProgram);



glUniformMatrix4fv(
glGetUniformLocation(
shaderProgram,
"MVP"),
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



SDL_Quit();


return 0;

}
