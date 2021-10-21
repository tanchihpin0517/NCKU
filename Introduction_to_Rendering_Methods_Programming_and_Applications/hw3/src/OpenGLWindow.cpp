#include "OpenGLWindow.hpp"

#include "Model/TextureFactory.hpp"
#include "OpenGL/OpenGLException.hpp"
#include "Utils/Compilers.hpp"
#include "Utils/Global.hpp"
#include "Utils/StringFormat/StringFormat.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtx/io.hpp"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "tiny_obj_loader.h"

#include <iostream>
#include <utility>
#include <vector>

namespace Detail
{

bool compileShaders(OpenGL::OpenGLShaderProgram &program,
                    const char *vertexShaderFile,
                    const char *fragmentShaderFile = nullptr,
                    const char *geometryShaderFile = nullptr);
void frameBufferSizeCallback(GLFWwindow *window, int width, int height);

bool compileShaders(OpenGL::OpenGLShaderProgram &program,
                    const char *vertexShaderFile,
                    const char *fragmentShaderFile,
                    const char *geometryShaderFile)
{
    if (!program.addShaderFromFile(OpenGL::OpenGLShader::Type::Vertex,
                                   vertexShaderFile))
    {
        return false;
    }

    if (fragmentShaderFile &&

        !program.addShaderFromFile(OpenGL::OpenGLShader::Type::Fragment,
                                   fragmentShaderFile))
    {
        return false;
    }

    if (geometryShaderFile &&
        !program.addShaderFromFile(OpenGL::OpenGLShader::Type::Geometry,
                                   geometryShaderFile))
    {
        return false;
    }

    program.link();
    if (!program.linkStatus())
    {
        return false;
    }

    return true;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    if (window)
    {
    }
    glViewport(0, 0, width, height);
}

} // namespace Detail

OpenGLWindow* OpenGLWindow::singleton = NULL;

void OpenGLWindow::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        OpenGLWindow::singleton->onMouseLeftButtonPressed();
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        OpenGLWindow::singleton->onMouseLeftButtonReleased();
    }
}

void OpenGLWindow::cursorPosCallback(GLFWwindow* window, double x, double y)
{
    OpenGLWindow::singleton->onCursorPosChanged(x, y);
}

void OpenGLWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        OpenGLWindow::singleton->onKeyPressed(key);
    }
}

OpenGLWindow::OpenGLWindow(glm::ivec2 windowSize, std::string title,
                           glm::ivec2 openglVersion)
    : window_{nullptr}, size_{windowSize}, title_{title},
      version_{openglVersion}, models_{}, renderMode_{RenderMode::Fill},
      backgroundColor_{0}, lookAt_{0}, cameraPosition_{20, 0, 0},
      cameraTransMatrix{1}, cameraRotateRef{0, 0, 1},
      mouseLeftButtonPressed{false}, mouseRightButtonPressed{false},
      mouseLeftButtonPressedLastPos{0}, mouseRightButtonPressedLastPos{0},
      lastTimeStamp{0}, totalFrame{0}, lastFrameStamp{0},
      animationUpdateCount{0}, animationMode{Animation::None},
      animationTorsoInverse{1}, animationHeadInverse{1}, animationTailInverse{1},
      animationLegInverse{1}, animationFootInverse{1},
      AnimationTorsoPeriod{1}, AnimationTorsoDelay{1},
      AnimationHeadPeriod{1}, AnimationHeadDelay{1},
      AnimationTailPeriod{1}, AnimationTailDelay{1},
      AnimationLegPeriod{1}, AnimationLegDelay{1},
      AnimationFootPeriod{1}, AnimationFootDelay{1},
      controlTarget{Control::Camera}, controlTargetIndex{0}, controlRef{Scene::Axis::X}
{
    OpenGLWindow* old = OpenGLWindow::singleton;
    OpenGLWindow::singleton = this;

    create();

    delete old; old = NULL;
}

OpenGLWindow::~OpenGLWindow() { destroy(); }

