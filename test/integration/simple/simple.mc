int idiv(int x, int y){
    return x / y;
}

int main() {

    int x;

    print("give an int number: ");
    x = read_int();

    x = idiv(x, 2);
    
    print_nl();
    print_int(x);
    print_nl();

    return 0;
}