#version 410 core

out vec4 color;

in VS_OUT
{
    vec3 vertex;
    vec3 normals;
    vec2 tc;
} fs_in;

uniform sampler2D tex;
uniform mat4 model_matrix;

void main(void){
  // color = vec4(1.0,1.0,1.0,1.0);
  // color = vec4(fs_in.normals, 1.0);

  color = texture(tex, fs_in.tc);
}

// #version 410 core

// out vec4 color;

// in VS_OUT
// {
//     vec3 vertex;
//     vec3 normals;
//     vec2 tc;
//     vec4 fragPos;
// } fs_in;

// uniform sampler2D tex;
// uniform mat4 model_matrix;

// uniform vec3 materialColor;
// uniform vec3 ia;
// uniform float ka;
// uniform vec4 lightPosition;

// void main(void){
//   vec3 normalsT = mat3(transpose(inverse(model_matrix))) * vec3(fs_in.normals.xyz);
//   vec4 normal = normalize(vec4(normalsT, 1.0));
//   vec4 lightDir = normalize(lightPosition - fs_in.fragPos);
//   float diff = max(dot(normal, lightDir), 0.0);
//   vec4 diffuse = diff * vec4(1.0, 1.0, 1.0, 1.0);
//   color = vec4( ka * ia , 1.0) + diffuse;
// }