bool OpenGLWindow::addModel(const char *modelSource, const char *textureSource,
                            OpenGL::OpenGLShaderProgram &program)
{
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string errorMessage;

    auto success =
        tinyobj::LoadObj(shapes, materials, errorMessage, modelSource);

    if (!success)
    {
        std::cerr << "[Error]" << errorMessage.c_str();

        return false;
    }

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> textureCoordinates;
    std::vector<unsigned int> indices;

    for (auto &shape : shapes)
    {
        positions.insert(positions.end(), shape.mesh.positions.begin(),
                         shape.mesh.positions.end());
        if (!shape.mesh.normals.empty())
        {
            normals.insert(normals.end(), shape.mesh.normals.begin(),
                           shape.mesh.normals.end());
        }

        if (!shape.mesh.texcoords.empty())
        {
            textureCoordinates.insert(textureCoordinates.end(),
                                      shape.mesh.texcoords.begin(),
                                      shape.mesh.texcoords.end());
        }

        indices.insert(indices.end(), shape.mesh.indices.begin(),
                       shape.mesh.indices.end());
    }

    std::unique_ptr<OpenGL::OpenGLTexture> texture;
    std::unique_ptr<Model::Mesh> mesh;

    if (textureSource)
    {
        texture = Model::TextureFactory::loadFromFile(textureSource);
        mesh.reset(new Model::Mesh{positions, normals, textureCoordinates,
                                   indices, program, texture.get()});

        textures.push_back(std::move(texture));
    }
    else
    {
        mesh.reset(new Model::Mesh{positions, normals, textureCoordinates,
                                   indices, program});
    }

    models_.push_back(std::move(mesh));

    return true;
}

