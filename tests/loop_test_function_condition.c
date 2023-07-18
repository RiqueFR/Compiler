

int par(int i){
    while(i>1){
        i = i/2;
    }
    if(i==1){
        return 0;
    }
    else{
        return 1;
    }
    return 1;
}
int main(){
    int i = 20;
    while(par(i)){
        i = i/3;
    }
    return 0;
}
