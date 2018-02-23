#version 400
#define MAX_LIGHTS 10

in vec2 textureUV;
in vec3 out_normal;
uniform sampler2D mytexture;
uniform vec4 cubeColor;

in vec3 vertexPositionWorldSpace;

uniform float shinyness;
uniform float specularPower;

in vec3 eyeDirectionCameraSpace;
in vec3 normalCameraSpace;
in vec3 vertexPositionCameraSpace;

in vec3 LightPosCameraSpace[MAX_LIGHTS];
uniform int numLights;

struct Light
{
	vec3 position;
	vec3 colour;
	float power;
};

uniform Light lightArray[MAX_LIGHTS];

void main(void)
{
	vec4 ambient = cubeColor * vec4(0.2, 0.2, 0.2, 1.0);
	vec3 eyeVecNormal = normalize(eyeDirectionCameraSpace);

	vec3 lightColour; //accumulate lighting colour in this

	for(int i = 0; i < numLights; ++i)
	{
		float lightDistance = length(LightPosCameraSpace[i] - vertexPositionCameraSpace);
		vec3 lightDir = normalize(LightPosCameraSpace[i]); 
  
		//reflect our light vector around the fragment normal
		vec3 reflectVec = normalize(-reflect(lightDir, out_normal));

		vec4 diffuse = vec4(lightArray[i].colour, 1.0) * max(dot(out_normal,lightDir),0.0) * cubeColor * lightArray[i].power / (lightDistance * lightDistance);
 
		//use the reflected light vector and the eye vector to calculate specular highlight amount
		vec4 specular = vec4(lightArray[i].colour, 1.0) * pow(max(dot(reflectVec, eyeVecNormal), 0.0), specularPower);
		lightColour = lightColour + diffuse.rgb + specular.rgb * shinyness;
	}

	gl_FragColor = vec4(ambient.rgb + lightColour, cubeColor.a);
}