bool OpenGLWindow::initModel(const char *modelSource, const char *textureSource,
                             OpenGL::OpenGLShaderProgram &program)
{
    glm::mat4 trans;
    unsigned int index = 0;

    for (int i = 0; i < 11+3+4; i++)
    {
        if (!addModel(modelSource, textureSource, program))
        {
            return false;
        }
    }

    // torso
    trans = glm::mat4(1.0f);
    trans = glm::scale(trans, glm::vec3(3.0, -2.0, 2.0));
    models_[index]->setModel(trans);
    torso = new Scene::Component(models_[index++].get(), glm::vec3(0.0, 0.0, 0.0));
    sceneGraphRoot = torso;
    // head
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(4.5, 0.0, 0.0));
    trans = glm::scale(trans, glm::vec3(1.5, 1.5, 1.5));
    models_[index]->setModel(trans);
    head = new Scene::Component(models_[index++].get(), glm::vec3(4.5, 0.0, 0.0));
    torso->addChild(head);
    // nose
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(6.25, -0.5, 0.0));
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 1.0));
    models_[index]->setModel(trans);
    nose = new Scene::Component(models_[index++].get(), glm::vec3(6.0, 0.0, 0.0));
    head->addChild(nose);
    // horn_l
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(5.0, 2.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.25, 0.5, 0.25));
    models_[index]->setModel(trans);
    horn_l = new Scene::Component(models_[index++].get(), glm::vec3(5.0, 1.5, 1.0));
    head->addChild(horn_l);
    // horn_r
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(5.0, 2.0, -1.0));
    trans = glm::scale(trans, glm::vec3(0.25, 0.5, 0.25));
    models_[index]->setModel(trans);
    horn_r = new Scene::Component(models_[index++].get(), glm::vec3(5.0, 1.5, 1.0));
    head->addChild(horn_r);
    // tail
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-4.5, 1.0, 0.0));
    trans = glm::scale(trans, glm::vec3(1.5, 0.5, 0.5));
    models_[index]->setModel(trans);
    tail = new Scene::Component(models_[index++].get(), glm::vec3(-3.0, 1.0, 0.0));
    torso->addChild(tail);
    // leg_lf
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.5, -3.0, -1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 1.0, 0.5));
    models_[index]->setModel(trans);
    leg_lf = new Scene::Component(models_[index++].get(), glm::vec3(1.5, -2.0, -1.0));
    torso->addChild(leg_lf);
    // foot_lf
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.5, -5.0, -1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 1.0, 0.5));
    models_[index]->setModel(trans);
    foot_lf = new Scene::Component(models_[index++].get(), glm::vec3(1.5, -4.0, -1.0));
    leg_lf->addChild(foot_lf);

    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.75, -6.0, -1.0));
    trans = glm::scale(trans, glm::vec3(0.75, 0.125, 0.625));
    models_[index]->setModel(trans);
    foot_lf->addChild(new Scene::Component(models_[index++].get(), glm::vec3(1.75, -6.0, -1.0)));
    // leg_rf
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.5, -3.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 1.0, 0.5));
    models_[index]->setModel(trans);
    leg_rf = new Scene::Component(models_[index++].get(), glm::vec3(1.5, -2.0, 1.0));
    torso->addChild(leg_rf);
    // foot_rf
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.5, -5.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 1.0, 0.5));
    models_[index]->setModel(trans);
    foot_rf = new Scene::Component(models_[index++].get(), glm::vec3(1.5, -4.0, 1.0));
    leg_rf->addChild(foot_rf);

    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.75, -6.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.75, 0.125, 0.625));
    models_[index]->setModel(trans);
    foot_rf->addChild(new Scene::Component(models_[index++].get(), glm::vec3(1.75, -6.0, 1.0)));
    // leg_lb
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-1.5, -3.0, -1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 1.0, 0.5));
    models_[index]->setModel(trans);
    leg_lb = new Scene::Component(models_[index++].get(), glm::vec3(-1.5, -2.0, -1.0));
    torso->addChild(leg_lb);
    // foot_lb
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-1.5, -5.0, -1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 1.0, 0.5));
    models_[index]->setModel(trans);
    foot_lb = new Scene::Component(models_[index++].get(), glm::vec3(-1.5, -4.0, -1.0));
    leg_lb->addChild(foot_lb);

    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-1.25, -6.0, -1.0));
    trans = glm::scale(trans, glm::vec3(0.75, 0.125, 0.625));
    models_[index]->setModel(trans);
    foot_lb->addChild(new Scene::Component(models_[index++].get(), glm::vec3(1.25, -6.0, -1.0)));
    // leg_rb
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-1.5, -3.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 1.0, 0.5));
    models_[index]->setModel(trans);
    leg_rb = new Scene::Component(models_[index++].get(), glm::vec3(-1.5, -2.0, 1.0));
    torso->addChild(leg_rb);
    // foot_rb
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-1.5, -5.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 1.0, 0.5));
    models_[index]->setModel(trans);
    foot_rb = new Scene::Component(models_[index++].get(), glm::vec3(-1.5, -4.0, 1.0));
    leg_rb->addChild(foot_rb);

    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-1.25, -6.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.75, 0.125, 0.625));
    models_[index]->setModel(trans);
    foot_rb->addChild(new Scene::Component(models_[index++].get(), glm::vec3(1.25, -6.0, 1.0)));

    return true;
}

OpenGL::OpenGLShaderProgram *
OpenGLWindow::addShader(const char *vertexShaderSource,
                        const char *fragmentShaderSource,
                        const char *geometryShaderSource)
{
    std::unique_ptr<OpenGL::OpenGLShaderProgram> program{
        new OpenGL::OpenGLShaderProgram{}};
    if (!Detail::compileShaders(*program, vertexShaderSource,
                                fragmentShaderSource, geometryShaderSource))
    {
        return nullptr;
    }
    shaders_.push_back(std::move(program));

    return shaders_.back().get();
}

float OpenGLWindow::aspectRatio() const noexcept
{
    return static_cast<float>(width()) / static_cast<float>(height());
}

void OpenGLWindow::create()
{
    if (!initializeOpenGL())
    {
        throw OpenGL::OpenGLException{"Failed to initialize OpenGL"};
    }

    if (!createWindow())
    {
        glfwTerminate();
        throw OpenGL::OpenGLException{"Failed to Create GLFW window"};
    }

    if (!initializeGLAD())
    {
        glfwTerminate();
        throw OpenGL::OpenGLException{"Failed to initialize GLAD"};
    }

    initializeImgui();

    glEnable(GL_DEPTH_TEST);
    glfwSetTime(0.0);
}

