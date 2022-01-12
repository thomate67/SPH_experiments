#version 460

layout(location = 0) out vec4 color;

void main ()
{
	//if(length(gl_PointCoord * 2.0f - 1.0f) <= 1.0f)
	//{
	//	color = vec4(0.0f, 0.0f, 0.95f, 1.0f);
	//}
	//else
	//{
	//	discard;
	//}
	color = vec4(0.0f, 0.0f, 0.95f, 1.0f);
}