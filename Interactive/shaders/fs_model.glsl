#version 410 core

out vec4 color;

in VS_OUT
{
    vec4 vertex;
    vec4 normals;
    vec2 tc;
} fs_in;

uniform sampler2D tex;
uniform mat4 model_matrix;

uniform vec4 lightPosition;
uniform vec4 viewPosition;

uniform vec4 ia;        // Ambient colour
uniform float ka;        // Ambient constant
uniform vec4 id;        // Diffuse colour
uniform float kd;        // Diffuse constant
uniform vec4 is;        // specular colour
uniform float ks;        // specular constant
uniform float shininess;// shininess constant

void main(void){


  

  // Diffuse
    vec4 lightDir = normalize(lightPosition - fs_in.vertex);
    float diff = max(dot(normalize(fs_in.normals), lightDir), 0.0);
    vec4 diffuse = diff * id;
  
    // Specular
    vec4 viewDir = normalize(viewPosition - fs_in.vertex);
    vec4 reflectDir = reflect(-lightDir, normalize(fs_in.normals));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);

    // Light
    // color = vec4(ka) * texture(tex, fs_in.tc);
    color = vec4(ka * ia.rgb + kd * id.rgb * diffuse.rgb + ks * is.rgb * spec, 1.0) * texture(tex, fs_in.tc);
}