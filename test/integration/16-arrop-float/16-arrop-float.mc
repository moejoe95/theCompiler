
int main(){

    float[3] arr;

    arr[0] = 0.0;
    arr[1] = 1.0;
    arr[2] = 2.0;

    print_float(arr[0]);
    print_nl();
    print_float(arr[1]);
    print_nl();
    print_float(arr[2]);
    print_nl();

    arr[1] = 9.0;
    print_float(arr[1]);
    print_nl();

    arr[2] = 2.0;
    print_float(arr[1] * arr[2]);
    print_nl();

    float x;
    x = arr[1];
    print_float(x);
    print_nl();

    float y;
    y = arr[0] + arr[2];
    print_float(y);
    print_nl();

    return 0;
}