/**
 * @file helful.cpp
 * @brief A set of helpful functions.
 * @version 1.0.0 (2023-03-02)
 * @date 2023-03-02
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */
#include "helpful.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "classes/camera/camera.h"

#include <iostream>

namespace bgq_opengl {

    glm::vec3 Helpful::unProjectScreenToWorld(glm::vec3 screen, Camera camera) {
    
        // Build the viewport from the camera.
        glm::vec4 viewport(0.0f, 0.0f, camera.getWidth(), camera.getHeight());

        // Reconstruct the
        glm::vec3 camera_xyz;
        camera_xyz.x = 2.0f * (screen.x - viewport.x) / viewport.z - 1.0f;
        camera_xyz.y = 1.0f - 2.0f * (screen.y - viewport.y) / viewport.w;
        camera_xyz.z = screen.z;
        
        // Now, let's transform the camera coordinates to worls coordinates.
        glm::mat4 camera_matrix = camera.getProjection() * camera.getView();
        glm::vec4 world = glm::inverse(camera_matrix) * glm::vec4(camera_xyz, 1.0);

        // Scale it to obtain the real ones.
        return glm::vec3(world) / world.w;
    
    }

    glm::vec3 Helpful::get3Angles(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
        
        glm::vec3 ret(0.0f);
        
        // Calculate the individual angles.
        ret.x = Helpful::getAngle(glm::vec2(a.z, a.y), glm::vec2(b.z, b.y), glm::vec2(c.z, c.y));
        ret.y = Helpful::getAngle(glm::vec2(a.x, a.z), glm::vec2(b.x, b.z), glm::vec2(c.x, c.z));
        ret.z = Helpful::getAngle(glm::vec2(a.x, a.y), glm::vec2(b.x, b.y), glm::vec2(c.x, c.y));
        
        return ret;

    }

    float Helpful::getAngle(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
        
        // Otherwise, get the angle.
        double dir_ca = atan2(a.y - c.y, a.x - c.x);
        double dir_cb = atan2(b.y - c.y, b.x - c.x);
        float angle = dir_ca - dir_cb;
        
        // Handle wrap around
        const double pi = acos(-1);
        if (angle > pi) angle -= 2 * pi;
        else if (angle < -pi) angle += 2 * pi;
        
        return glm::degrees(angle);
        
    }

}  // namespace bgq_opengl
