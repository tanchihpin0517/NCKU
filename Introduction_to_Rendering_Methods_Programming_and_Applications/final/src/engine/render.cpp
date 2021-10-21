#include "engine/render.hpp"

#include "opengl/shader.hpp"
#include "scene/scene.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

namespace Engine
{

RenderModule::RenderModule(std::array<int, 2> &openglVersion,
                           std::array<int, 2> &windowSize,
                           std::string &windowTitle)
{
    if (!initializeContext(openglVersion,
                           windowSize, windowTitle))
    {
        std::cerr << "[ERROR] Failed to initialize RenderModule context" << std::endl;
        exit(EXIT_FAILURE);
    }

    shader_.reset(new OpenGL::Shader("shader/BasicVertexShader.vs.glsl",
                                     "shader/BasicFragmentShader.fs.glsl"));
    depthShader_.reset(new OpenGL::Shader("shader/depth.vs.glsl",
                                          "shader/depth.fs.glsl"));
    debugShader_.reset(new OpenGL::Shader("shader/debug.vs.glsl",
                                          "shader/debug.fs.glsl"));
    
    shader_->setInt("objectTexture", 0);
    shader_->setInt("depthMap", 1);
    debugShader_->use();
    debugShader_->setInt("depthMap", 0);

    glGenFramebuffers(1, &depthMapFBO_);
    // create depth texture
    glGenTextures(1, &depthMap_);
    glBindTexture(GL_TEXTURE_2D, depthMap_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Light light{glm::vec3(20,20,20), glm::vec3(0,0,1), glm::vec3(1.0,1.0,1.0)};
    lights_.push_back(light);
}

bool RenderModule::initializeContext(std::array<int, 2> &openglVersion,
                                     std::array<int, 2> &windowSize,
                                     std::string &windowTitle)
{
    // initializeOpenGL
    if (!glfwInit())
    {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglVersion[0]);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglVersion[1]);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // createWindow
    window_.reset(OpenGL::Window::create(windowSize, windowTitle));
    if (!window_)
    {
        return false;
    }

    // initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return false;
    }

    // enable some functionalities
    glEnable(GL_DEPTH_TEST);

    return true;
}

void RenderModule::loop(std::shared_ptr<Scene::Scene> &scene)
{
    while (window_->updateFrame())
    {
        // render depth of scene to texture (from light's perspective)
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane_, far_plane_);
        glm::mat4 lightView = glm::lookAt(lights_[0].position, glm::vec3(0.0f), lights_[0].normal);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        depthShader_->use();
        depthShader_->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
        glClear(GL_DEPTH_BUFFER_BIT);
        for (auto &object : scene->objects())
        {
            depthShader_->setMat4("model", object->model());

            object->bind();
            glDrawElements(GL_TRIANGLES, object->indicesCount(), GL_UNSIGNED_INT, 0);
            object->release();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // render scene as normal using the generated depth/shadow map
        glViewport(0, 0, window_->width(), window_->height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 cameraPosition_{20, 20, 10};
        // glm::vec3 cameraPosition_{0, 20, 0};
        glm::vec3 lookAt_{0};
        glm::vec3 cameraNormal_{0, 0, 1};

        glm::mat4 view = 
                glm::lookAt(cameraPosition_, lookAt_, cameraNormal_) * glm::mat4(1);
        glm::mat4 projection{
            glm::perspective(glm::radians(45.0f), window_->aspectRatio(), near_plane_, far_plane_)};

        shader_->use();
        shader_->setMat4("view", view);
        shader_->setMat4("projection", projection);

        shader_->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader_->setVec3("lightPos", lights_[0].position);
        shader_->setVec3("viewPos", cameraPosition_);
        shader_->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap_);

        for (auto &object : scene->objects())
        {
            glm::mat4 mvp{projection * view * object->model()};
            shader_->setMat4("model", object->model());

            object->bind();
            glDrawElements(GL_TRIANGLES, object->indicesCount(), GL_UNSIGNED_INT, 0);
            object->release();
        }

        debugShader_->use();
        debugShader_->setFloat("near_plane", near_plane_);
        debugShader_->setFloat("far_plane", far_plane_);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap_);
        // renderQuad();
    }
}

} // Engine


