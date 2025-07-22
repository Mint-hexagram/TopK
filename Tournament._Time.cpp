#include<vector>
#include<algorithm>
#include<iostream>
#include<chrono>
#include <fstream>
#include <climits>
#include<cstdio>
#define lson (x*2)
#define rson (x*2+1)

const int MAXN=1e7+100000;

using namespace std;
typedef struct Node{
    double L; 
    double R; 
    double val; 
    int idx_L; 
    int idx_R;
    int from; 
    int to;
    int val_from; 
    int val_to;
    bool is_hole = false; 
}node;
node Nodes[4000000+400];
int n = 12,w,s,k=400;

struct HeapNode{
    double value;
    int l,r;
    HeapNode() : value(0.0), l(0), r(0) {}
    HeapNode(double v, int left, int right) : value(v), l(left), r(right) {}
};

class TopKHeap{
public:
    HeapNode* heap;
    int siz;
    TopKHeap(){
        heap = new HeapNode[k+1];
        siz = 0;
    }
    void Push(HeapNode x){
        int now = ++siz;
        while(now>1){
            int p = now/2;
            if(heap[p].value <= x.value) break;
            heap[now] = heap[p];
            now = p;
        }
        heap[now] = x;
    }
    HeapNode Pop(){
        HeapNode ans = heap[1];
        heap[1] = heap[siz];
        --siz;
        int now=1;
        while((now<<1)<=siz){
            int son = now<<1;
            if(son+1<=siz && heap[son+1].value<heap[son].value) ++son;
            if(heap[son].value>=heap[now].value) break;
            std::swap(heap[son],heap[now]);
            now = son;
        }
        return ans;
    }


    bool cal_topk(HeapNode x){
        if(siz<k) Push(x);
        else{
            if(x.value<=heap[1].value) return false;
            else{
                Pop();
                Push(x);
            }
        }
        return true; 
    }
};
TopKHeap* Max_TopKHead = new TopKHeap();





double  input[MAXN];
double  prefix[MAXN];



void build(int x,int l,int r) {

    if(l == r) {
        //node tmp = {l,r,input[l],nullptr,nullptr};
        Nodes[x].L = prefix[l] - input[l];
        Nodes[x].R = prefix[l];
        Nodes[x].val = input[l];
        Nodes[x].from = l;
        Nodes[x].to = r;
        Nodes[x].val_from = l;
        Nodes[x].val_to = r;
        Nodes[x].idx_L = l;
        Nodes[x].idx_R = r;
        return;
    }

    int mid = (l + r - 1) / 2;
    build(lson,l,mid);
    build(rson,mid + 1,r);

    if (Nodes[lson].L >= Nodes[rson].L) {
        Nodes[x].L = Nodes[rson].L;
        Nodes[x].idx_L = Nodes[rson].idx_L;
    }else {
        Nodes[x].L = Nodes[lson].L;
        Nodes[x].idx_L = Nodes[lson].idx_L;
    }
    if (Nodes[lson].R >= Nodes[rson].R) {
        Nodes[x].R = Nodes[lson].R;
        Nodes[x].idx_R = Nodes[lson].idx_R;
    }else {
        Nodes[x].R = Nodes[rson].R;
        Nodes[x].idx_R = Nodes[rson].idx_R;
    }

    if (Nodes[lson].val>Nodes[rson].val||(Nodes[lson].val == Nodes[rson].val && Nodes[lson].val_to - Nodes[lson].val_from < Nodes[rson].val_to - Nodes[rson].val_from)) {
        Nodes[x].val = Nodes[lson].val;
        Nodes[x].val_from = Nodes[lson].val_from;
        Nodes[x].val_to = Nodes[lson].val_to;
    }else {
        Nodes[x].val = Nodes[rson].val;
        Nodes[x].val_from = Nodes[rson].val_from;
        Nodes[x].val_to = Nodes[rson].val_to;
    }
    if (Nodes[rson].R-Nodes[lson].L>Nodes[x].val||
        (Nodes[rson].R-Nodes[lson].L == Nodes[x].val && 
            (Nodes[rson].idx_R - Nodes[lson].idx_L < Nodes[x].val_to - Nodes[x].val_from||
                (Nodes[rson].idx_R - Nodes[lson].idx_L == Nodes[x].val_to - Nodes[x].val_from&&Nodes[lson].idx_L > Nodes[x].val_from)))) {
        Nodes[x].val = Nodes[rson].R-Nodes[lson].L;
        Nodes[x].val_from = Nodes[lson].idx_L;
        Nodes[x].val_to = Nodes[rson].idx_R;
    }

    Nodes[x].from = l;
    Nodes[x].to = r;

    //cout<<l<<' '<<r<<' '<<newNode->val<<endl;
}
void update(int x,int hole_l,int hole_r) {

    if(Nodes[x].from>hole_r||Nodes[x].to<hole_l) return;

    if(Nodes[x].from>=hole_l&&Nodes[x].to<=hole_r) {
        Nodes[x].is_hole = true;
        Nodes[x].val = INT_MIN;
        Nodes[x].L = INT_MAX;
        Nodes[x].R = INT_MIN;
        return;
    }

    if(Nodes[x].is_hole == true&&Nodes[x].val == INT_MIN&&Nodes[x].L == INT_MAX&&Nodes[x].R == INT_MIN) return;

    update(lson,hole_l,hole_r);
    update(rson,hole_l,hole_r);

    if(Nodes[rson].is_hole) Nodes[x].L = Nodes[rson].L,Nodes[x].idx_L = Nodes[rson].idx_L;
    else {

        if(Nodes[lson].L >= Nodes[rson].L) {
            Nodes[x].L = Nodes[rson].L;
            Nodes[x].idx_L = Nodes[rson].idx_L;
        }else {
            Nodes[x].L = Nodes[lson].L;
            Nodes[x].idx_L = Nodes[lson].idx_L;
        }
    }
    if(Nodes[lson].is_hole) Nodes[x].R = Nodes[lson].R,Nodes[x].idx_R = Nodes[lson].idx_R;
    else {

        if(Nodes[lson].R >= Nodes[rson].R) {
            Nodes[x].R = Nodes[lson].R;
            Nodes[x].idx_R = Nodes[lson].idx_R;
        }else {
            Nodes[x].R = Nodes[rson].R;
            Nodes[x].idx_R = Nodes[rson].idx_R;
        }
    }

    if(Nodes[lson].is_hole||Nodes[rson].is_hole) Nodes[x].is_hole = true;

    if (Nodes[lson].val>Nodes[rson].val||(Nodes[lson].val == Nodes[rson].val && Nodes[lson].val_to - Nodes[lson].val_from < Nodes[rson].val_to - Nodes[rson].val_from)) {
        Nodes[x].val = Nodes[lson].val;
        Nodes[x].val_from = Nodes[lson].val_from;
        Nodes[x].val_to = Nodes[lson].val_to;
    }else {
        Nodes[x].val = Nodes[rson].val;
        Nodes[x].val_from = Nodes[rson].val_from;
        Nodes[x].val_to = Nodes[rson].val_to;
    }
    if (Nodes[rson].R-Nodes[lson].L>Nodes[x].val||
        (Nodes[rson].R-Nodes[lson].L == Nodes[x].val && 
            (Nodes[rson].idx_R - Nodes[lson].idx_L < Nodes[x].val_to - Nodes[x].val_from||
                (Nodes[rson].idx_R - Nodes[lson].idx_L == Nodes[x].val_to - Nodes[x].val_from&&Nodes[lson].idx_L > Nodes[x].val_from)))) {
        Nodes[x].val = Nodes[rson].R-Nodes[lson].L;
        Nodes[x].val_from = Nodes[lson].idx_L;
        Nodes[x].val_to = Nodes[rson].idx_R;
    }
}

