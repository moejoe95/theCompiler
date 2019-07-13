void print1() {
    print("hello");
    print_nl();
}

void print2(string s) {
    print(s);
    print_nl();
}

void add(int i, int j) {
    int k;
    k = i + j;
    print_int(k);
    print_nl();
}

int add2(int i, int j) {
    return i + j;
}

string print3() {
    return "bye";
}

int main(){
    print1();
    print2("world");

    add(12, 30);

    int result;
    result = add2(59, 5);
    print_int(result);
    print_nl();

    print(print3());
    print_nl();

    return 0;
}