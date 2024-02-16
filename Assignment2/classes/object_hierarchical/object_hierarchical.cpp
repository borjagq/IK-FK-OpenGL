/**
 * @file object.cpp
 * @brief Object class implementation file.
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

#include "object_hierarchical.h"

#include <cassert>
#include <iostream>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "classes/loader/loader.h"
#include "classes/node/node.h"
#include "structs/vertex/vertex.h"
#include "structs/bounding_box/bounding_box.h"
#include "structs/control_point/control_point.h"

namespace bgq_opengl {

ObjectHierarchical::ObjectHierarchical(const char *filename, const std::vector<int> &indexes, const std::vector<int> &parents, const char* first, const char* middle, const char* end) : root(0, {}, {}, {}, {}) {

        // Import the scene from the file.
        const aiScene* scene = aiImportFile(filename, aiProcess_Triangulate | aiProcess_PreTransformVertices);

        // Check if the scene was not read correctly.
        if (!scene) {
            
            std::cerr << "Could not read mesh on file " << filename << std::endl;
            exit(1);
            
        }

        // Print info from the scene.
        std::cerr << "  " << filename << std::endl;
        std::cerr << "  " << scene->mNumMaterials << " materials" << std::endl;
        std::cerr << "  " << scene->mNumMeshes << " meshes" << std::endl;
        std::cerr << "  " << scene->mNumTextures << " textures" << std::endl;
        
        // Load this node.
        this->root = loadNode(scene, 0, indexes, parents);
    
        // Load the control points.
        this->first = strdup(first);
        this->middle = strdup(middle);
        this->end = strdup(end);
        this->root.calculateControlPoints(first, middle, end);

        aiReleaseImport(scene);

	}

    BoundingBox ObjectHierarchical::getBoundingBox() {

        return this->root.getBoundingBox();

    }

    std::vector<ControlPoint> ObjectHierarchical::getControlPoints() {
        
        return this->root.getControlPoints();
        
    }

    std::vector<ControlPoint> ObjectHierarchical::getOrderedControlPoints(const int num_ik, ControlPoint ctrl) {
        
        return this->root.getOrderedControlPoints(num_ik, ctrl);
        
    }

	void ObjectHierarchical::draw(bgq_opengl::Shader& shader, bgq_opengl::Camera& camera) {
        
        this->root.draw(shader, camera);
        
	}

	void ObjectHierarchical::resetTransforms() {

        root.resetTransforms();

	}

    void ObjectHierarchical::rotate(float x, float y, float z, float angle) {
        
        this->root.rotate(glm::vec3(0.0f, 0.0f, 0.0f), x, y, z, angle);
        
    }

    void ObjectHierarchical::rotate(std::vector<unsigned int> inds, float x, float y, float z, float angle) {
        
        this->root.rotate(inds, x, y, z, angle);
        
    }

    void ObjectHierarchical::rotate(glm::vec3 anchor, float x, float y, float z, float angle) {
        
        this->root.rotate(anchor, x, y, z, angle);
        
    }

    void ObjectHierarchical::rotate(std::vector<unsigned int> inds, glm::vec3 anchor, float x, float y, float z, float angle) {
        
        this->root.rotate(inds, anchor, x, y, z, angle);
        
    }

	void ObjectHierarchical::scale(float x, float y, float z) {

		this->root.scale(x, y, z);

	}

	void ObjectHierarchical::translate(float x, float y, float z) {

        /*
		// Do the same for the subobjects.
		for (size_t i = 0; i < geoms.size(); i++) {

			geoms[i].translate(x, y, z);

		}
         */

	}
    
    Node ObjectHierarchical::loadNode(const aiScene* scene, const int currentInd, const std::vector<int> &node_indexes, const std::vector<int> &parents) {
        
        // Check there are the same indexes and parents.
        assert(node_indexes.size() == parents.size());
        
        // Init the children.
        std::vector<Node> children = {};
        
        // Load the children by iterating through all meshes and checking who are children of this.
        for (int i = 0; i < parents.size(); i++) {
                        
            if (parents[i] == currentInd) {
                                
                children.push_back(loadNode(scene, i, node_indexes, parents));
                
            }
            
        }
        
        // Get this mesh from assimp.
        const aiMesh* mesh = scene->mMeshes[node_indexes[currentInd]];

        // Init the ds.
        std::vector<bgq_opengl::Vertex> vertices(0);
        std::vector<GLuint> indices(0);
        
        // If has materials, pass them.
        aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D color;
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        
        // Iterate through the vertices in the mesh.
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            
            // Build an empty vertex.
            bgq_opengl::Vertex vertex{
                glm::vec3(0.0f, 0.0f, 0.0f),    // Position.
                glm::vec3(1.0f, 1.0f, 1.0f),    // Normal.
                glm::vec3(0.0f, 0.0f, 1.0f),    // Color.
                glm::vec2(0.0f, 0.0f),          // UV coords.
                glm::vec3(0.0f, 1.0f, 0.0f),    // Tangente.
                glm::vec3(1.0f, 0.0f, 0.0f)     // Bitangente.
            };
            
            // If it has a position, subtitute it in the vertex.
            if (mesh->HasPositions()) {
                
                const aiVector3D* vp = &(mesh->mVertices[i]);
                vertex.position = glm::vec3(vp->x, vp->y, vp->z);
                
            }
            
            vertex.color = glm::vec3(color.r, color.g, color.b);
            
            // If it has a normal, subtitute it in the vertex.
            if (mesh->HasNormals()) {
                
                const aiVector3D* vn = &(mesh->mNormals[i]);
                vertex.normal = glm::vec3(vn->x, vn->y, vn->z);
                
            }
            
            // If it has UV textures, subtitute it in the vertex.
            if (mesh->HasTextureCoords(0)) {
                                
                const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
                vertex.uv = glm::vec2(-vt->y, vt->x);
                
            }
            
            // Check if it has tangents.
            if (mesh->HasTangentsAndBitangents()) {
                
                const aiVector3D* tangent = &(mesh->mTangents[i]);
                vertex.tangent = glm::vec3(tangent->x, tangent->y, tangent->z);
                
                const aiVector3D* bitangent = &(mesh->mBitangents[i]);
                vertex.bitangent = glm::vec3(bitangent->x, bitangent->y, bitangent->z);
                                
            }
            
            // Add that to vertices.
            vertices.push_back(vertex);

        }
        
        // Lets store all the indices or faces.
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            
            indices.push_back(mesh->mFaces[j].mIndices[0]);
            indices.push_back(mesh->mFaces[j].mIndices[1]);
            indices.push_back(mesh->mFaces[j].mIndices[2]);
        
        }

        // Obtain the textures.
        std::vector<bgq_opengl::Texture> textures = {};
                
        return Node(currentInd, vertices, indices, textures, children);

    }

    void ObjectHierarchical::transform(glm::mat4 matrix) {
        
        this->root.transform(glm::vec3(0.0f), matrix);
        
    }

    void ObjectHierarchical::transform(std::vector<unsigned int> inds, glm::mat4 matrix) {
        
        this->root.transform(inds, matrix);
        
    }

    void ObjectHierarchical::transform(glm::vec3 anchor, glm::mat4 matrix) {
        
        this->root.transform(anchor, matrix);
        
    }

    void ObjectHierarchical::transform(std::vector<unsigned int> inds, glm::vec3 anchor, glm::mat4 matrix) {
        
        this->root.transform(inds, anchor, matrix);
        
    }

}
