#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;       // Position of the fragment in 3D space
in vec2 fragTexCoord;       // Texture coordinates of the fragment
in vec3 fragNormal;         // Normal vector of the fragment

// Input uniform values
uniform sampler2D texture0; // Texture sampler
uniform vec4 colDiffuse;    // Diffuse color of the fragment

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

// Constants and definitions
#define MAX_LIGHTS 4                   // Maximum number of lights
#define LIGHT_DIRECTIONAL 0           // Type constant for directional light
#define LIGHT_POINT 1                  // Type constant for point light

// Struct to represent a light source
struct Light {
    int enabled;                     // Whether the light is enabled (1 for enabled, 0 for disabled)
    int type;                        // Type of light (directional or point)
    vec3 position;                   // Position of the light source
    vec3 target;                     // Directional target (for directional lights)
    vec4 color;                      // Color of the light
};

// Input lighting values
uniform Light lights[MAX_LIGHTS];    // Array of light sources
uniform vec4 ambient;                // Ambient light color
uniform vec3 viewPos;                // Position of the viewer/camera

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 lightDot = vec3(0.0);        // Accumulator for diffuse lighting
    vec3 normal = normalize(fragNormal); // Normalize the fragment's normal vector
    vec3 viewD = normalize(viewPos - fragPosition); // Direction from fragment to viewer
    vec3 specular = vec3(0.0);        // Accumulator for specular lighting

    // NOTE: Implement here your fragment shader code

    // Loop through each light source
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        // Check if the light source is enabled
        if (lights[i].enabled == 1)
        {
            vec3 light = vec3(0.0);   // Initialize light vector

            // Calculate light direction based on light type
            if (lights[i].type == LIGHT_DIRECTIONAL)
            {
                light = -normalize(lights[i].target - lights[i].position); // Directional light
            }

            if (lights[i].type == LIGHT_POINT)
            {
                light = normalize(lights[i].position - fragPosition); // Point light
            }

            // Calculate the dot product of the normal and light direction
            float NdotL = max(dot(normal, light), 0.0);

            // Accumulate diffuse lighting
            lightDot += lights[i].color.rgb * NdotL;

            // Calculate specular reflection
            float specCo = 0.0;
            if (NdotL > 0.0)
                specCo = pow(max(0.0, dot(viewD, reflect(-(light), normal))), 16.0); // 16 refers to shine
            specular += specCo; // Accumulate specular lighting
        }
    }

    // Combine lighting contributions with texture color and ambient light
    finalColor = (texelColor * ((colDiffuse + vec4(specular, 1.0)) * vec4(lightDot, 1.0)));
    finalColor += texelColor * (ambient / 10.0) * colDiffuse;

    // Gamma correction
    finalColor = pow(finalColor, vec4(1.0 / 2.2));
}
