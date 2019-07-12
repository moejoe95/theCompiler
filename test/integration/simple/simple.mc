float transform_x(float x)
{
	float fx;
	fx = x;
	return (-2.0) + ((1.0) - (-2.0)) * (fx / 80.0);
}

float transform_y(float y)
{
	float fy;
	fy = y;
	return ((1.0) - ((1.0) - (-1.0)) * (fy / 50.0)) * 2.0;
}


int main(){
	float x;

	x = transform_x(100.0);
	print_float(x);

	print_nl();

	x = transform_y(100.0);
	print_float(x);

    return 0;
}