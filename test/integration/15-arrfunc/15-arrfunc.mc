
void func(int i) {
    print_int(i);
    print_nl();
}

int func2(int[3] arr) {
    return arr[1];
}

void func4(int[3] arr) {
    arr[0] = 3;
}

int main(){

    int[3] arr;
    arr[0] = 0;
    arr[1] = 1;
    arr[2] = 2;

    func(arr[0]);

    int i;
    i = func2(arr);
    print_int(i);
    print_nl();

    func(arr[1] + 1);

    func4(arr);
    print_int(arr[0]);
    print_nl();

    return 0;
}