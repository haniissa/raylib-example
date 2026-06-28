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
    //Generate a pulsing multiplier using a sine wave
    float pulse = 0.85 + 0.15 * sin(time * 5.0);

    //Apply vertex color, base diffuse color, and pulse intensity
    finalColor = fragColor * colDiffuse * vec4(pulse, pulse, pulse, 1.0);
}
