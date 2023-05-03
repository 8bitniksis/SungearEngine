#include "Window.h"

void Core::Main::Window::create()
{
    glfwSetErrorCallback(errorCallback);

    if(!glfwInit())
    {
        Logging::consolePrintf(Logging::SG_INFO, "Failed to initialize GLFW!");
    }

    Logging::consolePrintf(Logging::SG_INFO, "GLFW version is %i.%i.%i", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    glfwDefaultWindowHints(); // установка для будущего окна дефолтных настроек
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    m_handler = glfwCreateWindow(this->m_config.m_sizeX, this->m_config.m_sizeY, this->m_config.m_name.c_str(), nullptr, nullptr);

    if(!m_handler)
    {
        Logging::consolePrintf(Logging::SG_INFO, "Failed to initialize GLFW Window!");
    }

    glfwSetWindowCloseCallback(m_handler, windowCloseCallback);
    glfwSetWindowIconifyCallback(m_handler, windowIconifyCallback);
    glfwSetKeyCallback(m_handler, InputManager::keyboardKeyCallback);
    glfwSetMouseButtonCallback(m_handler, InputManager::mouseButtonCallback);

    glfwMakeContextCurrent(m_handler);

    glfwSetWindowPos(m_handler, this->m_config.m_positionX, this->m_config.m_positionY);
    glfwSetWindowSizeLimits(m_handler, this->m_config.m_sizeMinLimitX, this->m_config.m_sizeMinLimitY, this->m_config.m_sizeMaxLimitX, this->m_config.m_sizeMaxLimitY);
    glfwSetWindowUserPointer(m_handler, this);

    glfwShowWindow(m_handler);

    glfwSwapInterval(this->m_config.m_swapInterval);

    glfwSetInputMode(m_handler, GLFW_STICKY_KEYS, this->m_config.m_enableStickyKeys);
    glfwSetInputMode(m_handler, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
}

void Core::Main::Window::proceedFrame()
{
    glfwSwapBuffers(m_handler);
    glfwPollEvents();
}

void Core::Main::Window::setSize(const int& sizeX, const int& sizeY) noexcept
{
    m_config.m_sizeX = sizeX;
    m_config.m_sizeY = sizeY;

    glfwSetWindowSize(m_handler, sizeX, sizeY);
}

void Core::Main::Window::setPosition(const int& posX, const int& posY) noexcept
{
    m_config.m_positionX = posX;
    m_config.m_positionY = posY;

    glfwSetWindowSize(m_handler, posX, posY);
}

void Core::Main::Window::setSizeLimits(const int& sizeMinLimitX, const int& sizeMinLimitY, const int& sizeMaxLimitX, const int& sizeMaxLimitY) noexcept
{
    m_config.m_sizeMinLimitX = sizeMinLimitX;
    m_config.m_sizeMinLimitY = sizeMinLimitY;

    m_config.m_sizeMaxLimitX = sizeMaxLimitX;
    m_config.m_sizeMaxLimitY = sizeMaxLimitY;

    glfwSetWindowSizeLimits(m_handler, sizeMinLimitX, sizeMinLimitY, sizeMaxLimitX, sizeMaxLimitY);
}

void Core::Main::Window::setSwapInterval(const bool& swapInterval) noexcept
{
    m_config.m_swapInterval = swapInterval;

    glfwSwapInterval(swapInterval);
}

void Core::Main::Window::setEnableStickyKeys(const bool& enableStickyKeys) noexcept
{
    m_config.m_enableStickyKeys = enableStickyKeys;

    glfwSetInputMode(m_handler, GLFW_STICKY_KEYS, enableStickyKeys);
}

inline void Core::Main::Window::getPrimaryMonitorSize(int& sizeX, int& sizeY) noexcept
{
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    glfwGetMonitorPhysicalSize(primaryMonitor, &sizeX, &sizeY);
}

void Core::Main::Window::windowCloseCallback(GLFWwindow* window)
{
    sgCallWindowCloseCallback(window);

    Logging::consolePrintf(Logging::MessageType::SG_INFO, "GLFW window closed.");
}

void Core::Main::Window::windowIconifyCallback(GLFWwindow* window, int iconified)
{
    sgCallWindowIconifyCallback(window, iconified);

    Logging::consolePrintf(Logging::MessageType::SG_INFO, "GLFW window iconified.");
}

void Core::Main::Window::errorCallback(int errCode, const char* err_msg)
{
    Logging::consolePrintf(Logging::SG_ERROR, "GLFW error (code %i): %s", errCode, err_msg);
}