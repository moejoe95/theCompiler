int main(){
    int i;
    int j;

    i = 1;
    j = 2;

    int k;
    k = i + j + i;

    print_int(k);
	print_nl();

    int b;
	int a;

	b = 2 + 3;
	b = -b;

	print_int(b);
	print_nl();
	
	a = b;
	b = b * b;
	if (a == (b/a)) {
		print_int(b);
		print_nl();
	}

    return 0;
}