void OpenGLWindow::reset()
{
    if (sceneGraphRoot != NULL)
    {
        sceneGraphRoot->reset();
    }

    cameraPosition_ = glm::vec3(20, 0, 0);
    cameraNormal_ = glm::vec3(0, 1, 0);
    cameraRotateRef = glm::vec3(0, 0, 1);
    cameraTransMatrix = glm::mat4(1);
    cameraAngle = glm::vec2(0, 0);
    cameraRotate(-45, 15);

    animationMode = Animation::None;

    animationUpdateCount = 0;
    animationTorsoInverse = 1;
    animationHeadInverse = 1;
    animationTailInverse = 1;
    animationLegInverse = 1;
    animationFootInverse = 1;
}

void OpenGLWindow::clearColor()
{
    glClearColor(backgroundColor_.x, backgroundColor_.y, backgroundColor_.z,
                 backgroundColor_.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool OpenGLWindow::createWindow()
{
    window_ =
        glfwCreateWindow(width(), height(), title_.c_str(), nullptr, nullptr);
    if (!window_)
    {
        return false;
    }
    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, Detail::frameBufferSizeCallback);
    glfwSwapInterval(0);

    // event callback
    glfwSetMouseButtonCallback(window_, OpenGLWindow::mouseButtonCallback);
    glfwSetCursorPosCallback(window_, OpenGLWindow::cursorPosCallback);
    glfwSetKeyCallback(window_, OpenGLWindow::keyCallback);

    return true;
}

void OpenGLWindow::destroy()
{
    for (auto &model : models_)
    {
        model.reset(nullptr);
    }
    models_.clear();

    for (auto &texture : textures)
    {
        texture.reset(nullptr);
    }
    textures.clear();

    shaders_.clear();

    destroyImgui();
    destroyOpenGL();
}

void OpenGLWindow::destroyImgui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void OpenGLWindow::destroyOpenGL()
{
    glfwDestroyWindow(window_);
    glfwTerminate();
}

int OpenGLWindow::height() const noexcept { return size_.y; }

bool OpenGLWindow::initializeGLAD()
{
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void OpenGLWindow::initializeImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    PROGRAM_MAYBE_UNUSED(io)

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init(
        StringFormat::StringFormat("#version %d%d0", version_.x, version_.y)
            .c_str());
}

bool OpenGLWindow::initializeOpenGL()
{
    if (!glfwInit())
    {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version_[0]);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version_[1]);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    return true;
}

void OpenGLWindow::onMouseLeftButtonPressed()
{
    double x, y;

    glfwGetCursorPos(window_, &x, &y);
    mouseLeftButtonPressed = true;
    mouseLeftButtonPressedLastPos[0] = x;
    mouseLeftButtonPressedLastPos[1] = y;
}

void OpenGLWindow::onMouseLeftButtonReleased()
{
    mouseLeftButtonPressed = false;
}

void OpenGLWindow::onCursorPosChanged(double x, double y)
{
    if (mouseLeftButtonPressed)
    {
        double diff_x = x - mouseLeftButtonPressedLastPos[0];
        double diff_y = y - mouseLeftButtonPressedLastPos[1];
        mouseLeftButtonPressedLastPos[0] = x;
        mouseLeftButtonPressedLastPos[1] = y;

        if (controlTarget == Control::Camera)
        {
            cameraRotate(-diff_x, diff_y);
        }
        else
        {
            controlObject->rotate(diff_x, controlRef);
        }
    }
}

