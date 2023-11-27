#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 iResolution;
uniform float iTime;

out vec4 finalColor;




void main()
{
    // distance from center of image, used to adjust blur
    vec2 uv = fragTexCoord.xy;// / iResolution.xy;
    float d = length(uv - vec2(0.5,0.5));

    // blur amount
    float blur = 0.0;
    blur = (1.0 + sin(iTime*2.0)) * 0.125;
    blur *= 1.0 + sin(iTime*8.0) * 0.125;
    blur = pow(blur, 3.0);
    blur *= 0.05;
    // reduce blur towards center
    blur *= d;

    // final color
    vec3 col;
    col.r = texture( texture0, vec2(uv.x+blur,uv.y) ).r;
    col.g = texture( texture0, uv ).g;
    col.b = texture( texture0, vec2(uv.x-blur,uv.y) ).b;

    // scanline
    float scanline = sin(uv.y*2400.0)*0.04;
    col -= scanline*0.6;

    // vignette
    col *= 1.0 - d * 0.4;

    finalColor = vec4(col,1.0);
}
