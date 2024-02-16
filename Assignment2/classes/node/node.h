/**
 * @file geometry.h
 * @brief Geometry class header file.
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

#ifndef BGQ_OPENGL_CLASSES_NODE_H_
#define BGQ_OPENGL_CLASSES_NODE_H_

#include <vector>
#include <string>

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "classes/camera/camera.h"
#include "classes/geometry/geometry.h"
#include "classes/shader/shader.h"
#include "classes/texture/texture.h"
#include "classes/ebo/ebo.h"
#include "classes/vbo/vbo.h"
#include "classes/vao/vao.h"
#include "structs/vertex/vertex.h"
#include "structs/bounding_box/bounding_box.h"
#include "structs/control_point/control_point.h"

namespace bgq_opengl {

	/**
	* @brief Implementation of a Geometry class.
	*
	* Implementation of a Geometry class that will allow us to handle the geometric part
	* of the objects in the VBOs.
	*
	* @author Borja García Quiroga <garcaqub@tcd.ie>
	*/
	class Node {

		public:
			
			/**
			 * @brief Initializes the Geometry.
			 * 
			 * Initializes the geometry and stores it.
			 * 
			 * @param vertices Vertices of the object.
			 * @param indices Indices of the vertices.
			 * @param textures Textures in connection with this geometry.
			 */
			Node(const int id, const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures, const std::vector<Node> &children);
        
            /**
             * @brief Get the anchor point of this node.
             *
             * Get the anchor point of this node.
             */
            glm::vec3 getAnchor();
        
            /**
             * @brief Get the control points for this object.
             *
             * Get the control points for this object.
             *
             * @returns A vector of vectors containing those points.
             */
            std::vector<ControlPoint> getControlPoints();
        
            /**
             * @brief Get the control points ready for IK computing.
             *
             * Get the control ready for IK computing.
             *
             * @param num_ik The number of control points to return.
             * @param ctrl The control point to track to reach the specific node.
             *
             * @returns A vector of vectors containing those points.
             */
            std::vector<ControlPoint> getOrderedControlPoints(const int num_ik, ControlPoint ctrl);


            /**
             * @brief Calculates the anchor and control points.
             *
             * Calculates the anchor and control points of the nodes.
             *
             * @param first The method for the current node.
             * @param middle The method that will use for intermediate points.
             * @param end The method for extremities.
             */
            void calculateControlPoints(const char* first, const char* middle, const char* end);
        
            /**
             * @brief Calculates the anchor and control points using the parent.
             *
             * Calculates the anchor and control points of the nodes using the parent.
             *
             * @param parentGeom The parent geometry.
             * @param middle The method that will use for intermediate points.
             * @param end The method for extremities.
             */
            void calculateControlPoints(const Geometry &parentGeom, const char* middle, const char* end);
        
            /**
             * @brief Gets the bounding box.
             *
             * Gets the bounding box of all the geometries.
             *
             * @returns The bounding box struct.
             */
            BoundingBox getBoundingBox();
        
            /**
             * @brief Gets the geometry.
             *
             * Gets the geometry.
             *
             * @returns The geometry.
             */
            Geometry getGeometry();

			/**
			 * @brief Draws the Geometry.
			 *
			 * Displays the Geometry in OpenGL.
			 */
			void draw(Shader &shader, Camera &camera);

			/**
			 * @brief Reset
			 *
			 * Add a translation matrix to the model.
			 */
			void resetTransforms();

            /**
             * @brief Add a translation matrix to the model.
             *
             * Add a translation matrix to the model.
             *
             * @param x The x rotation.
             * @param y The y rotation.
             * @param z The z rotation.
             * @param angle The angle to rotate.
             */
            void rotate(float x, float y, float z, float angle);

            /**
             * @brief Add a translation matrix to the model.
             *
             * Add a translation matrix to the model.
             *
             * @param inds The indices to reach the specific node.
             * @param x The x rotation.
             * @param y The y rotation.
             * @param z The z rotation.
             * @param angle The angle to rotate.
             */
            void rotate(std::vector<unsigned int> inds, float x, float y, float z, float angle);
        
            /**
             * @brief Add a translation matrix to the model.
             *
             * Add a translation matrix to the model.
             *
             * @param anchor The anchor point the object will be rotated around.
             * @param x The x rotation.
             * @param y The y rotation.
             * @param z The z rotation.
             * @param angle The angle to rotate.
             */
            void rotate(glm::vec3 anchor, float x, float y, float z, float angle);
        
            /**
             * @brief Add a translation matrix to the model.
             *
             * Add a translation matrix to the model.
             *
             * @param inds The indices to reach the specific node.
             * @param anchor The anchor point the object will be rotated around.
             * @param x The x rotation.
             * @param y The y rotation.
             * @param z The z rotation.
             * @param angle The angle to rotate.
             */
            void rotate(std::vector<unsigned int> inds, glm::vec3 anchor, float x, float y, float z, float angle);

			/**
			 * @brief Add a translation matrix to the model.
			 *
			 * Add a translation matrix to the model.
			 *
			 * @param x The x scale.
			 * @param y The y scale.
			 * @param z The z scale.
			 */
			void scale(float x, float y, float z);

			/**
			 * @brief Add a translation matrix to the model.
			 *
			 * Add a translation matrix to the model.
			 *
			 * @param x The x translation.
			 * @param y The y translation.
			 * @param z The z translation.
			 */
			void translate(float x, float y, float z);
        
            /**
             * @brief Add a transform matrix to the model.
             *
             * Add a transform matrix to the model.
             *
             * @param matrix The new matrix to add.
             */
            void transform(glm::mat4 matrix);

            /**
             * @brief Add a transform matrix to the model.
             *
             * Add a transform matrix to the model.
             *
             * @param inds The indices to reach the specific node.
             * @param matrix The new matrix to add.
             */
            void transform(std::vector<unsigned int> inds, glm::mat4 matrix);
        
            /**
             * @brief Add a transform matrix to the model.
             *
             * Add a transform matrix to the model.
             *
             * @param anchor The anchor point the object will be rotated around.
             * @param matrix The new matrix to add.
             */
            void transform(glm::vec3 anchor, glm::mat4 matrix);
        
            /**
             * @brief Add a transform matrix to the model.
             *
             * Add a transform matrix to the model.
             *
             * @param inds The indices to reach the specific node.
             * @param anchor The anchor point the object will be rotated around.
             * @param matrix The new matrix to add.
             */
            void transform(std::vector<unsigned int> inds, glm::vec3 anchor, glm::mat4 matrix);

		private:

			/**
			 * @brief Updates the normal matrix.
			 * 
			 * Updates the normal matrix so it can be passed to the shader.
			 */
			void updateNormalMatrix();

            int node_id;                                    /// The id representing this node.
            Geometry geometry;                              /// The geometry corresponding to the node.
            std::vector<Node> children;                     /// The children nodes.
			glm::mat4 transforms = glm::mat4(1.0f);		    /// Tranform matrixes that will be passed to the shader.
            glm::vec3 anchor = glm::vec3(0.0f);             /// The anchor point of the node.
            glm::vec3 anchor_original = glm::vec3(0.0f);    /// The original anchor point of the node.
            std::vector<glm::vec3> control;                 /// The control points of this node.
            std::vector<glm::vec3> control_original;        /// The original ontrol points of this node.

	};

}  // namespace bgq_opengl

#endif  //! BGQ_OPENGL_CLASSES_NODE_H_
