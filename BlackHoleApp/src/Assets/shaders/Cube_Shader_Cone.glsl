#type vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 ViewMat;
uniform mat4 PVMMat;

out vec3 v_lightray;

void main()
{
    v_lightray = position;
    vec4 glpos = PVMMat * vec4(position, 1.0);
    gl_Position = glpos;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 outColor;

uniform vec3 cam_position;
uniform samplerCube u_texture;
uniform float steepness = 0;

in vec3 v_lightray;

#define M_PI 3.1415926535897932384626433832795

vec3 angmom = cross(cam_position, v_lightray);

float r2 = dot(cam_position,cam_position);
float rrdot = dot(cam_position, v_lightray);
float r2thetadot = length(angmom);

float r = length(cam_position);
	
vec3 finalray(float u, float v)
{
    return cam_position*r2thetadot*u + (v_lightray*r2 - cam_position*rrdot)*v;
}

float integrate()
{
	float v0 = 1.0/r;
	float w0 = -rrdot/(r*r2thetadot);
	
	float a = sqrt(steepness*steepness + 1);
	float angle = atan(-v0/w0*a)/a;
	if(w0 > 0)
	{
		angle += M_PI/a;
	}
	return angle;
}

void main()
{
	float angle;
	
	angle = integrate();
	outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
}