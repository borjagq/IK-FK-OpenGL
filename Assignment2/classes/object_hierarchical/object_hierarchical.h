/**
 * @file object.h
 * @brief Object class header file.
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

#ifndef BGQ_OPENGL_CLASSES_OBJECT_HIERARCHICAL_H_
#define BGQ_OPENGL_CLASSES_OBJECT_HIERARCHICAL_H_

#include <vector>
#include <string>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "classes/node/node.h"
#include "structs/bounding_box/bounding_box.h"
#include "structs/control_point/control_point.h"

namespace bgq_opengl {

	class ObjectHierarchical {
	
		public:
			
			/**
			 * @brief Loads a model in the specified format.
			 *
			 * Loads in a model from a file.
			 * 
			 * @param filename The name of the model file.
			 */
            ObjectHierarchical(const char* filename, const std::vector<int> &indexes, const std::vector<int> &parents, const char* first, const char* middle, const char* end);
        
            /**
             * @brief Gets the bounding box.
             *
             * Gets the bounding box of all the geometries.
             *
             * @returns The bounding box struct.
             */
            BoundingBox getBoundingBox();
        
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
			 * @brief Draws this object.
			 *
			 * Draws this object.
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
        
            Node loadNode(const aiScene* scene, const int currentInd, const std::vector<int> &indexes, const std::vector<int> &parents);

			// All the geometries and transformations
            Node root;
            char* first;
            char* middle;
            char* end;

	};

}

#endif  //!BGQ_OPENGL_CLASSES_OBJECT_HIERARCHICAL_H_
