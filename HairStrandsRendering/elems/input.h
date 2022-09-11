#pragma once
#include <GLFW/glfw3.h>

enum class InputButton
{
    Left = 0,
    Right = 1,
    Middle = 2,
    None = 9
};

class WinInput
{
public:
    static InputButton GetPressedButton(GLFWwindow* window)
    {
        InputButton result = InputButton::None;
  
        if (glfwGetMouseButton(window, 0) == GLFW_PRESS)
            return InputButton::Left;
        else if (glfwGetMouseButton(window, 1) == GLFW_PRESS)
            return InputButton::Right;
        else if (glfwGetMouseButton(window, 2) == GLFW_PRESS)
            return InputButton::Middle;
        
        return InputButton::None;
    }
};
