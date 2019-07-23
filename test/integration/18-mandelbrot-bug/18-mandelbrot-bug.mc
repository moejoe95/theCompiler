float transform_x(float x)
{
	return ((1.0) - (-2.0)) * (x / 80.0);
}

float transform_y(float y)
{
	float fy;
	fy = y;
	return ((1.0) - ((1.0) - (-1.0)) * (fy / 50.0)) * 2.0;
}

int main()
{
	float fx;
	float fy;
	float x;
	float y;

	y = 0.0;
	fy = transform_y(y);
	print_float(fy);
	print_nl();

	y = 1.0;
	fy = transform_y(y);
	print_float(fy);
	print_nl();

	return 0;
}
