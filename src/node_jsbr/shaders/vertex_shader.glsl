#version 300 es

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;

layout(std140) uniform mvp_matrix
{
    mat4 model;
    mat3 normmat;
    mat4 projection;
};

layout(std140) uniform lighting
{
    float ambient;
    float diffuse;
    float specular;
    float shininess;
    vec4 position;
}
u_light;

uniform bool enable_lighting;

out vec4 vColor;

// workarea for lighting calc
vec4 Ambient;
vec4 Diffuse;
vec4 Specular;

vec3 fnormal(in mat4 model_mat, in vec4 in_normal)
{
    // Compute the normal
    vec3 tmp_norm = transpose(inverse(mat3(model_mat))) * in_normal.xyz;
    // vec3 tmp_norm = in_normal.xyz;
    tmp_norm = normalize(tmp_norm);
    return tmp_norm;
}


void DirectionalLight(in vec3 normal)
{
    float nDotVP;  // normal . light direction
    float nDotHV;  // normal . light half vector
    float pf;      // power factor

    vec3 lightSource_position = u_light.position.xyz;
    vec3 lightSource_halfVector = normalize(lightSource_position + vec3(0, 0, 1));
    nDotVP = max(0.0, dot(normal, normalize(vec3(lightSource_position))));
    nDotHV = max(0.0, dot(normal, vec3(lightSource_halfVector)));

    float shininess = u_light.shininess;
    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow(nDotHV, shininess);

    Ambient = vec4(u_light.ambient, u_light.ambient, u_light.ambient, 0.0f);
    Diffuse = vec4(u_light.diffuse, u_light.diffuse, u_light.diffuse, 0.0f) * nDotVP;
    Specular = vec4(u_light.specular, u_light.specular, u_light.specular, 0.0f) * pf;
}

vec4 flight(in vec3 normal, in vec4 ecPosition, in vec4 in_color)
{
    vec4 color;

    // ConstDirectionalLight(normal);
    DirectionalLight(normal);

    vec4 lightModel_ambient = vec4(0.2, 0.2, 0.2, 1.0);
    vec4 frontMaterial_specular = vec4(0.4, 0.4, 0.4, 1.0);

    color = lightModel_ambient * in_color;
    color += Ambient * in_color;
    color += Diffuse * in_color;
    color += Specular * frontMaterial_specular;
    color = clamp(color, 0.0, 1.0);
    return color;
}

void main()
{
    vec4 ecPos = model * vec4(vertexPosition.xyz, 1.0);
    gl_Position = projection * ecPos;

    if (enable_lighting) {
        vec3 vnormal = fnormal(model, normal);
        // vec3 vnormal = fnormal(model, vec4(0, 0, 1, 1));
        // vec3 vnormal = vec3(0, 0, 1);

        vColor = flight(vnormal, ecPos, color);
        // vColor = flight(vnormal, ecPos, color);
        // vColor = vec4(1, 1, 1, 1);
        // vColor = normal;
    } else {
        vColor = color;
    }

    // TODO: Fog
}
