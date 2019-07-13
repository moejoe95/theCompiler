int main(){
    int count;
    count = 0;

    while(count < 10) {
        print_int(count);
        print_nl();
        count = count + 1;
    }

    print_nl();

    while(count > 0) {
        print_int(count);
        print_nl();
        count = count - 1;
    }

    print_nl();

    int a;
    int b;
    a = 10;
    b = 0;

    while(a >= 0 && b < 10) {
        print_int(a);
        print(" ");
        print_int(b);
        print_nl();
        a = a - 2;
        b = b + 1;
    }

    print_nl();

    int c;
    c = 0;

    while((c + 1) < 5) {
        print_int(c);
        print_nl();
        c = c + 1;
    }

    print_nl();

    a = 0;
    
    while(a < 3) {
        b = 0;

        print_int(a);
        print_nl();

        while (b < 3) {
            print_int(b);
            b = b + 1;
        }

        print_nl();        
        a = a + 1;
    }

    return 0;
}