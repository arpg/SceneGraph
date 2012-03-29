// see http://olivers.posterous.com/linear-depth-in-glsl-for-real

varying float depth;
void main(void)
{
    float A = gl_ProjectionMatrix[2].z;
    float B = gl_ProjectionMatrix[3].z;
    float zNear = - B / (1.0 - A);
    float zFar  =   B / (1.0 + A);
    float d = 0.5*(-A*depth + B) / depth + 0.5;
//    gl_FragDepth  = d;
    gl_FragColor = vec4( vec3(d), 1.0);
}

