#include<bits/stdc++.h>
using namespace std;
const int maxn=1e7+70;
double a[maxn];
int main(int argc, char* argv[]){
    int n,w;
    if(argc<2){
        n=10000000;
        w=20;
    }
    else{
        n=std::atoi(argv[1]);
        w=std::atoi(argv[2]);
    }
    freopen("input.txt","w",stdout);
    int sand = time(0);
    srand(sand);
    for(int i=1;i<=n;++i)
    {
        a[i] = (rand()%20000001);
        printf("%lf ",a[i]);
    }
    return 0;
}