#version 330 core

uniform sampler2D tex0;

uniform vec3 cameraPos;
uniform vec4 rgba = vec4(1.f);

out vec4 FragColor;

in vec3 normCoord;
in vec3 fragPos;
in vec2 texCoord;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float ambientStr;
	float specStr;
	float specPhong;

	vec3 lightColor;
	vec3 ambientColor;
};  

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float ambientStr;
    float specStr;
    float specPhong;

    vec3 lightColor;
    vec3 ambientColor;
};

uniform DirLight dirLight;
uniform PointLight pointLight;

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir); 
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir); 

float getFogFactor(float d)
{
    const float FogMax = 20.0;
    const float FogMin = 10.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}

void main(){
	//				     r     g      b      a
	// FragColor = vec4(1.0f, 0.72f, 0.77f, 1.0f);
	// FragColor = rgba;

	// light source info
	vec3 normal = normalize(normCoord);
	vec3 viewDir = normalize(cameraPos - fragPos);	

	// calculate texture, direction light, and point light vectors
	vec4 tex = texture(tex0, texCoord);
	vec4 dir = CalcDirLight(dirLight, normal, viewDir);
	vec4 point = CalcPointLight(pointLight, normal, fragPos, viewDir);

	vec4 light = vec4(0.f);
	FragColor = vec4(1.f); // initialize fragColor to 1.f in case nothing gets applied

	float d = distance(cameraPos, fragPos);
	float alpha = getFogFactor(d);
	float fog_start = 5.f;
	float fog_end = 50.f;

	// if (d > fog_end)
	// 	discard;

	//linear interpolation
	float fog_factor = (d - fog_start) / ( fog_end - fog_start);
	fog_factor = clamp(fog_factor, 0, 1);

	// if there is a texture, apply it
	if (all(greaterThan(tex, vec4(0.f))))
		FragColor *= tex;

	// // if there is a point light, apply it
	if (any(greaterThan(point.xyz, vec3(0.f))))
		light += point;
		
	// // if there is a direction light, apply it
	if (any(greaterThan(dir.xyz, vec3(0.f))))
		light += dir;

	if (any(greaterThan(light.xyz, vec3(0.f))))
		FragColor *= light;
	
	// if there's any changes to the colors, apply it
	if (any(lessThan(rgba.xyz, vec3(1.f))))
		FragColor *= rgba;

	FragColor = mix(vec4(0.02f, 0.02f, 0.05f, 1.f), FragColor, 1.f - fog_factor);
	// FragColor *= vec4(0.f, 1.f, 0.f, 1.f); // simple nightvision
}

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    // light source info
    vec3 lightDir = normalize(-light.direction);	

	// ambient
	vec3 ambient = light.ambientColor * light.ambientStr;

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.lightColor;
	
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.1), light.specPhong);
	vec3 specColor = spec * light.specStr * light.lightColor;

    return vec4(specColor + diffuse + ambient, 1.0);
} 

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // precalculated intensity scaling taken from online sources (OpenGL tutorials...)
	// float constant = 1.0;
	// float linear = 0.14;
	// float quadratic = 0.07;

    vec3 lightDir = normalize(light.position - fragPos);
	float dist = length(light.position - fragPos);

	// More realistic computation for attenuation
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	// Simple attenuation calculation
	// float attenuation = 1.0 / (dist * dist);

	// ambient
	vec3 ambient = light.ambientColor * light.ambientStr;

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.lightColor;
	
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), light.specPhong);
	vec3 specular = spec * light.specStr * light.lightColor;

	return vec4(attenuation * (specular + diffuse + ambient), 1.0);
}