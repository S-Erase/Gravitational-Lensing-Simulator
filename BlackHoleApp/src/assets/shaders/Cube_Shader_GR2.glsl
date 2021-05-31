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
uniform float r_s = 3;

in vec3 v_lightray;

#define M_PI 3.1415926535897932384626433832795

vec3 angmom = cross(cam_position, v_lightray);

float r2 = dot(cam_position,cam_position);
float rrdot = dot(cam_position, v_lightray);
float lightspeed2 = dot(v_lightray, v_lightray);
float r2thetadot = length(angmom);

float r = length(cam_position);
float r4thetadot2 = dot(angmom,angmom);

float relativisticangmom = (rrdot*rrdot/(r2*r4thetadot2) + (1 - r_s/r)/r2);
	
vec3 finalray(float u, float v)
{
    return cam_position*r2thetadot*u + (v_lightray*r2 - cam_position*rrdot)*v;
}

vec2 ode(vec2 state)
{
	return vec2(state.y, (state.x - 1)*state.x);
}

float integrate()
{
	float v0 = 1.5*r_s/r;
	float w0 = -1.5*r_s*rrdot/(r*r2thetadot);
	
	vec2 curr_state = vec2(v0, w0), prev_state;
	
	//Runge Kutta
	float h = 0.05, angle = 0;
	vec2 k1, k2, k3, k4;
	while(curr_state.x > 0 && angle < 8*M_PI)
	{
		k1 = ode(curr_state);
		k2 = ode(curr_state + k1*h*0.5);
		k3 = ode(curr_state + k2*h*0.5);
		k4 = ode(curr_state + k3*h);
		
		prev_state = curr_state;
		curr_state = curr_state + h*(k1+2*k2+2*k3+k4)/6;
		angle = angle+h;
	}
	
	return angle - h*curr_state.x/(curr_state.x-prev_state.x);
}

void main()
{
	float angle;
	
	float c_max = 4/(27*r_s*r_s) - relativisticangmom;
	if(r_s == 0)
	{
		outColor = texture(u_texture, v_lightray);
	}
	else if(c_max >= 0)
	{
		if(r < 3*r_s/2)
		{
			outColor = vec4(0.0, 0.0, 0.0, 1.0);
		}
		else if(r > 3*r_s/2)
		{
			angle = integrate();
			outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
		}
		else if(rrdot <= 0)
		{
			outColor = vec4(0.0, 0.0, 0.0, 1.0);
		}
		else
		{
			angle = integrate();
			outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
		}
	}
	else
	{
		if(rrdot <= 0)
		{
			outColor = vec4(0.0, 0.0, 0.0, 1.0);
		}
		else
		{
			angle = integrate();
			outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
		}
	}
}