void OpenGLWindow::onKeyPressed(int key)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        break;
    case GLFW_KEY_Q:
        onKeyPressedQ();
        break;
    case GLFW_KEY_R:
        onKeyPressedR();
        break;
    case GLFW_KEY_D:
        onKeyPressedD();
        break;
    case GLFW_KEY_W:
        onKeyPressedW();
        break;
    case GLFW_KEY_C: // camera
        onKeyPressedC();
        break;
    case GLFW_KEY_B: // body (torso)
        onKeyPressedB();
        break;
    case GLFW_KEY_H: // head
        onKeyPressedH();
        break;
    case GLFW_KEY_T: // tail
        onKeyPressedT();
        break;
    case GLFW_KEY_L: // leg
        onKeyPressedL();
        break;
    case GLFW_KEY_F: // foot
        onKeyPressedF();
        break;
    case GLFW_KEY_1: // 1
        onKeyPressed1();
        break;
    case GLFW_KEY_2: // 2
        onKeyPressed2();
        break;
    case GLFW_KEY_3: // 3
        onKeyPressed3();
        break;
    case GLFW_KEY_4: // 4
        onKeyPressed4();
        break;
    case GLFW_KEY_X: // x
        onKeyPressedX();
        break;
    case GLFW_KEY_Y: // y
        onKeyPressedY();
        break;
    case GLFW_KEY_Z: // z
        onKeyPressedZ();
        break;
    default:
        std::cout << "Unknown key: " << key << std::endl;
        break;
    }
}

void OpenGLWindow::onKeyPressedQ()
{
    glfwSetWindowShouldClose(window_, true);
}

void OpenGLWindow::onKeyPressedR()
{
    reset();
}

void OpenGLWindow::onKeyPressedD()
{
    reset();
    setupAnimationDance();
}

void OpenGLWindow::onKeyPressedW()
{
    reset();
    setupAnimationWalk();
}

void OpenGLWindow::onKeyPressedC()
{
    controlTarget = Control::Camera;
    updateControlObject();
}

void OpenGLWindow::onKeyPressedB()
{
    controlTarget = Control::Torso;
    updateControlObject();
}

void OpenGLWindow::onKeyPressedH()
{
    controlTarget = Control::Head;
    updateControlObject();
}

void OpenGLWindow::onKeyPressedT()
{
    controlTarget = Control::Tail;
    updateControlObject();
}

void OpenGLWindow::onKeyPressedL()
{
    controlTarget = Control::Leg;
    updateControlObject();
}

void OpenGLWindow::onKeyPressedF()
{
    controlTarget = Control::Foot;
    updateControlObject();
}

void OpenGLWindow::onKeyPressed1()
{
    controlTargetIndex = 0;
    updateControlObject();
}

void OpenGLWindow::onKeyPressed2()
{
    controlTargetIndex = 1;
    updateControlObject();
}

void OpenGLWindow::onKeyPressed3()
{
    controlTargetIndex = 2;
    updateControlObject();
}

void OpenGLWindow::onKeyPressed4()
{
    controlTargetIndex = 3;
    updateControlObject();
}

void OpenGLWindow::onKeyPressedX()
{
    controlRef = Scene::Axis::X;
}

void OpenGLWindow::onKeyPressedY()
{
    controlRef = Scene::Axis::Y;
}

void OpenGLWindow::onKeyPressedZ()
{
    controlRef = Scene::Axis::Z;
}

void OpenGLWindow::updateControlObject()
{
    switch (controlTarget)
    {
    case Control::Camera:
        controlObject = NULL;
        break;
    case Control::Torso:
        controlObject = torso;
        break;
    case Control::Head:
        controlObject = head;
        break;
    case Control::Tail:
        controlObject = tail;
        break;
    case Control::Leg:
        if (controlTargetIndex == 0)
            controlObject = leg_lf;
        else if (controlTargetIndex == 1)
            controlObject = leg_rf;
        else if (controlTargetIndex == 2)
            controlObject = leg_lb;
        else if (controlTargetIndex == 3)
            controlObject = leg_rb;
        break;
    case Control::Foot:
        if (controlTargetIndex == 0)
            controlObject = foot_lf;
        else if (controlTargetIndex == 1)
            controlObject = foot_rf;
        else if (controlTargetIndex == 2)
            controlObject = foot_lb;
        else if (controlTargetIndex == 3)
            controlObject = foot_rb;
        break;
    default:
        std::cout << "Unknown control target: " << controlTarget << std::endl;
        break;
    }
}

void OpenGLWindow::processInput()
{
    shouldExit();
}

void OpenGLWindow::shouldExit()
{
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window_, true);
    }
}

void OpenGLWindow::startRender() { windowRenderLoop(); }

int OpenGLWindow::width() const noexcept { return size_.x; }

