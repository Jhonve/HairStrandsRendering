#version 330

uniform vec3 LightColor1;
uniform vec3 LightColor2;
uniform vec3 LightColor3;
uniform vec3 LightColor4;

uniform vec3 LightDir1;
uniform vec3 LightDir2;
uniform vec3 LightDir3;
uniform vec3 LightDir4;

uniform sampler2D DepthMap;
uniform sampler2D OpacityMap;

uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform vec3 Color;

uniform float HeadShadow;
uniform float HairShadow;

in vec4 LightViewPosition1;
in vec4 LightViewPosition2;
in vec4 LightViewPosition3;
in vec4 LightViewPosition4;

in float LightViewDepth1;
in float LightViewDepth2;
in float LightViewDepth3;
in float LightViewDepth4;

in vec3 Normal;
in vec3 View;

out vec4 OutColor;

void GetShading(out vec3 val1, out vec3 val2, out vec3 val3, out vec3 val4)
{
	float Shininess = 2.;
	vec3 Kcd = Kd * Color;
	vec3 Kcs = Ks * vec3(1., 1., 1.);
	vec3 n = normalize(Normal);
	vec3 e = normalize(View);

	val1 = max(dot(LightDir1, n), 0.) * Kcd + pow(max(dot(reflect(-LightDir1, n), e), 0.), Shininess) * Kcs + vec3(Ka, Ka, Ka);
	val2 = max(dot(LightDir2, n), 0.) * Kcd + pow(max(dot(reflect(-LightDir2, n), e), 0.), Shininess) * Kcs + vec3(Ka, Ka, Ka);
	val3 = max(dot(LightDir3, n), 0.) * Kcd + pow(max(dot(reflect(-LightDir3, n), e), 0.), Shininess) * Kcs + vec3(Ka, Ka, Ka);
	val4 = max(dot(LightDir4, n), 0.) * Kcd + pow(max(dot(reflect(-LightDir4, n), e), 0.), Shininess) * Kcs + vec3(Ka, Ka, Ka);
}

