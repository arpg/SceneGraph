// see http://olivers.posterous.com/linear-depth-in-glsl-for-real
varying float depth;
void main(void)
{
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    depth = -(gl_ModelViewMatrix * gl_Vertex).z;
}

