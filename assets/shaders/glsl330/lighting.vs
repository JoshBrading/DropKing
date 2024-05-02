#version 330

// Input vertex attributes
in vec3 vertexPosition;     // Position of the vertex in 3D space
in vec2 vertexTexCoord;     // Texture coordinates of the vertex
in vec3 vertexNormal;       // Normal vector of the vertex
in vec4 vertexColor;        // Color of the vertex

// Input uniform values
uniform mat4 mvp;           // Model-view-projection matrix
uniform mat4 matModel;      // Model transformation matrix
uniform mat4 matNormal;     // Normal matrix (for transforming normals)

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;      // Position of the vertex in 3D space (to be interpolated and passed to the fragment shader)
out vec2 fragTexCoord;      // Texture coordinates of the vertex (to be interpolated and passed to the fragment shader)
out vec4 fragColor;         // Color of the vertex (to be interpolated and passed to the fragment shader)
out vec3 fragNormal;        // Normal vector of the vertex (to be interpolated and passed to the fragment shader)

// NOTE: Add here your custom variables

void main()
{
    // Transform vertex attributes to be passed to the fragment shader

    // Transform vertex position to world space and then to eye space
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));

    // Pass vertex texture coordinates directly
    fragTexCoord = vertexTexCoord;

    // Pass vertex color directly
    fragColor = vertexColor;

    // Transform vertex normal to world space and normalize it
    fragNormal = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));

    // Calculate final vertex position by applying model-view-projection transformation
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
