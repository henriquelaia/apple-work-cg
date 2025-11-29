#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

// Uniforms da Lanterna
uniform vec3 lightDir;
uniform float cutOff;
uniform float outerCutOff;
uniform bool isFlashlight;

void main()
{
    // ambiente
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // difusa
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDirectionFromFrag = normalize(lightPos - FragPos);

    // Iluminação robusta de dois lados:
    // Inverter sempre a normal para encarar o observador (câmara).
    // Isto garante que a face que vemos é a usada para os cálculos de iluminação.
    if (dot(norm, viewDir) < 0.0) {
        norm = -norm;
    }

    float diff = max(dot(norm, lightDirectionFromFrag), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // especular
    float specularStrength = 0.5;
    // vec3 viewDir já calculado acima
    vec3 reflectDir = reflect(-lightDirectionFromFrag, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    
    // Intensidade do Holofote (Lanterna)
    float intensity = 1.0;
    if (isFlashlight) {
        // lightDir uniform é a direção da lanterna (ex: camera.Front)
        // lightDirectionFromFrag é o vetor do fragmento para a fonte de luz
        float theta = dot(lightDirectionFromFrag, normalize(-lightDir)); 
        float epsilon = cutOff - outerCutOff;
        intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
        
        // Aplicar intensidade apenas à difusa e especular
        diffuse *= intensity;
        specular *= intensity;
    }
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
