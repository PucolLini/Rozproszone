int NWW(int a, int b){
    int max = (a>b) ? a : b;

    while(1){
        if((max % a == 0) && (max % b == 0)){
            break;
        }
        max++;
    }
    return max;
}