void GetOpacityFactor(out float val1, out float val2, out float val3, out float val4)
{
	float tolerance = 0.02;

	// 1

	vec4 lpos1 = LightViewPosition1 / LightViewPosition1.w;
	vec2 texPos1 = (lpos1.xy * 0.5 + vec2(0.5, 0.5)) * 0.5;
	val1 = 0.;
	for (int i = 0; i < 6; ++i){
		for (int j = 0; j < 6; ++j){
			vec2 texPos = texPos1 + vec2((float(i) - 2.5) / 512., (float(j) - 2.5) / 512.);
			vec4 opacity1 = texture(OpacityMap, texPos);
			float ratio1 = max(LightViewDepth1 - opacity1.w, 0.) * 0.2;
			int depthID1 = min(int(ratio1), 2);
			ratio1 = min(ratio1 - float(depthID1), 1.);
			vec2 opacityRange1;
			if (depthID1 == 0)
				opacityRange1 = vec2(0., opacity1.x);
			else if (depthID1 == 1)
				opacityRange1 = opacity1.xy;
			else
				opacityRange1 = opacity1.yz;
			val1 += LightViewDepth1 > texture(DepthMap, texPos).y + tolerance ? (1. - HairShadow) * (1. - HeadShadow) : pow(1. - HairShadow, mix(opacityRange1.x, opacityRange1.y, ratio1));
		}
	}
	val1 /= 36.;

	// 2

	vec4 lpos2 = LightViewPosition2 / LightViewPosition2.w;
	vec2 texPos2 = (lpos2.xy * 0.5 + vec2(0.5, 0.5)) * 0.5 + vec2(0.5, 0.);
	val2 = 0.;
	for (int i = 0; i < 6; ++i){
		for (int j = 0; j < 6; ++j){
			vec2 texPos = texPos2 + vec2((float(i) - 2.5) / 512., (float(j) - 2.5) / 512.);
			vec4 opacity2 = texture(OpacityMap, texPos);
			float ratio2 = max(LightViewDepth2 - opacity2.w, 0.) * 0.2;
			int depthID2 = min(int(ratio2), 2);
			ratio2 = min(ratio2 - float(depthID2), 1.);
			vec2 opacityRange2;
			if (depthID2 == 0)
				opacityRange2 = vec2(0., opacity2.x);
			else if (depthID2 == 1)
				opacityRange2 = opacity2.xy;
			else
				opacityRange2 = opacity2.yz;
			val2 += LightViewDepth2 > texture(DepthMap, texPos).y + tolerance ? (1. - HairShadow) * (1. - HeadShadow) : pow(1. - HairShadow, mix(opacityRange2.x, opacityRange2.y, ratio2));
		}
	}
	val2 /= 36.;

	// 3

	vec4 lpos3 = LightViewPosition3 / LightViewPosition3.w;
	vec2 texPos3 = (lpos3.xy * 0.5 + vec2(0.5, 0.5)) * 0.5 + vec2(0., 0.5);
	val3 = 0.;
	for (int i = 0; i < 6; ++i){
		for (int j = 0; j < 6; ++j){
			vec2 texPos = texPos3 + vec2((float(i) - 2.5) / 512., (float(j) - 2.5) / 512.);
			vec4 opacity3 = texture(OpacityMap, texPos);
			float ratio3 = max(LightViewDepth3 - opacity3.w, 0.) * 0.2;
			int depthID3 = min(int(ratio3), 2);
			ratio3 = min(ratio3 - float(depthID3), 1.);
			vec2 opacityRange3;
			if (depthID3 == 0)
				opacityRange3 = vec2(0., opacity3.x);
			else if (depthID3 == 1)
				opacityRange3 = opacity3.xy;
			else
				opacityRange3 = opacity3.yz;
			val3 += LightViewDepth3 > texture(DepthMap, texPos).y + tolerance ? (1. - HairShadow) * (1. - HeadShadow) : pow(1. - HairShadow, mix(opacityRange3.x, opacityRange3.y, ratio3));
		}
	}
	val3 /= 36.;

	// 4

	vec4 lpos4 = LightViewPosition4 / LightViewPosition4.w;
	vec2 texPos4 = (lpos4.xy * 0.5 + vec2(0.5, 0.5)) * 0.5 + vec2(0.5, 0.5);
	val4 = 0.;
	for (int i = 0; i < 6; ++i){
		for (int j = 0; j < 6; ++j){
			vec2 texPos = texPos4 + vec2((float(i) - 2.5) / 512., (float(j) - 2.5) / 512.);
			vec4 opacity4 = texture(OpacityMap, texPos);
			float ratio4 = max(LightViewDepth4 - opacity4.w, 0.) * 0.2;
			int depthID4 = min(int(ratio4), 2);
			ratio4 = min(ratio4 - float(depthID4), 1.);
			vec2 opacityRange4;
			if (depthID4 == 0)
				opacityRange4 = vec2(0., opacity4.x);
			else if (depthID4 == 1)
				opacityRange4 = opacity4.xy;
			else
				opacityRange4 = opacity4.yz;
			val4 += LightViewDepth4 > texture(DepthMap, texPos).y + tolerance ? (1. - HairShadow) * (1. - HeadShadow) : pow(1. - HairShadow, mix(opacityRange4.x, opacityRange4.y, ratio4));
		}
	}
	val4 /= 36.;
}

void main()
{
	vec3 shading1, shading2, shading3, shading4;
	GetShading(shading1, shading2, shading3, shading4);

	float opacityFactor1, opacityFactor2, opacityFactor3, opacityFactor4;
	GetOpacityFactor(opacityFactor1, opacityFactor2, opacityFactor3, opacityFactor4);

    OutColor = vec4((
		LightColor1 * shading1 * opacityFactor1 +
		LightColor2 * shading2 * opacityFactor2 +
		LightColor3 * shading3 * opacityFactor3 +
		LightColor4 * shading4 * opacityFactor4 ), 1.);
}
