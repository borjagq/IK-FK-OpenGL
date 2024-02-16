/**
 * @file main.cpp
 * @brief Main aladdin 3d file.
 * @version 1.0.0 (2022-10-21)
 * @date 2022-10-21
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#include "main.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/common.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "classes/camera/camera.h"
#include "classes/light/light.h"
#include "classes/object_hierarchical/object_hierarchical.h"
#include "classes/shader/shader.h"
#include "structs/bounding_box/bounding_box.h"
#include "structs/helpful/helpful.h"

void clean() {

	// Delete all the shaders.
	shader->remove();
    
    // Terminate ImGUI.
    ImGui_ImplGlfwGL3_Shutdown();
    
    // Close GL context and any other GLFW resources.
    glfwTerminate();

}

void clear() {
    
    // Specify the color of the background
    glClearColor(background.x, background.y, background.z, background.w);

    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
}

void displayElements() {
                
    // Pass the parameters to the shaders.
    shader->activate();
    shader->passLight(scene_light);
    
    // Get the selected object.
    bgq_opengl::ObjectHierarchical *selected;
    if (current_scene == 0)
        selected = onyx;
    else if (current_scene == 1)
        selected = jelly;
    else
        assert(false);
    
    // Do this for forward kinematics
    if (kinematic == 0) {
        
        // Check that there is actually some control point moving.
        if (moving_joint != -1 && ctrl_pnts[moving_joint].coords != ctrl_pnts[moving_joint].initial) {
            
            // Get the two vectors representing this rotation.
            // The first vector is the one defined by the current position and the anchor. The second is the desired position and the anchor.
            glm::vec3 v1 = glm::normalize(ctrl_pnts[moving_joint].initial - ctrl_pnts[moving_joint].anchor);
            glm::vec3 v2 = glm::normalize(ctrl_pnts[moving_joint].coords  - ctrl_pnts[moving_joint].anchor);
            
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
                        
            // Apply the same transformations to the anchor points and
            glm::vec3 anchor = ctrl_pnts[moving_joint].anchor;
            glm::mat4 trans_matrix = glm::translate(glm::mat4(1.0), glm::vec3(anchor.x, anchor.y, anchor.z));
            trans_matrix = trans_matrix * rotation;
            trans_matrix = glm::translate(trans_matrix, glm::vec3(-anchor.x, -anchor.y, -anchor.z));
                        
            // We skip rotations in X cause there is no freedom in that direction.
            selected->transform(ctrl_pnts[moving_joint].indices, ctrl_pnts[moving_joint].anchor, trans_matrix);
            
        }
        
    } else if (kinematic == 1) {
        
        if (moving_joint != -1 && ctrl_pnts[moving_joint].coords != ctrl_pnts[moving_joint].initial) {
            
            // Get the number of IK we're gonna use.
            int num_ik = std::min((int) ctrl_pnts[moving_joint].indices.size() + 1, ik_level);
            
            // Get the target we're trying to reach.
            glm::vec3 target = ctrl_pnts[moving_joint].coords;
            
            // Get the control points ordered.
            std::vector<bgq_opengl::ControlPoint> current_ctrl = selected->getControlPoints();
            std::vector<bgq_opengl::ControlPoint> ik_struc = selected->getOrderedControlPoints(num_ik, current_ctrl[moving_joint]);
            
            // Keep iterating until nothing moves.
            bool has_moved = true;
            int iterations = 0;
            while (has_moved) {
                
                // By now, nothing has moved.
                has_moved = false;
                
                // Iterate through the anchors and control points.
                for (int i = 0; i < ik_struc.size(); i++) {
                    
                    // Get what is being currently reached in the geometry.
                    glm::vec3 curr_reach = current_ctrl[moving_joint].coords;
                                        
                    // Get current anchor and indices to rotate.
                    glm::vec3 ik_anchor = ik_struc[i].anchor;
                    std::vector<unsigned int> ik_inds = ik_struc[i].indices;
                    
                    // Get the two vectors representing this rotation.
                    glm::vec3 v1 = glm::normalize(curr_reach - ik_anchor);
                    glm::vec3 v2 = glm::normalize(target  - ik_anchor);
                    
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
                                
                    // Apply the same transformations to the anchor points and
                    glm::mat4 trans_matrix = glm::translate(glm::mat4(1.0), glm::vec3(ik_anchor.x, ik_anchor.y, ik_anchor.z));
                    trans_matrix = trans_matrix * rotation;
                    trans_matrix = glm::translate(trans_matrix, glm::vec3(-ik_anchor.x, -ik_anchor.y, -ik_anchor.z));
                                
                    // We skip rotations in X cause there is no freedom in that direction.
                    selected->transform(ik_inds, ik_anchor, trans_matrix);
                    
                    // Update the current control points and ordered control points, as they could have changed.
                    current_ctrl = selected->getControlPoints();
                    ik_struc = selected->getOrderedControlPoints(num_ik, current_ctrl[moving_joint]);
                                                                                
                    // Check if the arm has mooved during this iteration.
                    if (glm::distance(curr_reach, current_ctrl[moving_joint].coords) >= 0.01)
                        has_moved = true;
                    
                    // Increment the iterations.
                    iterations++;
                    
                }
                
                // If the number of iterations has surpassed the cube of the IK, skip.
                if (iterations >= pow(num_ik, 3))
                    break;

            }
            
        }
        
    } else if (kinematic == 2) {
        
        // Only in the jelly is scripted.
        selected = jelly;
        
        // Get the parameters for the animation.
        const float amplitude = -script_amplitude;
        const float amplitude_inner = -amplitude;
        const float amplitude_inner_inner = 2.0f * amplitude_inner;
        const float frequency = script_frequency;
        const float phase = asin(0.0);
        const float phase_inner = asin(0.0);
        const float phase_inner_inner = asin(-0.8);
        
        std::vector<float> movements;
        movements.push_back(amplitude * sin(frequency * internal_time + phase) - (amplitude / 2.0f));
        movements.push_back(amplitude_inner * cos(frequency * internal_time + phase_inner));
        movements.push_back(amplitude_inner_inner * cos(frequency * internal_time + phase_inner_inner));
        
        for (unsigned int j = 1; j < 4; j++) {
            
            // Get control points that belong to the first arms.
            for (unsigned int i = 0; i < ctrl_pnts.size(); i++) {
                
                // If this is not one of those first control points, leave.
                if (ctrl_pnts[i].indices.size() != j)
                    continue;
                
                // Update the control points.
                ctrl_pnts = selected->getControlPoints();
                
                // Desired displacement.
                float disp_amount = 0;
                for (int k = 0; k < ctrl_pnts[i].indices.size(); k++)
                    disp_amount += movements[k];
                
                // Get the displacement direction and the goal point.
                glm::vec3 disp_dir = glm::normalize(ctrl_scrp[i].coords - glm::vec3(0.0f, ctrl_scrp[i].coords.y, 0.0f));
                glm::vec3 goal = ctrl_scrp[i].coords + (disp_dir * disp_amount);
                
                std::cout << glm::to_string(goal) << std::endl;
                
                // Get the two vectors representing this rotation.
                glm::vec3 v1 = glm::normalize(ctrl_pnts[i].initial - ctrl_pnts[i].anchor);
                glm::vec3 v2 = glm::normalize(goal  - ctrl_pnts[i].anchor);
                
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
                
                // Apply the same transformations to the anchor points and
                glm::vec3 anchor = ctrl_pnts[i].anchor;
                glm::mat4 trans_matrix = glm::translate(glm::mat4(1.0), glm::vec3(anchor.x, anchor.y, anchor.z));
                trans_matrix = trans_matrix * rotation;
                trans_matrix = glm::translate(trans_matrix, glm::vec3(-anchor.x, -anchor.y, -anchor.z));
                
                // We skip rotations in X cause there is no freedom in that direction.
                selected->transform(ctrl_pnts[i].indices, ctrl_pnts[i].anchor, trans_matrix);
                
            }
            
        }

    }

    // Draw the creature.
    selected->draw(*shader, *camera);
    
    // Update the control points.
    ctrl_pnts = selected->getControlPoints();
    
    // Draw the control points.
    if (show_ctrl == 1 and kinematic != 2) {
        
        for (int i = 0; i < ctrl_pnts.size(); i++)
            displayControlPoint(ctrl_pnts[i], 0.2);
        
    }
    
}

void displayGUI() {
    
    // Init ImGUI for rendering.
    ImGui_ImplGlfwGL3_NewFrame();
    
    // Begin the new widget.
    ImGui::Begin("Scene");
    
    // Select the creature.
    ImGui::Text("Creature");
    ImGui::RadioButton("Worm", &current_scene, 0);
    ImGui::RadioButton("Cube jelly", &current_scene, 1);

    // Select the method.
    ImGui::Text("Kinematics");
    ImGui::RadioButton("FK", &kinematic, 0);
    ImGui::RadioButton("IK", &kinematic, 1);
    ImGui::RadioButton("Scripted", &kinematic, 2);

    // This part only makes sense for inverse kinematics.
    if (kinematic == 1) {
        
        int max_ik = -1;
        for (int i = 0; i < ctrl_pnts.size(); i++) {
            int len = (int) ctrl_pnts[i].indices.size() + 1;
            if (max_ik < len) {
                max_ik = len;
            }
        }
        
        // Make sure that it is in the range.
        if (ik_level > max_ik) ik_level = max_ik;
        
        ImGui::Text("Max bones");
        for (int i = 1; i <= max_ik; i++) {
            std::string str = std::string("IK ") + std::to_string(i);
            ImGui::RadioButton(str.c_str() , &ik_level, i);
        }
        
    }
    
    if (kinematic != 2) {
        
        // Select whether to display the control points or not.
        ImGui::Text("Display controls");
        ImGui::RadioButton("Yes", &show_ctrl, 1);
        ImGui::RadioButton("No", &show_ctrl, 0);
        
    } else {
        
        // The scripted animation parameters.
        ImGui::Text("Script params");
        ImGui::SliderFloat("Amplitude", &script_amplitude, 0.0f, 2.0f);
        ImGui::SliderFloat("Frequency", &script_frequency, 0.01f, 30.0f);

    }
    
    ImGui::End();
    
    // Render ImGUI.
    ImGui::Render();
    
}

void displayControlPoint(const bgq_opengl::ControlPoint ctrl_pnt, const float size) {
    
    glDepthFunc(GL_ALWAYS);
    
    // Reset it.
    dis_pnt->resetTransforms();
    
    // Resize it to the desired size.
    dis_pnt->scale(size, size, size);
    
    // Center the object and get it in the right position.
    dis_pnt->translate(ctrl_pnt.coords.x, ctrl_pnt.coords.y, ctrl_pnt.coords.z);
    
    dis_pnt->draw(*shaderPnt, *camera);
    
    glDepthFunc(GL_LESS);
    
}

void buildCube() {
    
    /*
    // Make the coordinates.
    std::vector<glm::vec3> coords;
    std::vector<glm::vec3> normals;
    
    // Init the vertices that will be created.
    std::vector<bgq_opengl::Vertex> vertices;
    std::vector<GLuint> indices;
    
    // FRONT
    normals.push_back(glm::vec3(0.0f,  0.0f, 1.0f));
    coords.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
    coords.push_back(glm::vec3( 0.5f, -0.5f, 0.5f));
    coords.push_back(glm::vec3( 0.5f,  0.5f, 0.5f));
    coords.push_back(glm::vec3(-0.5f,  0.5f, 0.5f));
    
    // BACK
    normals.push_back(glm::vec3(0.0f,  0.0f, -1.0f));
    coords.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
    coords.push_back(glm::vec3(-0.5f,  0.5f, -0.5f));
    coords.push_back(glm::vec3( 0.5f,  0.5f, -0.5f));
    coords.push_back(glm::vec3( 0.5f, -0.5f, -0.5f));
    
    // LEFT
    normals.push_back(glm::vec3(-1.0f, 0.0f,  0.0f));
    coords.push_back(glm::vec3(-0.5f, -0.5f,  0.5f));
    coords.push_back(glm::vec3(-0.5f,  0.5f,  0.5f));
    coords.push_back(glm::vec3(-0.5f,  0.5f, -0.5f));
    coords.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
    
    // RIGHT
    normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    coords.push_back(glm::vec3( 0.5f, -0.5f, -0.5f));
    coords.push_back(glm::vec3( 0.5f,  0.5f, -0.5f));
    coords.push_back(glm::vec3( 0.5f,  0.5f,  0.5f));
    coords.push_back(glm::vec3( 0.5f, -0.5f,  0.5f));
    
    // TOP
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    coords.push_back(glm::vec3(-0.5f, 0.5f,  0.5f));
    coords.push_back(glm::vec3( 0.5f, 0.5f,  0.5f));
    coords.push_back(glm::vec3( 0.5f, 0.5f, -0.5f));
    coords.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
    
    // BOTTOM
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    coords.push_back(glm::vec3(-0.5f, -0.5f,  0.5f));
    coords.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
    coords.push_back(glm::vec3( 0.5f, -0.5f, -0.5f));
    coords.push_back(glm::vec3( 0.5f, -0.5f,  0.5f));
    
    // Iterate through the faces.
    for (int i = 0; i < normals.size(); i++) {
        
        // Iterate through the vertices forming that face.
        for (int j = 0; j < 4; j++) {
            
            // Add a new vertex.
            vertices.push_back(bgq_opengl::Vertex{
                coords[i * 4 + j],
                normals[i],
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 0.0f)
            });
            
        }
        
        // Add indices for this face.
        indices.push_back(i * 4 + 0);
        indices.push_back(i * 4 + 1);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 3);
        indices.push_back(i * 4 + 0);
        
    }
    
    // Build this geometry.
    dis_pnt = new bgq_opengl::Geometry(vertices, indices, {}, 1.0);
     */
    
    dis_pnt = new bgq_opengl::Object("Sphere.obj", "Assimp");
    
}

