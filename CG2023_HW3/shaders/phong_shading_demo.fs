#version 330 core

// Data from vertex shader.
in vec3 iPosWorld;
in vec3 iNormalWorld;
in vec2 iTexCoord;
// Material properties.
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

uniform sampler2D mapKd;
uniform int mapExist;
// Light data.
uniform vec3 ambientLight;
uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;
uniform vec3 pointLightPos;
uniform vec3 pointLightIntensity;

uniform vec3 cameraPos;

uniform vec3 spotLightPos;
uniform vec3 spotLightIntensity;
uniform vec3 spotLightDir;
uniform float TotalWidth;
uniform float FalloffStart;

out vec4 FragColor;


vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDirection)
{
    return Kd * I * max(0, dot(N, lightDirection));
}

vec3 Specular(vec3 Ks, float Ns, vec3 I, vec3 N, vec3 lightDirection, vec3 viewDirection)
{
    // Phong:
    // vec3 vR = normalize(2 * dot(N, lightDirection) * N - lightDirection);
    // return Ks * I * pow(max(0, dot(viewDirection, vR)), Ns);
    // --------------------------------------------------------
    // Blinn-Phong:
    vec3 vH = normalize(lightDirection + viewDirection);
    return Ks * I * pow(max(0, dot(N, vH)), Ns);
    // --------------------------------------------------------
    // vec3 bisector;
    // bisector.x = (lightDirection.x + viewDirection.x) * 0.5f;
    // bisector.y = (lightDirection.y + viewDirection.y) * 0.5f;
    // bisector.z = (lightDirection.z + viewDirection.z) * 0.5f;
    // float length = sqrt(lightDirection.x * lightDirection.x + lightDirection.y * lightDirection.y + lightDirection.z * lightDirection.z);

    // bisector.x /= length;
    // bisector.y /= length;
    // bisector.z /= length;

    // float specTerm = max(0, dot(N, bisector));
    // return Ks * I * pow(specTerm, Ns);
    // --------------------------------------------------------
}

void main()
{
    vec3 tex;
    if(mapExist == 0){
        tex = Kd;
    }
    else{
        tex = texture2D(mapKd, iTexCoord).rgb;
    }
    
    vec3 vN = normalize(iNormalWorld);
    vec3 vE = normalize(cameraPos - iPosWorld);
    // -------------------------------------------------------------
    // Ambient light.
    vec3 ambient = Ka * ambientLight;
    // -------------------------------------------------------------
    // Directional light.
    vec3 wsLightDirection = normalize(-dirLightDir);
    // Diffuse.
    vec3 diffuse = Diffuse(tex, dirLightRadiance, vN, wsLightDirection);
    // Specular.
    vec3 specular = Specular(Ks, Ns, dirLightRadiance, vN, wsLightDirection, vE);
    vec3 dirLight = diffuse + specular;
    // -------------------------------------------------------------
    // Point light.
    wsLightDirection = normalize(pointLightPos - iPosWorld);
    float distSurfaceToLight = distance(pointLightPos, iPosWorld);
    float attenuation = 1.0f / (distSurfaceToLight * distSurfaceToLight);
    vec3 radiance = pointLightIntensity * attenuation;
    // Diffuse.
    diffuse = Diffuse(tex, radiance, vN, wsLightDirection);
    // Specular.
    specular = Specular(Ks, Ns, radiance, vN, wsLightDirection, vE);
    vec3 pointLight = diffuse + specular;
    // -------------------------------------------------------------
    // Spot Light.
    vec3 wsSpotLightPositionDir = normalize(spotLightPos - iPosWorld);
    vec3 wsSpotLightDir = normalize(spotLightDir);
    float SpotToObjDistance = distance(spotLightPos, iPosWorld);
    float A = degrees(acos(dot(-wsSpotLightDir, wsSpotLightPositionDir)));

    float attenuationSpot = clamp((A - TotalWidth) / (FalloffStart - TotalWidth), 0, 1);  // 把算好的值轉換到0~1之間
    attenuationSpot /= (SpotToObjDistance * SpotToObjDistance);

    vec3 spotLightRadiance = spotLightIntensity * attenuationSpot;  // Spot light衰減值

    // Diffuse
    diffuse = Diffuse(tex, spotLightRadiance, vN, wsSpotLightPositionDir);
    // Specular
    specular = Specular(Ks, Ns, spotLightRadiance, vN, wsSpotLightPositionDir, vE);
    vec3 spotLight = diffuse + specular;
    // -------------------------------------------------------------
    vec3 iLightingColor = ambient + dirLight + pointLight + spotLight;
    // vec3 iLightingColor = dirLight;
    // vec3 iLightingColor = pointLight;
    // vec3 iLightingColor = spotLight;
    FragColor = vec4(iLightingColor, 1.0);
}
