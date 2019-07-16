int main(){
    
    float y;
    y = 0.0;

    while (y < 5.0) {
		print_float(y);
		print_nl();
		y = y + 1.0;
	}

    print_nl();

    float x;
    float z;
    z = 0.0;

    while (z < 3.0) {
        print("z ");
		print_float(z);
        print_nl();

		x = 0.0;

		while (x < 3.0) {
            print("x ");
            print_float(x);
            print_nl();
			x = x + 1.0;
		}
        
        print_nl();

        
		z = z + 1.0;
	}

    return 0;
}