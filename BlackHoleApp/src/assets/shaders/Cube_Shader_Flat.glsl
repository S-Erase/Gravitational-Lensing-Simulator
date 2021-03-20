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

float r2 = dot(cam_position,cam_position);
vec3 angmom = cross(cam_position, v_lightray);

float rrdot = dot(cam_position, v_lightray);
float lightspeed2 = dot(v_lightray, v_lightray);

float r4thetadot2 = dot(angmom,angmom);

float r = length(cam_position);

void main()
{
		float dr2_dtheta2 = r2*rrdot*rrdot/r4thetadot2;
        if(r2 < r_s*r_s){
            outColor = vec4(0.0, 0.0, 0.0, 1.0);
			
		//	float t = (rrdot-sqrt(rrdot*rrdot-r2*lightspeed2+lightspeed2*r_s*r_s))/lightspeed2;
		//	vec3 intersect = cam_position - t*v_lightray;
		//	float inang = acos(dot(v_lightray,intersect)/(sqrt(lightspeed2)*r_s));
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
			vec3 intersect = cam_position - t*v_lightray;
			//Refraction
		//	float inang = acos(-dot(v_lightray,intersect)/(sqrt(lightspeed2)*r_s));
		//	float outang;
		//	float ref = 0.95;
		//	outang = 2*(inang-asin(ref*sin(inang)));
        //  outColor = texture(u_texture, rotateray(cos(outang),sin(outang)));
			//Reflection
			float inang = -dot(v_lightray,intersect)/(sqrt(lightspeed2)*r_s);
			float fresnel = 0.2 + 0.8*pow(1-inang,5);
			outColor = texture(u_texture, v_lightray-intersect*2*dot(v_lightray,intersect)/(r_s*r_s))*fresnel;
        }
        else{
            outColor = texture(u_texture, v_lightray);
        }
}