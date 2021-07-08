#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform sampler2D equirectangularMap;

const vec2 constAtan = vec2(0.1591, 0.3183); //const numbers, changing em breaks skybox

/**
Get the uv from the spherical map
    @param _v frags sample direction
*/
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= constAtan;
    uv += 0.5;
    return uv;
}

/**
Main function
*/
void main()
{	
//get uv from the sphere map
    vec2 uv = SampleSphericalMap(normalize(WorldPos));
//get colour by sampling the equirectangular map with that uv
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}