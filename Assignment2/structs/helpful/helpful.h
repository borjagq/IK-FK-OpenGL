/**
 * @file helful.h
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

#ifndef BGQ_OPENGL_STRUCTS_HELPFUL_H_
#define BGQ_OPENGL_STRUCTS_HELPFUL_H_

#include "glm/glm.hpp"

#include "classes/camera/camera.h"

namespace bgq_opengl {

    /**
     * @brief A set of helpful functions.
     *
     * A set of helpful functions.
     *
     * @author Borja García Quiroga <garcaqub@tcd.ie>
     */
    struct Helpful {

        /**
         * @brief Unproject a 3d screen point to the 3d world space.
         *
         * Unproject a 3d screen point to the 3d world space.
         *
         * @param screen The screen coordinates as x, y and depth components.
         * @param camera The camera recording the scene.
         */
        static glm::vec3 unProjectScreenToWorld(glm::vec3 screen, Camera camera);
        
        /**
         * @brief Calculates rotation angles as X, Y and Z rotations.
         *
         * Calculates rotation angles as X, Y and Z rotations.
         *
         * @param a The first extreme.
         * @param b The second extreme.
         * @param c The joint.
         *
         * @returns The three angles as a vector of degrees.
         */
        static glm::vec3 get3Angles(glm::vec3 a, glm::vec3 b, glm::vec3 c);
        
        /**
         * @brief Calculates a 2D rotation angles.
         *
         * Calculates a 2D rotation angle.
         *
         * @param a The first extreme.
         * @param b The second extreme.
         * @param c The joint.
         *
         * @returns The angle in degrees.
         */
        static float getAngle(glm::vec2 a, glm::vec2 b, glm::vec2 c);
        
    };

}  // namespace bgq_opengl

#endif  //! BGQ_OPENGL_STRUCTS_HELPFUL_H_