void OpenGLWindow::windowImguiGeneralSetting()
{
    ImGui::Begin("Setting");
    ImGui::ColorEdit4("Background color", glm::value_ptr(backgroundColor_));
    ImGui::SliderFloat3("Camera Position", glm::value_ptr(cameraPosition_),
                        -20.0f, 20.0f);
    ImGui::SliderFloat3("Look At", glm::value_ptr(lookAt_), -20.0f, 20.0f);

    const char *items[] = {"Line", "Fill"};
    int current_item = renderMode_;
    ImGui::Combo("combo", &current_item, items, IM_ARRAYSIZE(items));

    if (current_item != renderMode_)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE + current_item);
        renderMode_ = static_cast<RenderMode>(current_item);
    }

    ImGui::End();
}

void OpenGLWindow::windowRenderLateUpdate()
{
    timeEventUpdate();
}

void OpenGLWindow::timeEventUpdate()
{
    double time_sec = glfwGetTime();
    if (time_sec - lastTimeStamp > AnimationUpdateInterval)
    {
        updateAnimation();
        //std::cout << time_sec << "," << (totalFrame - lastFrameStamp) << std::endl;
        lastTimeStamp += AnimationUpdateInterval;
        lastFrameStamp = totalFrame;
    }
    totalFrame += 1;
}

void OpenGLWindow::updateAnimation()
{
    switch (animationMode)
    {
    case Animation::Dance:
        torso->translate(animationTorsoInverse * glm::vec3(0, 0.1, 0));
        head->rotate(animationHeadInverse * 5, Scene::Axis::X);
        tail->rotate(animationTailInverse * 5, Scene::Axis::Z);
        leg_lf->rotate(animationLegInverse *  2.5, Scene::Axis::X);
        leg_rf->rotate(animationLegInverse * -2.5, Scene::Axis::X);
        leg_lb->rotate(animationLegInverse *  2.5, Scene::Axis::X);
        leg_rb->rotate(animationLegInverse * -2.5, Scene::Axis::X);
        foot_lf->rotate(animationFootInverse *  5, Scene::Axis::X);
        foot_rf->rotate(animationFootInverse * -5, Scene::Axis::X);
        foot_lb->rotate(animationFootInverse *  5, Scene::Axis::X);
        foot_rb->rotate(animationFootInverse * -5, Scene::Axis::X);
        break;
    case Animation::Walk:
        //torso->translate(animationTorsoInverse * glm::vec3(0, 0.05, 0));
        torso->rotate(animationTorsoInverse * 0.25, Scene::Axis::Y);
        head->rotate(animationHeadInverse * 2.5, Scene::Axis::Y);
        tail->rotate(animationTailInverse * 5, Scene::Axis::Y);
        leg_lf->rotate(animationLegInverse *  2.5, Scene::Axis::Z);
        leg_rf->rotate(animationLegInverse * -2.5, Scene::Axis::Z);
        leg_lb->rotate(animationLegInverse *  2.5, Scene::Axis::Z);
        leg_rb->rotate(animationLegInverse * -2.5, Scene::Axis::Z);
        foot_lf->rotate(animationFootInverse *  5, Scene::Axis::Z);
        foot_rf->rotate(animationFootInverse * -5, Scene::Axis::Z);
        foot_lb->rotate(animationFootInverse *  5, Scene::Axis::Z);
        foot_rb->rotate(animationFootInverse * -5, Scene::Axis::Z);
        break;
    default:
        break;
    }

    animationUpdateCount += 1;
    if ((animationUpdateCount + AnimationTorsoDelay) % AnimationTorsoPeriod == 0)
        animationTorsoInverse *= -1;
    if ((animationUpdateCount + AnimationHeadDelay) % AnimationHeadPeriod == 0)
        animationHeadInverse *= -1;
    if ((animationUpdateCount + AnimationTailDelay) % AnimationTailPeriod == 0)
        animationTailInverse *= -1;
    if ((animationUpdateCount + AnimationLegDelay) % AnimationLegPeriod == 0)
        animationLegInverse *= -1;
    if ((animationUpdateCount + AnimationFootDelay) % AnimationFootPeriod == 0)
        animationFootInverse *= -1;
}

