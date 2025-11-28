#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

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
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDirectionFromFrag = normalize(lightPos - FragPos);

    // Robust Two-sided lighting: 
    // Always flip normal to face the viewer (camera).
    // This ensures that the face we see is the one used for lighting calculations.
    if (dot(norm, viewDir) < 0.0) {
        norm = -norm;
    }

    float diff = max(dot(norm, lightDirectionFromFrag), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    // vec3 viewDir is already calculated above
    vec3 reflectDir = reflect(-lightDirectionFromFrag, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    
    // Spotlight (Flashlight) intensity
    float intensity = 1.0;
    if (isFlashlight) {
        // lightDir uniform is the flashlight's direction (e.g., camera.Front)
        // lightDirectionFromFrag is the vector from the fragment to the light source
        float theta = dot(lightDirectionFromFrag, normalize(-lightDir)); 
        float epsilon = cutOff - outerCutOff;
        intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
        
        // Apply intensity to diffuse and specular only
        diffuse *= intensity;
        specular *= intensity;
    }
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
