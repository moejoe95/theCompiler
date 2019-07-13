void func(float x) {
    print_float(x);
    print_nl();
}

void func2(float x, float y) {
    float z;
    z = 1.0;

    print_float(z);
    print_nl();
    print_float(x);
    print_nl();
    print_float(y);
    print_nl();
}

float func3() {
    return 4.0;
}

float func4(float z) {
    return z;
}

int main(){

    func(0.0);

    float a;
    a = 1.0;
    func(a);

    func2(2.0, 3.0);
    
    float b;
    b = func3();
    print_float(b);
    print_nl();

    float c;
    c = func4(5.0);
    print_float(c);
    print_nl();

    return 0;
}