void OpenGLWindow::setupAnimationDance()
{
    animationMode = Animation::Dance;

    AnimationTorsoPeriod = (int)(0.5 / AnimationUpdateInterval / 2);
    AnimationTorsoDelay = (int)(0 / AnimationUpdateInterval / 2);

    AnimationHeadPeriod = (int)(0.25 / AnimationUpdateInterval / 2);
    AnimationHeadDelay = (int)(0.125 / AnimationUpdateInterval / 2);

    AnimationTailPeriod = (int)(0.5 / AnimationUpdateInterval / 2);
    AnimationTailDelay = (int)(0.25 / AnimationUpdateInterval / 2);

    AnimationLegPeriod = (int)(0.5 / AnimationUpdateInterval / 2);
    AnimationLegDelay = (int)(0.1 / AnimationUpdateInterval / 2);

    AnimationFootPeriod = (int)(0.5 / AnimationUpdateInterval / 2);
    AnimationFootDelay = (int)(0 / AnimationUpdateInterval / 2);
}

void OpenGLWindow::setupAnimationWalk()
{
    animationMode = Animation::Walk;

    AnimationTorsoPeriod = (int)(1 / AnimationUpdateInterval / 2);
    AnimationTorsoDelay = (int)(0 / AnimationUpdateInterval / 2);

    AnimationHeadPeriod = (int)(0.5 / AnimationUpdateInterval / 2);
    AnimationHeadDelay = (int)(0.25 / AnimationUpdateInterval / 2);

    AnimationTailPeriod = (int)(0.5 / AnimationUpdateInterval / 2);
    AnimationTailDelay = (int)(0.25 / AnimationUpdateInterval / 2);

    AnimationLegPeriod = (int)(0.5 / AnimationUpdateInterval / 2);
    AnimationLegDelay = (int)(0.25 / AnimationUpdateInterval / 2);

    AnimationFootPeriod = (int)(0.5 / AnimationUpdateInterval / 2);
    AnimationFootDelay = (int)(0.25 / AnimationUpdateInterval / 2);
}

void OpenGLWindow::cameraRotate(float deg_x, float deg_y)
{
    if (cameraAngle[1] + deg_y > 90) {
        deg_y = 90 - cameraAngle[1];
    }
    if (cameraAngle[1] + deg_y < -90) {
        deg_y = -90 - cameraAngle[1];
    }

    glm::mat4 RX = glm::rotate(glm::mat4(1), (float)glm::radians(deg_x),
                               glm::vec3(0, 1, 0));
    cameraRotateRef = glm::vec3(RX * glm::vec4(cameraRotateRef, 1));
    glm::mat4 RY = glm::rotate(glm::mat4(1), (float)glm::radians(deg_y),
                               cameraRotateRef);
    cameraNormal_ = glm::vec3(RY * RX * glm::vec4(cameraNormal_, 1));
    cameraTransMatrix = RY * RX * cameraTransMatrix;

    cameraAngle[0] += deg_x;
    cameraAngle[1] += deg_y;
}

void OpenGLWindow::windowRenderImguiUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    windowImguiGeneralSetting();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGLWindow::windowRenderLoop()
{
    while (!(glfwWindowShouldClose(window_)))
    {
        processInput();
        clearColor();

        windowRenderUpdate();
        windowRenderLateUpdate();

        //windowRenderImguiUpdate();

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

void OpenGLWindow::windowRenderUpdate()
{
    PRAGMA_WARNING_PUSH
    PRAGMA_WARNING_DISABLE_CONSTANTCONDITIONAL
    glm::vec3 cameraTransPosition = glm::vec3(
            cameraTransMatrix * glm::vec4(cameraPosition_, 1));
    glm::mat4 view = glm::lookAt(cameraTransPosition, lookAt_, cameraNormal_) *
                     glm::mat4(1);
    PRAGMA_WARNING_POP

    glm::mat4 projection{
        glm::perspective(glm::radians(45.0f), aspectRatio(), 0.1f, 100.0f)};

    /*
    for (auto &model : models_)
    {
        model->draw(view, projection);
    }
    */
    sceneGraphRoot->draw(view, projection);
}
