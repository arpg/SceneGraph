varying float hank;

void main()
{
	//if (hank <= 300.0) {
		gl_FragColor = vec4(1.0 - hank, 1.0 - hank, 1.0 - hank, 1.0);
	//} else if (hank < 1000.0) {
	//	gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	//} else {
	//        gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
	//}
}