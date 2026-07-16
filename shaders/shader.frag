#version 330

//Input vertex attributes (tinterpolated from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

//Output color
out vec4 finalColor;

//Input uniform values (provided automatically by raylib)
uniform vec4 colDiffuse;

//Custom uniform (updated from C++ main loop)
uniform float time;

void main(){
    //Distance from center of face (0=center, 1=edge)
    float dx = abs(fragTexCoord.x - 0.5) * 2.0;
    float dy = abs(fragTexCoord.y - 0.5) * 2.0;
    float edge = max(dx, dy);

    //Generate a pulsing multiplier using a sine wave

    //Pulsing intensity
    float pulse = 0.85 + 0.15 * sin(time * 4.0 + (fragTexCoord.x + fragTexCoord.y)* 12.0);

    //Rim glow: brightest at edges
    float glow = pow(edge, 3.0);
    vec4 base = fragColor * colDiffuse;
    //Apply vertex color, base diffuse color, and pulse intensity
    finalColor = base * pulse + vec4(glow * 0.3, glow * 0.1, 0.0, 0.0);
}
