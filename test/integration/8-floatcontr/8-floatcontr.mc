
int main(){

    float f;
    f = 1.0;

    if(f == 1.0) {
        print("1 true");
    } else {
        print("1 false");
    }

    print_nl();

    if(f < 2.0) {
        print("2 true");
    } else {
        print("2 false");
    }

    print_nl();
    print_nl();

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