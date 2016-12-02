#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    FRONT,
    BACK
};

// Default camera values
const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  3.0f;
const GLfloat SENSITIVTY =  0.9f;
const GLfloat ZOOM       =  40.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Eular Angles
    GLfloat Yaw;
    GLfloat Pitch;
    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;
    
    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->Position = position;
        this->WorldUp = up;
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
        this->Front = glm::vec3(-0.0306749, -0.837354, -0.5458);
        this->Right = glm::vec3(0.998424, 0, -0.0561131);
        this->Up = glm::vec3(-0.0469865, 0.546662, -0.836034);
        this->Position = glm::vec3(1.86539, 24.2274, 4.38784);
        /**
         Front-0.197481 -0.425782 -0.883012
         Right0.975892 0 -0.218253
         Up-0.0929283 0.904826 -0.415518
         Position4.79858 7.19151 3.84198

         Front-0.163805 -0.775287 -0.609998
         Right0.965784 0 -0.259346
         Up-0.201067 0.631609 -0.74876
         
         
         Position1.50197 8.19151 0.944337
         Front-0.0592109 -0.669549 -0.740405
         Right0.996818 0 -0.0797165
         Up-0.053374 0.742768 -0.667418
         
         Position3.05157 16.5589 3.91323
         Front-0.0619703 -0.789572 -0.610521
         Right0.994888 0 -0.100985
         Up-0.0797349 0.613658 -0.785535
         
         Position1.87205 23.8205 7.74261
         Front-0.0417703 -0.760446 -0.648056
         Right0.997929 0 -0.0643212
         Up-0.0489128 0.649401 -0.758871
         
         Position1.86539 24.2274 4.38784
         Front-0.0306749 -0.837354 -0.5458
         Right0.998424 0 -0.0561131
         Up-0.0469865 0.546662 -0.836034
         */
    }
    // Constructor with scalar values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->Position = glm::vec3(posX, posY, posZ);
        this->WorldUp = glm::vec3(upX, upY, upZ);
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }
    
    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }
    
    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->MovementSpeed * deltaTime * 10;
        if (direction == FRONT)
            this->Position.z -= (velocity*1.8);
        if (direction == BACK)
            this->Position.z += (velocity*1.8);
//
//        return;
        
        if (direction == FORWARD)
            this->Position += this->Front * velocity;
        if (direction == BACKWARD)
            this->Position -= this->Front * velocity;
        if (direction == LEFT)
            this->Position -= this->Right * velocity;
        if (direction == RIGHT)
            this->Position += this->Right * velocity;
        //        this->Position.y = 0.0f;
        
        std::cout<<"Position" <<this->Position.x<<" " << this->Position.y << " " << this->Position.z << std::endl;
    }
    
    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
//        return;
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;
        
        this->Yaw   += xoffset;
        this->Pitch += yoffset;
        
        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (this->Pitch > 89.0f)
                this->Pitch = 89.0f;
            if (this->Pitch < -89.0f)
                this->Pitch = -89.0f;
        }
        
        // Update Front, Right and Up Vectors using the updated Eular angles
        this->updateCameraVectors();
    }
    
    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(GLfloat yoffset)
    {
        return;
        if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
            this->Zoom -= yoffset;
        if (this->Zoom <= 1.0f)
            this->Zoom = 1.0f;
        if (this->Zoom >= 45.0f)
            this->Zoom = 45.0f;
    }
    
private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
        std::cout<<"Front" <<this->Front.x<<" " << this->Front.y << " " << this->Front.z << std::endl;
//
        std::cout<<"Right" <<this->Right.x<<" " << this->Right.y << " " << this->Right.z << std::endl;
//
        std::cout<<"Up" <<this->Up.x<<" " << this->Up.y << " " << this->Up.z << std::endl;
        
    }
};

