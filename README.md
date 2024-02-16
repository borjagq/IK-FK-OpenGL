# Inverse and Forward Kinematics in OpenGL

Implements forward, inverse, and scripted kinematics using hierarchichal creatures in OpenGL.

## About this repo

This project explores forward, inverse, and scripted kinematics for hierarchical creature modeling in OpenGL. It features two different boned creatures: a worm-like creature featuring five interconnected bones, and a cube jellyfish with four three-boned arms. Both models have been crafted in Blender and loaded with the Assimp library. The control points and joints are calculated within the code, as the models do not incorporate them.

Interactions with the control points are calculated by projecting points onto the screen and calculating distances relative to mouse pixel coordinates. For forward kinematics, bone rotation angles are determined by control points, converted into quaternions, and applied seamlessly using vectors.

Inverse kinematics employ the CCD algorithm, offering flexibility with a variable number of bones. The algorithm's termination criteria account for minimal distances and iteration limits, ensuring precision in reaching specified targets. This project displays a sophisticated behavior with up to five interconnected bones in three dimensions. Control point movement in three dimensions necessitates the creation of an imaginary plane mathematically defined by the vector from the scene center to the camera, allowing for unrestricted movement.

The final section addresses staged jellyfish animation using inverse kinematics, employing mathematical functions to propel control points along continuous splines. Initial attempts at forward kinematics lead to desynchronization issues, prompting a shift to this approach combined with sinusoid functions for enhanced realism. Additional out-of-phase functions accentuate resistance, creating a visually appealing illusion of tentacles gliding through water.

## Demo video

[![Watch the video](https://img.youtube.com/vi/SMO3cQ4DK7Y/0.jpg)](https://www.youtube.com/watch?v=SMO3cQ4DK7Y)

## Getting Started

### Dependencies

- [Xcode 14.3.1](https://developer.apple.com/xcode/)
- OpenGL 4.1 (included in macOS)
- [Glew 2.2.0](https://glew.sourceforge.net)
- [GLFW 3.3.8](https://github.com/glfw/glfw)
- [Assimp 5.3.1](https://github.com/assimp/assimp)
- [GLM 1.0.0](https://github.com/g-truc/glm)

### Installing

1. Clone this repository.

```sh
git clone https://github.com/borjagq/IK-FK-OpenGL/
```

2. Open the project ```Assignment2.xcodeproj```

3. Build and run

# License

This project is licensed under the MIT License - see the [LICENSE](https://github.com/borjagq/IK-FK-OpenGL/LICENSE/) file for details

# Acknowledgments

This repository incorporates essential components from the open-source libraries [stb](https://github.com/nothings/stb) and [Dear ImGui](https://github.com/ocornut/imgui). These libraries provide the file read/write and GUI functionalities.  For further details about these libraries, visit their GitHub repositories.
