varying vec3 normal;
void main()
{
//       gl_FragColor.rgb = normal;
       gl_FragColor.rgba = vec4( normal, 1.0 );
}
