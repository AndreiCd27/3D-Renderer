#version 330 core

out vec4 FragColor;

in vec3 color;
in vec3 normalVector;
in vec4 vertexShadowPosition;

uniform sampler2D shadowMap;
uniform vec3 lightDirection;

void main()
{
	vec3 projCoords = vertexShadowPosition.xyz / vertexShadowPosition.w;

	if(projCoords.z > 1.0) {
        FragColor = vec4(color, 1.0);
        return;
    }

	float shadowFactor = texture(shadowMap, projCoords.xy).r;

	vec3 normal = normalize(normalVector);
	vec3 ToLightSource = normalize(lightDirection);

	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float DOT = dot(normal,ToLightSource) + 0.9;
	vec3 diffuse = lightColor * max(DOT / 2.0, 0.0);

	vec3 ambient = vec3(0.05, 0.05, 0.05);

	float B = max( 0.0001, 0.001 * ( 1.0 - DOT ) ); 

	float visibility = 0.0;

	vec2 dtex = 1.0 / textureSize(shadowMap, 0);

	for (float xi = -1.0; xi <= 1.0; xi++) {
		for (float yi = -1.0; yi <= 1.0; yi++) {
			vec2 dxy = vec2(xi, yi) * dtex;
			shadowFactor = texture(shadowMap, projCoords.xy + dxy).r;
			visibility += ( (projCoords.z - B > shadowFactor) ? 0.5 : 1.0 );
		}
	}

	visibility /= 9.0;

	

    FragColor = vec4( visibility * diffuse * color + ambient, 1.0);

	//FragColor = vec4(vec3(shadowFactor), 1.0); 
};