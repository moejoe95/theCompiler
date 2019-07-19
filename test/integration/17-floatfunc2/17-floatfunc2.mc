float transform(float x) {
	return 1.0;
}

int main()
{
	float x;
	float fx;
	x = 0.0;

	while (x < 5.0) {
		fx = transform(x);
		x = x + 1.0;

		print_float(x);
		print_nl();
	}

	return 0;
}
