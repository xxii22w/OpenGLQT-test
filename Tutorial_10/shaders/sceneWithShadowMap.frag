#version 330 core
out vec4 FragColor;

in VS_OUT {
  vec3 FragPos;
  vec4 FragPosLightSpace;
  vec3 FragColor;
} fs_in;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

// PCF
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
  float bias = 0.0007;
  float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

  return shadow;
}

void main()
{
  vec3 color = fs_in.FragColor;
  // ambient
  vec3 ambient = 0.35 * color;
  // diffuse
  // calculate shadow
  float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
  vec3 lighting = (ambient + (1.0 - shadow) ) * color;
  FragColor = vec4(lighting, 1.0);
}