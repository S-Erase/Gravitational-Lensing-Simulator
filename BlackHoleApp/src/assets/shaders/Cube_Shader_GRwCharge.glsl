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
uniform float r_Q2 = 1;

in vec3 v_lightray;

#define M_PI 3.1415926535897932384626433832795

vec3 angmom = cross(cam_position, v_lightray);

float r2 = dot(cam_position,cam_position);
float rrdot = dot(cam_position, v_lightray);
float lightspeed2 = dot(v_lightray, v_lightray);
float r2thetadot = length(angmom);

float r = length(cam_position);
float r4thetadot2 = dot(angmom,angmom);

float relativisticangmom = (rrdot*rrdot/(r2*r4thetadot2) + (1 - r_s/r + r_Q2/r2)/r2);
	
vec3 finalray(float u, float v)
{
    return cam_position*r2thetadot*u + (v_lightray*r2 - cam_position*rrdot)*v;
}

vec2 ode(vec2 state)
{
	return vec2(state.y, (-2*r_Q2*state.x*state.x + 1.5*r_s*state.x - 1)*state.x);
}

float integrate(float limit)
{
	float v0 = 1/r;
	float w0 = -rrdot/(r*r2thetadot);
	
	vec2 curr_state = vec2(v0, w0), prev_state;
	
	//Runge Kutta
	float h = 0.05, angle = 0;
	vec2 k1, k2, k3, k4;
	while(curr_state.x > 0 && angle < limit)
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

float C(float u) //funtcion -(du/d theta)^2
{
	return ((r_Q2*u - r_s)*u + 1)*u*u - relativisticangmom;
}

void main()
{
	float angle;
	
	if(32*r_Q2 >= 9*r_s*r_s) //One root
	{
		angle = integrate(8*M_PI);
		outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
	}
	else
	{
		float u_minus = (3*r_s - sqrt(9*r_s*r_s - 32*r_Q2))/(8*r_Q2);
		float c_minus = C(u_minus);
		
		if(c_minus < 0) //One root
		{
			outColor = vec4(0.5, 0.0, 0.0, 1.0); //Not fixed
			angle = integrate(24*M_PI);
			outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
		}
		else if(c_minus == 0) //Two roots
		{
			outColor = vec4(0.5, 0.0, 0.0, 1.0); //Not fixed
		}
		else
		{
			float c_plus = C((3*r_s + sqrt(9*r_s*r_s - 32*r_Q2))/(8*r_Q2));
			
			if(c_plus < 0) //Three roots
			{
				if(1 > r*u_minus) // u > u_minus
				{
					outColor = vec4(0.0, 0.0, 0.0, 1.0);
				}
				else{
					angle = integrate(8*M_PI);
					outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
				}
			}
			else if(c_plus == 0) //Two roots
			{
				outColor = vec4(0.5, 0.0, 0.0, 1.0); //Not fixed
			}
			else //One root
			{
				angle = integrate(8*M_PI);
				outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
			}
		}
	}
}