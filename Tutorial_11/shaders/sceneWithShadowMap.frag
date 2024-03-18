#version 330 core
out vec4 FinalColor;

in VS_OUT {
	vec3 FragPos;            // fragment 碎片在世界坐标中的位置
	vec4 FragPosLightSpace;  // fragment 片段在光空间坐标中的位置
	vec3 FragNormal;         // 顶点法线在片段中的位置
	vec3 FragColor;          // color of fragment
} fs_in;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
  // 执行透视分割
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;
  // 从光的角度获得最接近的深度值(使用[0,1]范围fragPosLight作为坐标)
  float closestDepth = texture(shadowMap, projCoords.xy).r;
  // 从光的角度获得当前碎片的深度
  float currentDepth = projCoords.z;
  // 检查当前碎片位置是否在阴影中
  //  float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
  float bias = 0.001;
  float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

  return shadow;
}

void main()
{
  vec3 color = fs_in.FragColor;
  vec3 normal = normalize(fs_in.FragNormal);
  vec3 lightColor = vec3(1.0);
  // ambient
  vec3 ambient = 0.15 * color;
  // diffuse
  vec3 lightDir = normalize(lightPos - fs_in.FragPos);
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = diff * lightColor;
  // specular
  vec3 viewDir = normalize(viewPos - fs_in.FragPos);
  float spec = 0.0;
  vec3 halfwayDir = normalize(lightDir + viewDir);
  spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
  vec3 specular = spec * lightColor;
  // calculate shadow: 1 - in light, 0 - dark
  float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
  // compose final light value - mind that this can lead to a brighter color than the original color
  vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
  FinalColor = vec4(lighting, 1.0);
}