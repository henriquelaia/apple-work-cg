#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D texture1;
uniform bool useTexture;

// Flashlight uniforms
uniform vec3 lightDir;
uniform float cutOff;
uniform float outerCutOff;
uniform bool isFlashlight;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDirection;
    
    if (isFlashlight) {
        lightDirection = normalize(lightPos - FragPos); // Direction from frag to light (camera)
    } else {
        lightDirection = normalize(lightPos - FragPos);
    }

    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    
    // Spotlight (Flashlight) intensity
    float intensity = 1.0;
    if (isFlashlight) {
        float theta = dot(lightDirection, normalize(-lightDir)); 
        float epsilon = cutOff - outerCutOff;
        intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
        
        // Apply intensity to diffuse and specular only
        diffuse *= intensity;
        specular *= intensity;
    }
        
    vec3 result;
    if(useTexture) {
        vec4 texColor = texture(texture1, TexCoords);
        result = (ambient + diffuse + specular) * texColor.rgb;
    } else {
        result = (ambient + diffuse + specular) * objectColor;
    }
    FragColor = vec4(result, 1.0);
}
