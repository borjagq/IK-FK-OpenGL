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
    
    // Get the normal ready to use.
    vec3 normal = normalize(vertexNormal);
    
    // Get the light direction.
    vec3 lightDir = lightPos - vertexPosition;

    // Get the distance from the light to this fragment.
    float dist = length(lightDir);
    dist = dist * dist;

    // Normalize the light direction as a vector.
    lightDir = normalize(lightDir);

    // Get the lambertian component as stated in the docs.
    float lambertian = max(dot(lightDir, normal), 0.0);
    
    // Get the surface color from the vertex color.
    vec4 surfaceColor = vec4(vertexColor, 1.0);
    
    // Calculate the specular component.
    float specular = 0.0;
    if (lambertian > 0.0) {

        // Get the direction from the position to the camera as a vector.
        vec3 viewDir = normalize(-vertexPosition);

        // Blinn-phong calculations.
        vec3 halfAngle = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfAngle, normal), 0.0);
        specular = pow(specAngle, shininess);
       
    }
    
    // Get the minimum color.
    vec3 ambientColor = vec3(surfaceColor) * minAmbientLight;

    // Get the diffuse final color.
    vec3 diffuseColor = vec3(surfaceColor) * lambertian * vec3(lightColor) * lightPower / dist;

    // Get the specular final color.
    vec3 specularColor = vec3(surfaceColor) * specular * vec3(lightColor) * lightPower / dist;

    // Get the final color that would go in the fragment.
    vec3 fragmentColor = ambientColor + diffuseColor + specularColor;

    // Apply gamma correction.
    fragmentColor = pow(fragmentColor, vec3(1.0 / screenGamma));

    // Final color.
    outColor = vec4(fragmentColor, 1.0);
        
}
