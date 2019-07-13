int main(){
    int i;
    int j;

    i = 2;
    j = 10;

    if(true) {
        print("1 true");
    }
    print_nl();

    if(false) {
        print("2 false");
    } else {
        print("2 true");
    }
    print_nl();

    if(1 == 1 && true) {
        print("3 true");
    }
    print_nl();

    if(i * 5 == j) {
        print("4 true");
    }
    print_nl();

    if((2 * 1) * 5 == ((j * 10) / 10)) {
        print("5 true");
    } else  {
        print("5 false");
    }
    print_nl();

    bool b;
    bool c;

    b = true;
    c = false;

    if(b) {
        print("6 true");
    } else  {
        print("6 false");
    }
    print_nl();

    if(!c) {
        print("7 true");
    } else  {
        print("7 false");
    }
    print_nl();

    if(b && !c && true) {
        print("8 true");
    } else  {
        print("8 false");
    }
    print_nl();

    return 0;
}