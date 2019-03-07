void bubble_sort(int[20] a, int n){
    while ( !( n < 2 ) )
    {
        int i = 0;
        int last = 1;

        while ( ++i < n )
        {
            if ( a[i] < a[i-1] )
            {
                int tmp = a[i]; 
                a[i] = a[i-1];
                a[i-1] = tmp;
                last = i;
            }
        }

        n = last;
    }
}

int main(){
    int[5] arr;
    int i = 0;
    int j = 0;

    int x;
    while(i < 5){
        print("Please enter a number for sorting (");
        print_int(i + 1);
        print("/5): ");
        x = read_int();
        print_nl();
        arr[i] = x;
        i = i+1;
    }

    bubble_sort(arr, 5);

    i = 0;
    while(i < 5){
        print_int(arr[i]);
        print("  ");
        i = i+1;
    }

    return 0;
}
