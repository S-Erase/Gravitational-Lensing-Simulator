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
	float w02 = 2.25*r_s*r_s*rrdot*rrdot/(r4thetadot2*r2);
	
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
		float D = 27*r_s*r_s*(r*rrdot*rrdot + (r-r_s)*r4thetadot2)/(4*r2*r*r4thetadot2);
		float angle;
		if(2*r > 3*r_s){
			if(r_s == 0){
				outColor = texture(u_texture, v_lightray);
			}else if(D<1){
				angle = integrate();
				outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
			}else{
				if(rrdot>0){
					angle = integrate();
					outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
				}else{
					outColor = vec4(0.0, 0.0, 0.0, 1.0);
				}
			}
		}else if(2*r == 3*r_s){
			if(D==1){
				outColor = vec4(0.0, 0.0, 0.0, 1.0);
			}else{
				if(rrdot>0){
					angle = integrate();
					outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
				}else{
					outColor = vec4(0.0, 0.0, 0.0, 1.0);
				}
			}
		}else{
			if(D>1){
				if(rrdot>0){
					angle = integrate();
					outColor = texture(u_texture, finalray(cos(angle),sin(angle)));
				}else{
					outColor = vec4(0.0, 0.0, 0.0, 1.0);
				}
			}else{
			    outColor = vec4(0.0, 0.0, 0.0, 1.0);
			}
		}
}