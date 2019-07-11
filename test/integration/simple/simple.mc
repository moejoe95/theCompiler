float transform_x(float x)
{
	float fx;
	fx = x;
	return (-2.0) + ((1.0) - (-2.0)) * (fx / 80.0);
}

int main(){

	float x;

    x = transform_x(160.0);

    print_float(x);
 
    return 0;
}