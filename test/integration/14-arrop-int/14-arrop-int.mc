
int main(){

    int[3] arr;

    arr[0] = 0;
    arr[1] = 1;
    arr[2] = 2;

    print_int(arr[0]);
    print_nl();
    print_int(arr[1]);
    print_nl();
    print_int(arr[2]);
    print_nl();

    int x;
    x = arr[1];
    print_int(x);
    print_nl();

    int y;
    y = arr[1] + arr[2];
    print_int(y);
    print_nl();

    arr[1] = 9;
    print_int(arr[1]);
    print_nl();

    arr[2] = 2;
    print_int(arr[1] * arr[2]);
    print_nl();

    return 0;
}