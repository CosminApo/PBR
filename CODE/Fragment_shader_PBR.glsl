#version 330 core
out vec4 FragColor;
uniform sampler2D u_Texture;
varying vec2 v_TexCoord;
varying vec3 v_FragPos;
varying vec3 v_Normal;
varying vec4 v_Colour;//debug
varying vec3 v_Tangent;
varying vec3 v_biTangent;

uniform vec3 u_ViewPos;
uniform vec3 u_LightPos;
uniform vec3 u_LightPositions[4];

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_AoMap;
uniform sampler2D u_BrdfLUT;  
uniform samplerCube u_IrrMap;
uniform samplerCube u_PrefilterMap;

float PI = 3.1415926538;

float specularStrength = 0.5;

/**
Distribution function of Cook torrance BRDF
    @param H the halfway vector
    @param N the normal vector
    @param roughness the roughness of the current position
*/
float distributionFunction(vec3 H, vec3 N, float roughness)
{
    float rtn = 0.0;
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N,H),0);
    float NdotH2 = NdotH*NdotH;
    float denom = (NdotH2)*(a2 - 1.0)+1.0;
    float denom2 = denom * denom;
    rtn = a2 / (PI * denom2);

    return rtn;
}

/**
Fresnel function of Cook torrance BRDF
    @param cosTheta the angle obtained from the product of the view vector and normal vector
    @param F0 reflection index
*/
vec3 fresnelFunction(float cosTheta, vec3 F0)
{
    vec3 rtn = vec3(1,1,1);
  
    rtn = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);

    return rtn;
}

/**
Fresnel function of Cook torrance BRDF, but accounts for roughness
    @param cosTheta the angle obtained from the product of the view vector and normal vector
    @param F0 reflection index
    @param roughness roughness of the current position
*/
vec3 fresnelFunctionRough(float cosTheta, vec3 F0, float roughness)
{
    vec3 rtn = vec3(1,1,1);
  
    rtn = F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);

    return rtn;
}


/**
Geometry function part 2 of Cook torrance BRDF
    @param xdotV either NdotL or NdotV
    @param k roughness
*/
float geometry_SchlickGGX(float xdotV, float k)
{
    float rtn = 0.0;
    float r = (k + 1.0); //remove this
    float x = (r*r) / 8.0; //nd this
    rtn = xdotV / (xdotV*(1.0-x)+x);
    return rtn;
}


/**
Geometry function of Cook torrance BRDF
    @param N normal vector
    @param V view vector
    @param L light vector
    @param k roughness
*/
float geometryFunction(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N,V),0);
    float NdotL = max(dot(N,L),0);
    float ggx1 = geometry_SchlickGGX(NdotV, k);
    float ggx2 = geometry_SchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}


/**
Returns a normal in world space from a normal map
*/
vec3 getNormalFromNormalMap()
{
//NOTE: THIS FUNC CAN CHANGE TO HAVE CALCULATION IN VERTEX SHADER FOR BETTER PERFORMANCE

    vec3 tangentNormal = texture(u_NormalMap, v_TexCoord).rgb * 2.0 - 1.0;
    vec3 N   = normalize(v_Normal);
    vec3 T = normalize(v_Tangent);
    vec3 B = normalize(v_biTangent);
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

/**
Main draw function
*/
void main()
{
    /*Converted material input*/
    vec3 albedo = pow(texture(u_AlbedoMap, v_TexCoord).rgb, vec3(2.2,2.2,2.2)); //this be weird
    vec3 normal = getNormalFromNormalMap();
    float metallic = texture(u_MetallicMap, v_TexCoord).r;
    float roughness = texture(u_RoughnessMap, v_TexCoord).r;
    float Ao = texture(u_AoMap, v_TexCoord).r;

    /*Calculation terms*/
    vec3 N = normal; // normal vector
    vec3 V = normalize(u_ViewPos-v_FragPos); // view vector
    vec3 F0 = vec3(0.04); //approximated to 0.04 as standard value
    vec3 Lo = vec3(0.0); //will contain the sum of radiance
    vec3 L = vec3(0,0,0); // light vector
    vec3 H = vec3(1,1,1); // half way vector
    vec3 radiance = vec3(0,0,0);
    vec3 lightColour = vec3(200,200,200); //universal light colour 
    vec3 fresnelTerm = vec3(1,1,1);
    vec3 kD = vec3(1,1,1);
    vec3 kS = vec3(1,1,1);
    vec3 R = reflect(-V, N);
   
    float distribTerm = 0.0; 
    float geometryTerm = 0.0;
    float NdotL = 0.0;
    float distance = 0.0;
    float attenuation = 0.0; //to be continued <-
    int steps = 4;
    F0 = mix(F0, albedo, metallic);

    //assumming his world pos = my frag pos
    for (int i = 0; i < steps; i++)
    {
    //per light radiance
        L = normalize(u_LightPositions[i] - v_FragPos);
        H = normalize(V+L);
        distance = length(u_LightPositions[i] - v_FragPos);
        attenuation = 1.0 / (distance * distance);
        radiance = lightColour * attenuation;
    //BRDF -> cook-torrance
        distribTerm = distributionFunction(N, H, roughness);
        geometryTerm = geometryFunction(N, V, L, roughness);
        fresnelTerm = fresnelFunction(max(dot(N, V), 0.0), F0);

        kS = fresnelTerm;
        kD = vec3(1.0) - kS;
        kD *=1.0 - metallic;

        vec3 num = fresnelTerm * geometryTerm * distribTerm;
        float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = num / max(denom, 0.001);

     //add to total radiance
        NdotL = max(dot(N, L), 0.0);
        Lo = (kD * albedo / PI + specular) * radiance * NdotL;
    }


    kS = fresnelFunctionRough(max(dot(N, V), 0.0), F0, roughness);
    kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = texture(u_IrrMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(u_PrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb; 
    vec2 envBRDF  = texture(u_BrdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD*diffuse+specular) * Ao;
    vec3 colour = ambient + Lo;
    colour = colour / (colour + vec3(1.0));
    colour = pow(colour, vec3(1.0/2.2)); 

   FragColor = vec4(colour, 1.0);
}