double a[MAXN];
void DataInit_input(){
    a[0] = 0;
    for(int i=1;i<=n;++i){
        scanf("%lf",&a[i]);
    }

    for(int i = 1;i<n;++i){
        input[i-1] = a[i+1] - a[i];
        prefix[i-1] = a[i+1] - a[1];
    }
}
void run2(){
    int LL = 0,RR = w-1;

    
    while (1)
    {
        build(1,LL,RR);
        int cnt=0;
        while (Nodes[1].val>0) {
            HeapNode h;
            h.value=Nodes[1].val;
            h.l = Nodes[1].val_from;
            h.r = Nodes[1].val_to;
            Max_TopKHead->cal_topk(h);
            update(1,Nodes[1].val_from ,Nodes[1].val_to);
            ++cnt;
            if(cnt>k) break;
        }
        LL += s;
        RR += s;
        Max_TopKHead->siz = 0;
        if(RR >= n - 2){
            RR = n - 2;
            build(1,LL,RR);
            while (Nodes[1].val>0) {
               HeapNode h;
                h.value=Nodes[1].val;
                h.l = Nodes[1].val_from;
                h.r = Nodes[1].val_to;
                Max_TopKHead->cal_topk(h);
                update(1,Nodes[1].val_from ,Nodes[1].val_to);
                ++cnt;
                if(cnt>k) break;
            }
            break;
        }
    }

}

void A_High_DataInit_input(){freopen("A_total.txt","r",stdin);}
void B_High_DataInit_input(){freopen("B_total.txt","r",stdin);}
void C_High_DataInit_input(){freopen("C_total.txt","r",stdin);}
void D_High_DataInit_input(){freopen("D_total.txt","r",stdin);}
void E_High_DataInit_input(){freopen("E_total.txt","r",stdin);}
void F_High_DataInit_input(){freopen("input_new.txt","r",stdin);}
void normal_DataInit_input(){freopen("normal_input.txt","r",stdin);}
void uniform_DataInit_input(){freopen("uniform_input.txt","r",stdin);}
void trip_DataInit_input(){freopen("trip_counts_sequence.txt","r",stdin);}

void choose_data(int n){
    if(n==7107586) A_High_DataInit_input();
    if(n==9028998) B_High_DataInit_input();
    if(n==4087684) C_High_DataInit_input();
    if(n==5506870) D_High_DataInit_input();
    if(n==3943016) E_High_DataInit_input();
    if(n==9987840) trip_DataInit_input();
    if(n==10000000) normal_DataInit_input();
    if(n==9999999) uniform_DataInit_input();        
}



int main(int argc, char* argv[])
{
    if(argc<2){
        n=7000000;
        w=600000;
        s=2000;
        freopen("A_total.txt","r",stdin);
    }
    else{
        n=std::atoi(argv[1]);
        w=std::atoi(argv[2]);
        s=std::atoi(argv[3]);
        k=std::atoi(argv[4]);
    }

    choose_data(n);

    DataInit_input();

    auto start = std::chrono::high_resolution_clock::now();

    run2();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    printf("%lf",duration.count());

    return 0;
}
