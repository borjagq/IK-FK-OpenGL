/**
 * @file control_point.h
 * @brief BoundingBox struct header file.
 * @version 1.0.0 (2023-01-02)
 * @date 2023-01-02
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#ifndef BGQ_OPENGL_STRUCT_CONTROLPOINT_H_
#define BGQ_OPENGL_STRUCT_CONTROLPOINT_H_

#include "glm/glm.hpp"

namespace bgq_opengl {

    /**
     * @brief A control  point struct.
     *
     * This Struct represents the control point of an object.
     */
    struct ControlPoint {

        glm::vec3 coords;                   /// The 3d coordinates.
        glm::vec3 initial;                  /// The 3d coordinates.
        glm::vec3 anchor;                   /// The 3d coordinates of the anchor.
        std::vector<unsigned int> indices;  /// The indices leading to this node.

    };

} // namespace bgq_opengl

#endif //!BGQ_OPENGL_STRUCT_CONTROLPOINT_H_
