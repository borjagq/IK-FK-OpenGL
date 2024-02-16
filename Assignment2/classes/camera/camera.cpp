/**
 * @file camera.cpp
 * @brief Camera class implementation file.
 * @version 1.0.0 (2022-11-23)
 * @date 2022-11-23
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#include "camera.h"

#include <iostream>

#include "GL/glew.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

namespace bgq_opengl {

	Camera::Camera(glm::vec3 position, glm::vec3 direction, float fov, float near, float far, int width, int height) {

		// Store all the parameters.
		this->direction = direction;
		this->position = position;
		this->far = far;
		this->fov = fov;
		this->near = near;
		this->window_width = width;
		this->window_height = height;

		// Set defaults.
		this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	}

	glm::mat4 Camera::getCameraMatrix() {

        return this->getProjection() * this->getView();

	}

	glm::vec3 Camera::getDirection() {

        return glm::normalize(glm::vec3(transforms * glm::vec4(this->direction, 1.0)));

	}

	glm::vec3 Camera::getPosition() {

        return glm::vec3(transforms * glm::vec4(this->position, 1.0));

	}

	glm::mat4 Camera::getProjection() {
        
        // Adds perspective to the scene.
        float ratio = (float)this->window_width / this->window_height;
        return glm::perspective(glm::radians(this->fov), ratio, this->near, this->far);

	}

	glm::vec3 Camera::getUp() {

		return glm::normalize(glm::vec3(transforms * glm::vec4(this->up, 1.0)));

	}

	glm::mat4 Camera::getView() {
        
        // Get the position and direction.
        glm::vec3 position = this->getPosition();
        glm::vec3 direction = this->getDirection();
        glm::vec3 up = this->getUp();
        
        // Calculate the view matrix.
        return glm::lookAt(position, position + direction, up);

	}

    void Camera::setWidth(int new_width) {
        
        this->window_width = new_width;
        
    }

    void Camera::setHeight(int new_height) {
        
        this->window_height = new_height;
        
    }

    int Camera::getWidth() {
        
        return this->window_width;
        
    }

    int Camera::getHeight() {
        
        return this->window_height;
        
    }

    float Camera::getNear() {
        
        return this->near;
        
    }

    float Camera::getFar() {
        
        return this->far;
        
    }

	void Camera::moveBack() {

		// Get the direction.
        glm::vec3 direction = glm::normalize(this->getDirection()) * 0.1f;
        
        // Apply it as an inversed translation.
        this->translate(-direction.x, -direction.y, -direction.z);

	}

	void Camera::moveDown() {

        // Get the direction.
        glm::vec3 up = glm::normalize(this->getUp()) * 0.1f;
        
        // Apply it as an inversed translation.
        this->translate(-up.x, -up.y, -up.z);

	}

	void Camera::moveFront() {
        
        // Get the direction.
        glm::vec3 direction = glm::normalize(this->getDirection()) * 0.1f;
        
        // Apply it as an inversed translation.
        this->translate(direction.x, direction.y, direction.z);

	}

	void Camera::moveLeft() {

        // Get the direction.
        glm::vec3 direction = glm::normalize(this->getDirection()) * 0.1f;
        glm::vec3 up = glm::normalize(this->getUp());

        glm::vec3 left_direction = glm::normalize(-glm::cross(direction, up));
        
        // Apply it as an inversed translation.
        this->translate(left_direction.x, left_direction.y, left_direction.z);

	}

	void Camera::moveRight() {

        // Get the direction.
        glm::vec3 direction = glm::normalize(this->getDirection()) * 0.1f;
        glm::vec3 up = glm::normalize(this->getUp());

        glm::vec3 left_direction = glm::normalize(glm::cross(direction, up));
        
        // Apply it as an inversed translation.
        this->translate(left_direction.x, left_direction.y, left_direction.z);

	}

	void Camera::moveUp() {

        // Get the direction.
        glm::vec3 up = glm::normalize(this->getUp()) * 0.1f;
        
        // Apply it as an inversed translation.
        this->translate(up.x, up.y, up.z);

	}

	void Camera::rotateDown() {
        
        // Get the current vector and the up vector.
        glm::vec3 v1 = glm::normalize(this->getPosition());
        glm::vec3 v2 = glm::normalize(this->getPosition() - this->getUp() * 0.2f);
        
        // Build the quaternion that will give us the desired rotations.
        glm::vec3 a = glm::cross(v1, v2);
        glm::quat q;
        q.x = a.x;
        q.y = a.y;
        q.z = a.z;
        q.w = std::sqrt(std::pow(glm::length(v1), 2) * std::pow(glm::length(v2), 2)) + glm::dot(v1, v2);
        
        // Normalize it.
        q = glm::normalize(q);
        
        // Transform it into a matrix so that we can apply it.
        glm::mat4 rotation = glm::toMat4(q);
                    
        // Apply it to the transforms.
        this->transforms = rotation * this->transforms;

	}

	void Camera::rotateLeft() {

        this->rotate(0.0, 1.0, 0.0, -1.0);

	}

	void Camera::rotateRight() {

        this->rotate(0.0, 1.0, 0.0, 1.0);

	}

	void Camera::rotateUp() {

        // Get the current vector and the up vector.
        glm::vec3 v1 = glm::normalize(this->getPosition());
        glm::vec3 v2 = glm::normalize(this->getPosition() + this->getUp() * 0.2f);
        
        // Build the quaternion that will give us the desired rotations.
        glm::vec3 a = glm::cross(v1, v2);
        glm::quat q;
        q.x = a.x;
        q.y = a.y;
        q.z = a.z;
        q.w = std::sqrt(std::pow(glm::length(v1), 2) * std::pow(glm::length(v2), 2)) + glm::dot(v1, v2);
        
        // Normalize it.
        q = glm::normalize(q);
        
        // Transform it into a matrix so that we can apply it.
        glm::mat4 rotation = glm::toMat4(q);
                    
        // Apply it to the transforms.
        this->transforms = rotation * this->transforms;

	}

    void Camera::resetTransforms() {

        this->transforms = glm::mat4(1.0f);

    }

    void Camera::rotate(float x, float y, float z, float angle) {

        // Get the angle as radians.
        float radians = glm::radians(angle);

        // Create a rotation matrix.
        glm::mat4 identity_matrix(1.0f);
        glm::mat4 rotation_matrix = glm::rotate(identity_matrix, radians, glm::vec3(x, y, z));

        this->transforms = rotation_matrix * this->transforms;

    }

    void Camera::translate(float x, float y, float z) {

        // Get the translation matrix.
        glm::mat4 trans_matrix = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));

        // Apply it to the transormations.
        this->transforms = trans_matrix * this->transforms;

    }

    glm::mat4 Camera::getTransformMat() {
        
        return this->transforms;
        
    }

    void Camera::setTransformMat(glm::mat4 transform) {
        
        this->transforms = transform;
        
    }

}  // namespace bgq_opengl
