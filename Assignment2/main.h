/**
 * @file main.h
 * @brief Main header aladdin 3d file.
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

#ifndef BGQ_OPENGL_MAIN_H_
#define BGQ_OPENGL_MAIN_H_

#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 800
#define GAME_NAME "Real-time animation"
#define NORM_SIZE 1.0

#include <vector>
#include <string>
#include <ctime>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "classes/camera/camera.h"
#include "classes/object_hierarchical/object_hierarchical.h"
#include "classes/object/object.h"
#include "classes/shader/shader.h"
#include "classes/texture/texture.h"
#include "classes/turbulence/turbulence.h"
#include "structs/control_point/control_point.h"

bgq_opengl::Camera *camera;                         /// Holds all the existing cameras.
bgq_opengl::Shader *shader;                         /// Holds all the initialized shanders.
bgq_opengl::Shader *shaderPnt;                      /// Holds all the initialized shanders used for the control points.
bgq_opengl::Object *dis_pnt;                        /// The geometry used to display points.
bgq_opengl::ObjectHierarchical *onyx;               /// Holds the onyx.
bgq_opengl::ObjectHierarchical *jelly;              /// Holds the jellyfish.
std::vector<bgq_opengl::ControlPoint> ctrl_pnts;    /// Holds the control points for the joints.
std::vector<bgq_opengl::ControlPoint> ctrl_scrp;    /// Holds the control points for the joints.
int current_scene = 0;                              /// The current scene being loaded.
int moving_joint = -1;                              /// The moving joint control point.
int kinematic = 0;                                  /// The kinematic method used.
int show_ctrl = 1;                                  /// Whether to display the control points or not.
int ik_level = 2;                                   /// The number of bones to take into account in ik.
GLFWwindow *window = 0;						        /// Window ID.
double internal_time = 0;					        /// Time that will rule everything in the game.
bgq_opengl::Light scene_light;                      /// The light in the scene.
float script_amplitude = 0.5f;                     /// The amplitude of the jellyfish motion.
float script_frequency = 0.9f;                      /// The frequency of the jellyfish motion.

const glm::vec4 background(82 / 255.0, 103 / 255.0, 125 / 255.0, 1.0);

/**
 * @brief Clean everything to end the program.
 *
 * Clean everything to end the program.
 */
void clean();

/**
 * @brief Clear the viewport.
 *
 * Clear the viewport before printing anything else.
 */
void clear();

/**
 * @brief Build the geometry we will use to print control points.
 *
 * Build the geometry we will use to display control points.
 */
void buildCube();

/**
 * @brief Display the OpenGL elements.
 *
 * Display all the OpenGL elements in the scene.
 */
void displayElements();

/**
 * @brief Display the GUI.
 *
 * This function will display the GUI elements.
 */
void displayGUI();

/**
 * @brief Display a control point in the scene.
 *
 * Display a control point in the scene.
 *
 * @param ctrl_pnt The control point that will be displayed.
 * @param size The size that will be used for displaying the point.
 */
void displayControlPoint(const bgq_opengl::ControlPoint ctrl_pnt, const float size);

/**
 * @brief Handles the key events.
 *
 * Handles the freeglut key events.
 */
void handleKeyEvents(unsigned char key, int x, int y);

/**
 * @brief Handle the mouse events.
 *
 * Handle the mouse events using GLFW.
 *
 * @param window The OpenGL window ID.
 * @param xpos The X coordinate of the mouse position.
 * @param ypos The X coordinate of the mouse position.
 */
void handleMouseEvents(GLFWwindow* window, double xpos, double ypos);

/**
 * @brief Init the elements of the program
 *
 * Initialize the objects, elements and all.
 */
void initElements();

/**
 * @brief Init the environment.
 *
 * Initialize the OpenGL, Glew and Freeglut environments.
 */
void initEnvironment(int argc, char** argv);

/**
 * @brief Main function.
 * 
 * Main function.
 */
int main(int argc, char** argv);

#endif  //!BGQ_OPENGL_MAIN_H_
