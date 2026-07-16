#version 330

//Input vertex attributes (provided automatically by ralib)
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

//Input uniform values (provided automatically by raylib)
uniform mat4 mvp;

//Output vertex attributes (passed to the fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

void main() {
    fragTexCoord = vertexTexCoord;
    // fragColor = vec4(vertexTexCoord, 0.0, 1.0);
    fragColor = vertexColor;

    //Calculate final vertex position in 3D clip space
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