void handleKeyEvents() {
    
    // Key W will move camera 0 forward.
    if (ImGui::IsKeyPressed('S')) {

        // Move forward.
        camera->rotateDown();
        
    }
    
    // Key W will move camera 0 forward.
    if (ImGui::IsKeyPressed('W')) {

        // Move forward.
        camera->rotateUp();
        
    }
    
    // Key W will move camera 0 forward.
    if (ImGui::IsKeyPressed('D')) {

        // Move forward.
        camera->rotateRight();
        
    }
    
    // Key W will move camera 0 forward.
    if (ImGui::IsKeyPressed('A')) {

        // Move forward.
        camera->rotateLeft();
        
    }
    
}

void handleMouseEvents(GLFWwindow* window, double xpos, double ypos) {
    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
    
        // Release the joint.
        moving_joint = -1;
        return;

    }
        
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        
        // If there was no moving joint, locate the closest one in a minimum range.
        if (moving_joint == -1) {
            
            // Init the control variables to find the minimum distance.
            float min_dist = 10000.0f;
            
            // Build the view port.
            glm::vec4 viewport(0.0f, 0.0f, camera->getWidth(), camera->getHeight());
            
            // Loop through all the control points and store the minimum distance.
            for (int i = 0; i < ctrl_pnts.size(); i++) {
                
                // Get the screen coordinates for this point.
                glm::vec3 img_coord = glm::project(ctrl_pnts[i].coords, camera->getView(), camera->getProjection(), viewport);
                
                // Invert the Y coordinates.
                img_coord.y = camera->getHeight() - img_coord.y;
                                
                // Get the 2D distance.
                float dist = glm::distance(glm::vec2(xpos, ypos), glm::vec2(img_coord));
                
                // If this is the new minimum, store it as so and select this point.
                if (dist < min_dist and dist < 10) {
                    moving_joint = i;
                    min_dist = dist;
                }
                
            }
                        
            return;
            
        }
        
        // Here we already have a selected point.
        // Let's build a plane using the 0,0,0 to camera normal and the image screen depth of the point.
        
        // Get the screen coordinates by calculating the z as well.
        glm::vec4 reference(ctrl_pnts[moving_joint].initial, 1.0f);
        reference = (*camera).getProjection() * (*camera).getView() * reference;
        
        // Build the screen coordinates.
        glm::vec3 screen(xpos, ypos, reference.z / reference.w);
        glm::vec3 click = bgq_opengl::Helpful::unProjectScreenToWorld(screen, *camera);
        
        // Move the current control point to the current position of the mouse.
        ctrl_pnts[moving_joint].coords = click;
                
        return;

    }

}

