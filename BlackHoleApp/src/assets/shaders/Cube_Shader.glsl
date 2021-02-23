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
uniform int gravity_mode = 0;

in vec3 v_lightray;

#define M_PI 3.1415926535897932384626433832795
//vec3 normallight = v_lightray;
vec3 normallight = v_lightray;

vec3 angmom;

float r2 = dot(cam_position,cam_position);
float rrdot;
float lightspeed2;
float r2thetadot;

float r = length(cam_position);
float r4thetadot2;
	
vec3 finalray(float u, float v)
{
    return cam_position*r2thetadot*u + (normallight*r2 - cam_position*rrdot)*v;
}

vec3 rotateray(float u, float v)
{
	return normallight*r2thetadot*u + (normallight*rrdot - cam_position*lightspeed2)*v;
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
	if(gravity_mode == 1)
	{
		normallight = normalize(v_lightray);
	}

	angmom = cross(cam_position, normallight);

	rrdot = dot(cam_position, normallight);
	lightspeed2 = dot(normallight, normallight);
	r2thetadot = length(angmom);

	r4thetadot2 = dot(angmom,angmom);

    if(gravity_mode == 0) //Flat Space
	{
		float dr2_dtheta2 = r2*rrdot*rrdot/r4thetadot2;
        if(r2 < r_s*r_s){
            outColor = vec4(0.0, 0.0, 0.0, 1.0);
			
		//	float t = (rrdot-sqrt(rrdot*rrdot-r2*lightspeed2+lightspeed2*r_s*r_s))/lightspeed2;
		//	vec3 intersect = cam_position - t*normallight;
		//	float inang = acos(dot(normallight,intersect)/(sqrt(lightspeed2)*r_s));
		//	float outang;
		//	//Refraction
		//	float ref = 0.95;
		//	outang = asin(sin(inang)/ref)-inang;
        //  outColor = texture(u_texture, rotateray(cos(outang),sin(outang)));
        }else if(rrdot > 0){
            outColor = texture(u_texture, v_lightray);
        }
        else if(r2*r2< r_s*r_s*(dr2_dtheta2+r2)){
            outColor = vec4(0.0, 0.0, 0.0, 1.0);
			
			float t = (rrdot+sqrt(rrdot*rrdot-r2*lightspeed2+lightspeed2*r_s*r_s))/lightspeed2;
			vec3 intersect = cam_position - t*normallight;
			//Refraction
		//	float inang = acos(-dot(normallight,intersect)/(sqrt(lightspeed2)*r_s));
		//	float outang;
		//	float ref = 0.95;
		//	outang = 2*(inang-asin(ref*sin(inang)));
        //  outColor = texture(u_texture, rotateray(cos(outang),sin(outang)));
			//Reflection
			float inang = -dot(normallight,intersect)/(sqrt(lightspeed2)*r_s);
			float fresnel = 0.2 + 0.8*pow(1-inang,5);
			outColor = texture(u_texture, normallight-intersect*2*dot(normallight,intersect)/(r_s*r_s))*fresnel;
        }
        else{
            outColor = texture(u_texture, v_lightray);
        }
    }
	else if(gravity_mode == 1) //Newtonian Gravity
	{
        
        float D2 = r4thetadot2*(r2*lightspeed2 - r_s*r);
        if(0 > D2){
            outColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
        else{
			float A = r4thetadot2 - 0.5*r_s*r;
			float B = -rrdot*r2thetadot;
			float C = -0.5*r_s*r;
            float D = sqrt(D2);
            //Equation has two solutions, must use the smaller one
            if((B*C+A*D)*(B*C-A*D)<=0){
                outColor = texture(u_texture, finalray(A*C-sign(A)*B*D, B*C+sign(A)*A*D));
            }
            else if(B*C>0) //0<theta<pi
            {
                outColor = texture(u_texture, finalray(A*C+sign(B)*B*D, B*C-sign(B)*A*D));
            }
            else //pi<theta<2pi
            {
                outColor = texture(u_texture, finalray(A*C-sign(B)*B*D, B*C+sign(B)*A*D));
            }
        }
    }
	else //General Relativity
	{
		float D = 27*r_s*r_s*(r*rrdot*rrdot + (r-r_s)*r4thetadot2)/(4*r2*r*r4thetadot2);
		float angle;
		if(2*r > 3*r_s){
			if(r_s == 0){
				outColor = texture(u_texture, normallight);
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
    
}