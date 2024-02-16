/**
 * @file geometry.cpp
 * @brief Geometry class implementation file.
 * @version 1.0.0 (2022-11-26)
 * @date 2022-11-26
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#include "node.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

#include "GL/glew.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "classes/camera/camera.h"
#include "classes/ebo/ebo.h"
#include "classes/shader/shader.h"
#include "classes/texture/texture.h"
#include "classes/vao/vao.h"
#include "structs/vertex/vertex.h"
#include "structs/bounding_box/bounding_box.h"
#include "structs/control_point/control_point.h"

namespace bgq_opengl {

    Node::Node(const int id, const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures, const std::vector<Node> &children) : geometry(vertices, indices, textures, 1.0) {
        
        this->children = children;
        
    }

    glm::vec3 Node::getAnchor() {
        
        return this->anchor;
        
    }

    std::vector<ControlPoint> Node::getControlPoints() {
        
        // Get the values for this node itself.
        std::vector<ControlPoint> curr_ctrl;
        
        // For each control point, add it to the structure.
        for (unsigned int i = 0; i < this->control.size(); i++) {
            
            curr_ctrl.push_back(ControlPoint{
                this->control[i],
                this->control[i],
                this->anchor,
                std::vector<unsigned int>()
            });
            
        }
        
        // For each child, get its points and append them.
        for (unsigned int i = 0; i < this->children.size(); i++) {
            
            // Get the controls of this child.
            std::vector<ControlPoint> child_ctrl = this->children[i].getControlPoints();
            
            // Recursively, append the new index to all of the descendents.
            for (unsigned int j = 0; j < child_ctrl.size(); j++) {
                
                // Append this index to each child control point.
                child_ctrl[j].indices.push_back(i);
                
            }
            
            // Append this to the general list.
            curr_ctrl.insert(curr_ctrl.end(), child_ctrl.begin(), child_ctrl.end());
            
        }
        
        return curr_ctrl;
        
    }

    std::vector<ControlPoint> Node::getOrderedControlPoints(const int num_ik, ControlPoint ctrl) {
        
        // Init what we will return.
        std::vector<ControlPoint> ret_ctrl(0);
        
        // If it has directions, go there.
        if (ctrl.indices.size() > 0) {
            
            // Get the last element.
            ControlPoint ctrl_copy = ctrl;
            int ind = ctrl_copy.indices.back();
            ctrl_copy.indices.pop_back();
            
            // Apply this to the next ones.
            ret_ctrl = this->children[ind].getOrderedControlPoints(num_ik, ctrl_copy);
            
            // Add this ID to all of em.
            for (int i = 0; i < ret_ctrl.size(); i++) {
                ret_ctrl[i].indices.push_back(ind);
            }
            
        }
        
        // If the ret_ctrl has less points than wanted, it means that this node is part of the
        // result. Therefore, append.
        if (ret_ctrl.size() < num_ik) {
            
            ControlPoint new_pnt;
            
            // Chek if there is something to follow or not.
            if (ret_ctrl.size() > 0) {
                
                new_pnt.coords = ret_ctrl.back().anchor;
                new_pnt.initial = ret_ctrl.back().anchor;
                new_pnt.anchor = this->getAnchor();
                new_pnt.indices = {};
                
            } else {
                
                new_pnt.coords = ctrl.coords;
                new_pnt.initial = ctrl.initial;
                new_pnt.anchor = this->getAnchor();
                new_pnt.indices = {};
                
            }
            
            ret_ctrl.push_back(new_pnt);
            
        }
        
        return ret_ctrl;
        
    }

    void Node::calculateControlPoints(const char* first, const char* middle, const char* end) {

        if (this->children.size() > 0) {
            
            // For each child, get its anchor point and set it as control.
            for (int i = 0; i < this->children.size(); i++) {
                
                this->children[i].calculateControlPoints(this->geometry, middle, end);
                this->control.push_back(this->children[i].getAnchor());

            }
            
        } else {
            
            // Get the control point.
            if (strcmp(end, "right") == 0) {
                
                BoundingBox bb = this->geometry.getBoundingBox();
                
                this->control.push_back(glm::vec3(bb.max.x, (bb.min.y + bb.max.y) / 2, (bb.min.z + bb.max.z) / 2));
                
            } else {
                
                assert(false);
                
            }
            
        }
        
        // Get the anchor point.
        if (strcmp(first, "left") == 0) {
            
            BoundingBox bb = this->geometry.getBoundingBox();
            
            this->anchor = glm::vec3(bb.min.x, (bb.min.y + bb.max.y) / 2, (bb.min.z + bb.max.z) / 2);
            
        } else if (strcmp(first, "centre") == 0) {
            
            BoundingBox bb = this->geometry.getBoundingBox();
            
            this->anchor = glm::vec3((bb.min.x + bb.max.x) / 2, (bb.min.y + bb.max.y) / 2, (bb.min.z + bb.max.z) / 2);
            
        } else {
            
            assert(false);
            
        }
        
        this->anchor_original = this->anchor;
        this->control_original = this->control;

    }

    void Node::calculateControlPoints(const Geometry &parentGeom, const char* middle, const char* end) {
        
        if (this->children.size() > 0) {
            
            // For each child, get its anchor point and set it as control.
            for (int i = 0; i < this->children.size(); i++) {
                
                this->children[i].calculateControlPoints(this->geometry, middle, end);
                this->control.push_back(this->children[i].getAnchor());
                
            }
            
        } else {
            
            // Get the control point.
            if (strcmp(end, "right") == 0) {
                
                BoundingBox bb = this->geometry.getBoundingBox();
                
                this->control.push_back(glm::vec3(bb.max.x, (bb.min.y + bb.max.y) / 2, (bb.min.z + bb.max.z) / 2));

            } else if (strcmp(end, "down") == 0) {
                
                BoundingBox bb = this->geometry.getBoundingBox();
                
                this->control.push_back(glm::vec3((bb.min.x + bb.max.x) / 2, bb.min.y, (bb.min.z + bb.max.z) / 2));

            } else {
                
                assert(false);
                
            }
            
        }
        
        // Get the anchor point.
        if (strcmp(middle, "left") == 0) {
            
            BoundingBox bb = this->geometry.getBoundingBox();
            
            this->anchor = glm::vec3(bb.min.x, (bb.min.y + bb.max.y) / 2.0f, (bb.min.z + bb.max.z) / 2.0f);
            
        } else if (strcmp(middle, "inter") == 0) {
            
            BoundingBox bb_1 = this->geometry.getBoundingBox();
            BoundingBox bb_2 = parentGeom.getBoundingBox();
            
            BoundingBox intersection {
                glm::vec3(fmax(bb_1.min.x, bb_2.min.x), fmax(bb_1.min.y, bb_2.min.y), fmax(bb_1.min.z, bb_2.min.z)),
                glm::vec3(fmin(bb_1.max.x, bb_2.max.x), fmin(bb_1.max.y, bb_2.max.y), fmin(bb_1.max.z, bb_2.max.z))
            };
            
            this->anchor = (intersection.min + intersection.max) / 2.0f;
            
        } else {
            
            assert(false);
            
        }
        
        this->anchor_original = this->anchor;
        this->control_original = this->control;
        
    }

    Geometry Node::getGeometry() {
        
        return this->geometry;
        
    }

    BoundingBox Node::getBoundingBox() {

        // Create the bb.
        BoundingBox global_bb = geometry.getBoundingBox();

        // Loop through the vertices and get tge min and max values.
        for (int i = 1; i < this->children.size(); i++) {

            // Get the current bb.
            BoundingBox bb = this->children[i].getBoundingBox();

            if (global_bb.min.x > bb.min.x)
                global_bb.min.x = bb.min.x;

            if (global_bb.min.y > bb.min.y)
                global_bb.min.y = bb.min.y;

            if (global_bb.min.z > bb.min.z)
                global_bb.min.z = bb.min.z;

            if (global_bb.max.x < bb.max.x)
                global_bb.max.x = bb.max.x;

            if (global_bb.max.y < bb.max.y)
                global_bb.max.y = bb.max.y;

            if (global_bb.max.z < bb.max.z)
                global_bb.max.z = bb.max.z;

        }

        return global_bb;

    }

	void Node::draw(Shader &shader, Camera &camera) {
        
        this->geometry.draw(shader, camera);
        
        // Do the same for its children.
        for (int i = 0; i < this->children.size(); i++)
            this->children[i].draw(shader, camera);

	}

	void Node::resetTransforms() {

        geometry.resetTransforms();
        
        // Do the same for its children.
        for (int i = 0; i < this->children.size(); i++)
            this->children[i].resetTransforms();
        
        this->anchor = this->anchor_original;
        this->control = this->control_original;

	}

    void Node::rotate(float x, float y, float z, float angle) {
        
        // Resend it using the 0,0,0 anchor to reduce code.
        this->rotate(this->getAnchor(), x, y, z, angle);
        
    }

    void Node::rotate(std::vector<unsigned int> inds, float x, float y, float z, float angle) {
        
        // If it has something, apply it and end.
        if (inds.size() > 0) {
            
            // Get the last element.
            int ind = inds.back();
            inds.pop_back();
            
            // Apply this to the next ones.
            this->children[ind].rotate(inds, x, y, z, angle);
            
            return;
            
        }
        
        // If it doesn't it means that it is destined here.
        this->rotate(inds, this->getAnchor(), x, y, z, angle);
        
    }

    void Node::rotate(glm::vec3 anchor, float x, float y, float z, float angle) {
        
        // Rotate the geometry.
        this->geometry.translate(-anchor.x, -anchor.y, -anchor.z);
        this->geometry.rotate(x, y, z, angle);
        this->geometry.translate(anchor.x, anchor.y, anchor.z);
        
        // Do the same for its children.
        for (int i = 0; i < this->children.size(); i++)
            this->children[i].rotate(anchor, x, y, z, angle);
        
        // Apply the same transformations to the anchor points and
        glm::mat4 trans_matrix = glm::translate(glm::mat4(1.0), glm::vec3(anchor.x, anchor.y, anchor.z));
        trans_matrix = glm::rotate(trans_matrix, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
        trans_matrix = glm::translate(trans_matrix, glm::vec3(-anchor.x, -anchor.y, -anchor.z));
        
        this->anchor = glm::vec3(trans_matrix * glm::vec4(this->anchor, 1.0f));
        this->anchor_original = glm::vec3(trans_matrix * glm::vec4(this->anchor, 1.0f));
        for (int i = 0; i < this->control.size(); i++) {
            this->control[i] = glm::vec3(trans_matrix * glm::vec4(this->control[i], 1.0f));
            this->control_original[i] = glm::vec3(trans_matrix * glm::vec4(this->control_original[i], 1.0f));
        }

    }

    void Node::rotate(std::vector<unsigned int> inds, glm::vec3 anchor, float x, float y, float z, float angle) {
        
        // If it has something, apply it and end.
        if (inds.size() > 0) {
            
            // Get the last element.
            int ind = inds.back();
            inds.pop_back();
            
            // Apply this to the next ones.
            this->children[ind].rotate(inds, anchor, x, y, z, angle);
            
            return;
            
        }
        
        // Rotate the geometry.
        this->geometry.translate(-anchor.x, -anchor.y, -anchor.z);
        this->geometry.rotate(x, y, z, angle);
        this->geometry.translate(anchor.x, anchor.y, anchor.z);
        
        // Do the same for its children.
        for (int i = 0; i < this->children.size(); i++)
            this->children[i].rotate(anchor, x, y, z, angle);
        
        // Apply the same transformations to the anchor points and
        glm::mat4 trans_matrix = glm::translate(glm::mat4(1.0), glm::vec3(anchor.x, anchor.y, anchor.z));
        trans_matrix = glm::rotate(trans_matrix, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
        trans_matrix = glm::translate(trans_matrix, glm::vec3(-anchor.x, -anchor.y, -anchor.z));
        
        this->anchor = glm::vec3(trans_matrix * glm::vec4(this->anchor, 1.0f));
        for (int i = 0; i < this->control.size(); i++) {
            this->control[i] = glm::vec3(trans_matrix * glm::vec4(this->control[i], 1.0f));
        }
        
    }

	void Node::scale(float x, float y, float z) {

        geometry.scale(x, y, z);
        
        // Do the same for its children.
        for (int i = 0; i < this->children.size(); i++)
            this->children[i].scale(x, y, z);
        
        // Create the scale matrix.
        glm::mat4 identity_matrix(1.0f);
        glm::mat4 scale_matrix = glm::scale(identity_matrix, glm::vec3(x, y, z));
        
        this->anchor = glm::vec3(scale_matrix * glm::vec4(this->anchor, 1.0));
        this->anchor_original = glm::vec3(scale_matrix * glm::vec4(this->anchor_original, 1.0));
        for (int i = 0; i < this->control.size(); i++) {
            this->control[i] = glm::vec3(scale_matrix * glm::vec4(this->control[i], 1.0));
            this->control_original[i] = glm::vec3(scale_matrix * glm::vec4(this->control_original[i], 1.0));
        }

	}

	void Node::translate(float x, float y, float z) {

        geometry.translate(x, y, z);
        
        // Do the same for its children.
        for (int i = 0; i < this->children.size(); i++)
            this->children[i].translate(x, y, z);
        
        glm::mat4 trans_matrix = glm::translate(glm::mat4(1.0), glm::vec3(x, y, z));
        
        this->anchor = glm::vec3(trans_matrix * glm::vec4(this->anchor, 1.0));
        this->anchor_original = glm::vec3(trans_matrix * glm::vec4(this->anchor_original, 1.0));
        for (int i = 0; i < this->control.size(); i++) {
            this->control[i] = glm::vec3(trans_matrix * glm::vec4(this->control[i], 1.0));
            this->control_original[i] = glm::vec3(trans_matrix * glm::vec4(this->control_original[i], 1.0));
        }

	}

    void Node::transform(glm::mat4 matrix) {
        
        // Resend it using the 0,0,0 anchor to reduce code.
        this->transform(this->getAnchor(), matrix);
        
    }

    void Node::transform(std::vector<unsigned int> inds, glm::mat4 matrix) {
        
        // If it has something, apply it and end.
        if (inds.size() > 0) {
            
            // Get the last element.
            int ind = inds.back();
            inds.pop_back();
            
            // Apply this to the next ones.
            this->children[ind].transform(inds, matrix);
            
            return;
            
        }
        
        // If it doesn't it means that it is destined here.
        this->transform(inds, this->getAnchor(), matrix);
        
    }

    void Node::transform(glm::vec3 anchor, glm::mat4 matrix) {
        
        glm::mat4 trans_matrix = glm::mat4(matrix);
     
        // Apply it to the geometry.
        this->geometry.setTransformMat(glm::mat4(trans_matrix * this->geometry.getTransformMat()));
        
        // Do the same for its children.
        for (int i = 0; i < this->children.size(); i++)
            this->children[i].transform(anchor, matrix);
        
        this->anchor = glm::vec3(trans_matrix * glm::vec4(this->anchor, 1.0f));
        this->anchor_original = glm::vec3(trans_matrix * glm::vec4(this->anchor, 1.0f));
        for (int i = 0; i < this->control.size(); i++) {
            this->control[i] = glm::vec3(trans_matrix * glm::vec4(this->control[i], 1.0f));
            this->control_original[i] = glm::vec3(trans_matrix * glm::vec4(this->control_original[i], 1.0f));
        }
        
    }

    void Node::transform(std::vector<unsigned int> inds, glm::vec3 anchor, glm::mat4 matrix) {
        
        // If it has something, apply it and end.
        if (inds.size() > 0) {
            
            // Get the last element.
            int ind = inds.back();
            inds.pop_back();
            
            // Apply this to the next ones.
            this->children[ind].transform(inds, anchor, matrix);
            
            return;
            
        }
        
        glm::mat4 trans_matrix = glm::mat4(matrix);

        // Apply it to the geometry.
        this->geometry.setTransformMat(glm::mat4(trans_matrix * this->geometry.getTransformMat()));
        
        // Do the same for its children.
        for (int i = 0; i < this->children.size(); i++)
            this->children[i].transform(anchor, matrix);

        this->anchor = glm::vec3(trans_matrix * glm::vec4(this->anchor, 1.0f));
        for (int i = 0; i < this->control.size(); i++) {
            this->control[i] = glm::vec3(trans_matrix * glm::vec4(this->control[i], 1.0f));
        }
        
    }

}  // namespace bgq_opengl