void initElements() {
    
	// Create a white light in the center of the world.
    scene_light = bgq_opengl::Light(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // Init the shader.
    shader = new bgq_opengl::Shader("blinnPhongColor.vert", "blinnPhongColor.frag");
    shaderPnt = new bgq_opengl::Shader("blinnPhongColor.vert", "red.frag");

	// Creates the first camera object
    camera = new bgq_opengl::Camera(glm::vec3(0.0f, 0.0f, 15.0f), glm::vec3(0.0f, -0.0f, -1.0f), 45.0f, 0.1f, 300.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Init the creatures.
    std::vector<int> indOnyx = {0, 1, 2, 3, 4};
    std::vector<int> parOnyx = {-1, 0, 1, 2, 3};
    onyx = new bgq_opengl::ObjectHierarchical("Onyx.glb", indOnyx, parOnyx, "left", "inter", "right");
    
    std::vector<int> indJelly = {12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    std::vector<int> parJelly = {-1, 0, 1, 2, 0, 4, 5, 0, 7, 8, 0, 10, 11};
    
    jelly = new bgq_opengl::ObjectHierarchical("CubeJelly.glb", indJelly, parJelly, "centre", "inter", "down");

    // Get the control points for the onyx.
    ctrl_pnts = onyx->getControlPoints();
    ctrl_scrp = jelly->getControlPoints();

}

void initEnvironment(int argc, char** argv) {
    
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        
        std::cerr << "ERROR: could not start GLFW3" << std::endl;
        exit(1);

    }
    
    // MacOS initializations.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create the window.
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_NAME, NULL, NULL);
    if (!window) {
        
        std::cerr << "Error 121-1001 - Failed to create the window." << std::endl;
        glfwTerminate();
        exit(1);

    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW and OpenGL.
    GLenum res = glewInit();

    // Check for any errors.
    if (res != GLEW_OK) {

        std::cerr << "Error 121-1002 - GLEW could not be initialized:" << glewGetErrorString(res) << std::endl;
        exit(1);

    }
    
    // Get version info.
    std::cerr << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cerr << "OpenGL version supported " << glGetString(GL_VERSION) << std::endl;
    std::cerr << "GLSL version supported " << (char *) glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    // Init the mouse events handler.
    glfwSetCursorPosCallback(window, handleMouseEvents);
    
    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    
    // Get the elements that will be used to display control points.
    buildCube();
    
}

int main(int argc, char** argv) {

	// Initialise the environment.
    initEnvironment(argc, argv);
    
	// Initialise the objects and elements.
	initElements();
    
	// Main loop.
    while(!glfwWindowShouldClose(window)) {
        
        // Clear the scene.
        clear();
        
        // Get the window size.
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        camera->setWidth(width);
        camera->setHeight(height);
        
        // Get the current time.
        internal_time = glfwGetTime();

        // Handle key events.
        handleKeyEvents();
        
        // Display the scene.
        displayElements();
        
        // Make the things to print everything.
        displayGUI();
        
        // Update other events like input handling.
        glfwPollEvents();
        glfwSwapBuffers(window);
        
    }

	// Clean everything and terminate.
	clean();

}
