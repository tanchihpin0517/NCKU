#ifndef SAMPLECODE_WINDOW_HPP_
#define SAMPLECODE_WINDOW_HPP_

#include "Model/Mesh.hpp"
#include "OpenGL/OpenGLShaderProgram.hpp"
#include "OpenGL/OpenGLTexture.hpp"
#include "Scene/Component.hpp"

#include "glad/glad.h"

#include "GLFW/glfw3.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <memory>
#include <string>

class OpenGLWindow
{
private:
    enum RenderMode
    {
        Line = 0,
        Fill = 1
    };
    enum Animation
    {
        None,
        Dance,
        Walk
    };
    enum Control
    {
        Camera,
        Torso, Head, Tail,
        Leg, Foot
    };
    const double AnimationUpdateInterval = 1.0/60;
    int AnimationTorsoPeriod;
    int AnimationTorsoDelay;
    int AnimationHeadPeriod;
    int AnimationHeadDelay;
    int AnimationTailPeriod;
    int AnimationTailDelay;
    int AnimationLegPeriod;
    int AnimationLegDelay;
    int AnimationFootPeriod;
    int AnimationFootDelay;

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double x, double y);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
    static OpenGLWindow *singleton;

    explicit OpenGLWindow(glm::ivec2 windowSize, std::string title,
                          glm::ivec2 openglVersion);
    ~OpenGLWindow();

    OpenGLWindow(OpenGLWindow &&other) = delete;
    OpenGLWindow &operator=(OpenGLWindow &&other) = delete;
    OpenGLWindow(const OpenGLWindow &other) = delete;
    OpenGLWindow &operator=(const OpenGLWindow &other) = delete;

    void create();
    void reset();
    void startRender();

    bool addModel(const char *modelSource, const char *textureSource,
                  OpenGL::OpenGLShaderProgram &program);
    OpenGL::OpenGLShaderProgram *
    addShader(const char *vertexShaderSource, const char *fragmentShaderSource,
              const char *geometryShaderSource = nullptr);

    bool initModel(const char *modelSource, const char *textureSource,
                  OpenGL::OpenGLShaderProgram &program);

    void onMouseLeftButtonPressed();
    void onMouseLeftButtonReleased();
    void onCursorPosChanged(double x, double y);
    void onKeyPressed(int key);

private:
    bool createWindow();
    bool initializeGLAD();
    void initializeImgui();
    bool initializeOpenGL();

    void destroy();
    void destroyGLAD();
    void destroyImgui();
    void destroyOpenGL();

    void windowRenderLoop();
    void windowRenderUpdate();
    void windowRenderLateUpdate();
    void windowRenderImguiUpdate();

    void windowImguiGeneralSetting();

    void clearColor();

    void processInput();
    void shouldExit();
    void shouldShowPolygonMode();

    void onKeyPressedQ();
    void onKeyPressedR();
    void onKeyPressedD();
    void onKeyPressedW();
    void onKeyPressedC();
    void onKeyPressedB();
    void onKeyPressedH();
    void onKeyPressedT();
    void onKeyPressedL();
    void onKeyPressedF();
    void onKeyPressed1();
    void onKeyPressed2();
    void onKeyPressed3();
    void onKeyPressed4();
    void onKeyPressedX();
    void onKeyPressedY();
    void onKeyPressedZ();

    void updateControlObject();

    void timeEventUpdate();
    void updateAnimation();
    void setupAnimationDance();
    void setupAnimationWalk();
    void cameraRotate(float x, float y);

    float aspectRatio() const noexcept;
    int height() const noexcept;
    int width() const noexcept;

    GLFWwindow *window_;

    glm::ivec2 size_;
    std::string title_;
    glm::ivec2 version_;

    std::vector<std::unique_ptr<Model::Mesh>> models_;
    std::vector<std::unique_ptr<OpenGL::OpenGLTexture>> textures;
    std::vector<std::unique_ptr<OpenGL::OpenGLShaderProgram>> shaders_;

    RenderMode renderMode_;

    glm::vec4 backgroundColor_;

    glm::vec3 lookAt_;
    glm::vec3 cameraPosition_;
    glm::vec3 cameraNormal_;
    glm::vec3 cameraRotateRef;
    glm::mat4 cameraTransMatrix;
    glm::vec2 cameraAngle;

    bool mouseLeftButtonPressed;
    bool mouseRightButtonPressed;
    glm::vec2 mouseLeftButtonPressedLastPos;
    glm::vec2 mouseRightButtonPressedLastPos;

    Scene::Component *sceneGraphRoot;
    Scene::Component *torso, *head, *nose, *horn_l, *horn_r, *tail;
    Scene::Component *leg_lf, *leg_rf, *leg_lb, *leg_rb;
    Scene::Component *foot_lf, *foot_rf, *foot_lb, *foot_rb;

    double lastTimeStamp;
    unsigned int totalFrame;
    unsigned int lastFrameStamp;

    Animation animationMode;
    Control controlTarget;
    Scene::Component *controlObject;
    int controlTargetIndex;
    Scene::Axis controlRef;

    unsigned int animationUpdateCount;
    float animationTorsoInverse;
    float animationHeadInverse;
    float animationTailInverse;
    float animationLegInverse;
    float animationFootInverse;
};

#endif // SAMPLECODE_WINDOW_HPP_
