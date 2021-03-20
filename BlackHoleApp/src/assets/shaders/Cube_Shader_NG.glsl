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

vec3 normallight = normalize(v_lightray);

vec3 angmom = cross(cam_position, normallight);

float r2 = dot(cam_position,cam_position);
float rrdot = dot(cam_position, normallight);
float r2thetadot = length(angmom);

float r = length(cam_position);
float r4thetadot2 = dot(angmom,angmom);
	
vec3 finalray(float u, float v)
{
    return cam_position*r2thetadot*u + (normallight*r2 - cam_position*rrdot)*v;
}

void main()
{
        float D2 = r4thetadot2*(r2 - r_s*r);
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