attribute vec3 in_Position;
attribute vec2 a_TexCoord;
attribute vec3 a_Normal;
attribute vec3 a_Tangent;
attribute vec3 a_biTangent;

varying vec2 v_TexCoord;
varying vec3 v_Normal;
varying vec3 v_FragPos;
varying vec4 v_Colour; //debug
varying vec3 v_Tangent;
varying vec3 v_biTangent;


uniform mat4 u_Projection;
uniform mat4 u_Model;
uniform mat4 u_View;



void main()	
{	
	//convert frag pos 
	v_FragPos = vec3(u_Model*vec4(in_Position,1.0));
	//set position in world space
	gl_Position = u_Projection * u_View * u_Model * vec4(in_Position, 1.0);
	v_TexCoord = a_TexCoord;


	mat3 modelVector = transpose(inverse(mat3(u_Model)));
  
    vec3 T = (mat3(u_Model) * a_Tangent);
    vec3 B = (mat3(u_Model) * a_biTangent); 
    vec3 N = (mat3(u_Model) * a_Normal);

    v_Normal =  N;
	v_Tangent = T;
	v_biTangent = B; 
}