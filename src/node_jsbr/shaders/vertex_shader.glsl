#version 300 es
// -*-Mode: C++;-*-

layout (location=0) in vec4 vertexPosition;
layout (location=1) in vec4 color;
layout (location=2) in vec4 normal;

layout (std140) uniform mvp_matrix {
    mat4 model;
    mat3 normmat;
    mat4 projection;
};

layout (std140) uniform lighting {
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    vec4 position;
} u_light;

uniform bool enable_lighting;

out vec4 vColor;


// workarea for lighting calc
vec4 Ambient;
vec4 Diffuse;
vec4 Specular;

vec3 fnormal(in mat4 model_mat)
{
    // Compute the normal
    vec3 norm = transpose(inverse(mat3(model_mat))) * normal.xyz;
    norm = normalize(norm);
    return norm;
}

void DirectionalLight(in int i, in vec3 normal)
{
  float nDotVP;         // normal . light direction
  float nDotHV;         // normal . light half vector
  float pf;             // power factor
  
  vec3 lightSource_position = u_light.position.xyz; //vec3(1.0f, 1.0f, 1.5f);
  vec3 lightSource_halfVector = normalize(lightSource_position + vec3(0, 0, 1));
  nDotVP = max(0.0, dot(normal,
                        normalize(vec3(lightSource_position))));
  nDotHV = max(0.0, dot(normal, vec3(lightSource_halfVector)));
  
  float shininess = u_light.shininess; //32.0f;
  if (nDotVP == 0.0)
    pf = 0.0;
  else
    pf = pow(nDotHV, shininess);
  
  Ambient  += vec4(u_light.ambient,u_light.ambient,u_light.ambient, 0.0f);
  Diffuse  += vec4(u_light.diffuse, u_light.diffuse, u_light.diffuse, 0.0f) * nDotVP;
  Specular += vec4(u_light.specular, u_light.specular, u_light.specular, 0.0f) * pf;
}

vec4 flight(in vec3 normal, in vec4 ecPosition, in vec4 in_color)
{
    vec4 color;
    vec3 ecPosition3;
    vec3 eye;
    
    ecPosition3 = (vec3 (ecPosition)) / ecPosition.w;
    eye = vec3 (0.0, 0.0, 1.0);
    
    // return in_color + vec4(normal.xyz, 0.0);
    // Clear the light intensity accumulators
    Ambient  = vec4(0.0);
    Diffuse  = vec4(0.0);
    Specular = vec4(0.0);
    
    //pointLight(0, normal, eye, ecPosition3);
    DirectionalLight(0, normal);
    
    vec4 lightModel_ambient = vec4(0.2, 0.2, 0.2, 1.0);
    vec4 frontMaterial_specular = vec4(0.4, 0.4, 0.4, 1.0);

    color = lightModel_ambient * in_color;
    color += Ambient  * in_color;
    color += Diffuse  * in_color;
    color += Specular * frontMaterial_specular;
    color = clamp( color, 0.0, 1.0 );
    return color;
}

void main() {
    vec4 ecPos = model * vec4(vertexPosition.xyz, 1.0);
    gl_Position = projection * ecPos;
    // gl_Position = vec4(vertexPosition.xyz, 1.0);

    // vec3 vnormal = fnormal(model);
    // vColor = flight(vnormal, ecPos, color);

    if (enable_lighting) {
        vec3 vnormal = fnormal(model);
        vColor = flight(vnormal, ecPos, color);
    }
    else {
        vColor = color;
    }

  // TODO: Fog
}
