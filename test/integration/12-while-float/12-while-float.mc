int main(){
    
    float a;
    float b;

    a = 0.0;
    b = 10.0;

    while (a < 4.0){

        while(b < 12.0){
            b = b + 1.0;
            print_float(b);
        }

        a = a + 1.0;
        print_float(a);
    }

    return 0;
}