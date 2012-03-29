uniform mat4 worldViewProj;
uniform vec4 texelOffsets;
uniform vec4 depthRange;

varying float hank;

void main()
{
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;  
//	gl_Position = ftransform();
//  	gl_Position.xy += texelOffsets.zw * gl_Position.w;
//	hank = (gl_Position.z - depthRange.x) * depthRange.w;
	hank = (gl_Position.z) / 1000;
//	hank = gl_Position.z
}