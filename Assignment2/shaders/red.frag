#version 330 core

in vec3 vertexPosition;             // Position from the VS.
in vec3 vertexNormal;               // Normal from the VS.
in vec3 vertexColor;                // Color from the VS.
in vec2 vertexUV;                   // UV coordinates from the VS.

uniform vec4 lightColor;            // Light color.
uniform vec3 lightPos;              // Light position.
uniform vec3 cameraPosition;        // Position of the camera.

const float minAmbientLight = 0.25; // The minimum ambient that will control ambient light.
const float lightPower = 10.0;      // The power that light has to light up the scene.
const float shininess = 100.0;      // The material shininess.
const float screenGamma = 2.2;      // Used for gamma corrections.

out vec4 outColor;                  // Outputs color in RGBA.

void main() {

    // Final color.
    outColor = vec4(0.5, 0.25, 0.8, 1.0);
            
}
