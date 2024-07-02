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
	if(r2 < r_s*r_s) //Inside Event Horizon
	{
		outColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else if(r2 < 9/4*r_s*r_s) //Inside Photon Sphere
	{
		if(rrdot > 0) //Facing away from Black Hole
		{
			outColor = texture(u_texture, v_lightray);
		}
		else if(r2*r2< r_s*r_s*(dr2_dtheta2+r2)) //Looking at Event Horizon
		{
			outColor = vec4(0.0, 0.0, 0.0, 1.0);
			
			float t = (rrdot+sqrt(rrdot*rrdot-r2*lightspeed2+lightspeed2*r_s*r_s))/lightspeed2;
			vec3 intersect = cam_position - t*v_lightray;
			//Reflection
			float inang = -dot(v_lightray,intersect)/(sqrt(lightspeed2)*r_s);
			float fresnel = 0.2 + 0.8*pow(1-inang,5);
			outColor = texture(u_texture, v_lightray-intersect*2*dot(v_lightray,intersect)/(r_s*r_s))*fresnel;
		}
		else //Not Looking at Event Horizon
		{
			outColor = texture(u_texture, v_lightray);
		}
		outColor = 0.5 * outColor;
	}
	else if(rrdot > 0) //Facing away from Photon Sphere
	{
		outColor = texture(u_texture, v_lightray);
	}
	else if(r2*r2< 9/4*r_s*r_s*(dr2_dtheta2+r2)) //Looking at Photon Sphere
	{
		if(r2*r2< r_s*r_s*(dr2_dtheta2+r2)) //Looking at Event Horizon
		{
			float t = (rrdot+sqrt(rrdot*rrdot-r2*lightspeed2+lightspeed2*r_s*r_s))/lightspeed2;
			vec3 intersect = cam_position - t*v_lightray;
			//Reflection
			float inang = -dot(v_lightray,intersect)/(sqrt(lightspeed2)*r_s);
			float fresnel = 0.2 + 0.8*pow(1-inang,5);
			outColor = texture(u_texture, v_lightray-intersect*2*dot(v_lightray,intersect)/(r_s*r_s))*fresnel;
		}
		else
		{
		outColor = texture(u_texture, v_lightray);
		}
		
		float t = (rrdot+sqrt(rrdot*rrdot-r2*lightspeed2+lightspeed2*9/4*r_s*r_s))/lightspeed2;
		vec3 intersect = cam_position - t*v_lightray;
		//Reflection
		float inang = -dot(v_lightray,intersect)/(sqrt(lightspeed2)*3/2*r_s);
		float fresnel = 0.2 + 0.8*pow(1-inang,5);
		vec4 refColor = texture(u_texture, v_lightray-intersect*2*dot(v_lightray,intersect)/(9/4*r_s*r_s))*fresnel;
		outColor = outColor * 0.5 + refColor * 0.5;
	}
	else //Not Looking at Event Horizon
	{
		outColor = texture(u_texture, v_lightray);
	}
}