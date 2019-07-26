
void func(int i) {
    print_int(i);
    print_nl();
}

int func2(int[3] array) {
    return array[1];
}

void func4(int[3] array) {
    array[0] = 3;
}

void func5(float[3] f_arr){
    float tmp;
    tmp = f_arr[0];
    f_arr[0] = f_arr[1];
    f_arr[1] = tmp;
}

int main(){

    int[3] arr;
    arr[0] = 0;
    arr[1] = 1;
    arr[2] = 2;

    float[3] f_arr;
    f_arr[0] = 2.0;
    f_arr[1] = 3.0;

    func(arr[0]);

    int i;
    i = func2(arr);
    print_int(i);
    print_nl();

    func(arr[1] + 1);

    func4(arr);
    print_int(arr[0]);
    print_nl();

    func5(f_arr);
    print_float(f_arr[0]);
    print_nl();
    print_float(f_arr[1]);

    return 0;
}