
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

    if(f <= 1.0) {
        print("3 true");
    } else {
        print("3 false");
    }

    print_nl();

    if(f != 1.0) {
        print("4 true");
    } else {
        print("4 false");
    }

    print_nl();

    if(f == 1.0) {
        print("5 true");
    } else {
        print("5 false");
    }

    print_nl();

    if (f < 0.0 || f < 0.0) {
		print("6 true");
	} else {
        print("6 false");
    }

    print_nl();

    return 0;
}