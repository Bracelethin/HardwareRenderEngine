#version 330 core
out vec4 FragColor;
struct Light {
    vec3 position;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform vec3 viewPos;
uniform Light light;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

vec3 calculatePointLight(){
    vec3 Normal=Normal+texture(texture_normal1, TexCoords).rgb;
    vec3 ambient = light.ambient*texture(texture_diffuse1, TexCoords).rgb;
    // diffuse 
    vec3 lightDir=normalize(light.position-FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),32);
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;  
    
    // attenuation
//    float distance    = length(light.position - FragPos);
//    float attenuation = 1.0 / distance;    

    // ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
    //diffuse   *= attenuation;
    //specular *= attenuation;   
        
    vec3 result = ambient+diffuse+specular;
    return result;
} 

void main()
{
  // define an output color value
  vec3 output = vec3(0.0);
  // do the same for all point lights
    output += calculatePointLight();
  
  FragColor = vec4(output, 1.0);
    // FragColor=vec4(0.5,0.5,0.5,1);
} 