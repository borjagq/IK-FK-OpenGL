#version 330 core

layout (location = 0) in vec3 inVertex; // Vertex.
layout (location = 1) in vec3 inNormal; // Normal (not necessarily normalized).
layout (location = 2) in vec3 inColor;  // Color (not necessarily normalized).
layout (location = 3) in vec2 inUV;     // UV coordinates.

uniform mat4 Model;                     // Imports the model matrix.
uniform mat4 View;                      // Imports the View matrix.
uniform mat4 Projection;                // Imports the projection matrix.
uniform mat4 modelView;                 // Imports the modelView already multiplied.
uniform mat4 normalMatrix;              // Imports the normal matrix.
uniform vec3 cameraPosition;            // Position of the camera.

out vec3 vertexNormal;                  // Passes the normal to the fragment shader.
out vec3 vertexColor;                   // Passes the color to the fragment shader.
out vec2 vertexUV;                      // Passes the UV coordinates to the fragment shader.
out vec3 vertexPosition;                // Passes the current vertex to the fragment shader.

void main() {
    
    // Assigns the direct passes.
    vertexNormal = vec3(normalMatrix * vec4(inNormal, 0.0));
    vertexColor = inColor;
    vertexUV = mat2(0.0, -1.0, 1.0, 0.0) * inUV;
    vertexPosition = vec3(modelView * vec4(inVertex, 1.0));
    
    // Sets the visualized position by applying the camera matrix.
    gl_Position = Projection * vec4(vertexPosition, 1.0);
    
}
