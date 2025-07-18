#include<vector>
#include<algorithm>
#include<iostream>
#include<chrono>
#include <fstream>

int MAX_DATA_SIZE = 1e7+10000,w,s,k=400;

using namespace std;

struct HeapNode{
    double value;
    int l,r;
};

long long cnt = 0;

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


typedef struct {
    int start; //起始索引
    int end; //终止索引
    double L; //起始前累计得分
    double R; //起始后累计得分
    int point;
}sub_array;

int LL = 0;
int RR;
vector<double> input(MAX_DATA_SIZE);
vector<sub_array> wait(MAX_DATA_SIZE);
vector<sub_array> res(MAX_DATA_SIZE);
int top_wait = 0;
int top_res = 0;

int tot=0;
void clear() { // 这里是清空待选区~
    tot+=top_wait;
    while (top_wait != 0) {
        res[top_res++] = wait[--top_wait];
    }
}
void run2() { //开始运行
    double current = 0;
    double min_s = INT_MAX;
    for(int i = LL;i<=RR;i++) {
        if(input[i] > 0) {
            if(current<=min_s) { //如果当前的L小于等于min_s那么说明 备选区的数字已经无法合并，那么就可以清空备选区了，然后把新的子序列加入备选区更新min_s。
                clear();
                wait[top_wait++] = {i,i,current,current+input[i],-1};
                min_s = current;
            }else {
                wait[top_wait++] = {i,i,current,current+input[i],top_wait-1};
                int p = top_wait - 2;
                while (p > -1) {
                    if(wait[p].L < wait[top_wait-1].L) {
                        if (wait[p].R < wait[top_wait-1].R) {
                            wait[p].end = i;
                            wait[p].R = current + input[i];
                            top_wait = p + 1;
                        }else{
                            wait[top_wait-1].point = p;
                            break;
                        }
                    }
                    p = wait[p].point;
                }
            }
        }
        current += input[i];
    }
    clear();
}

void init() {
    //int arr[] = {4,-5,3,-3,1,2,-2,2,-2,1,5};
    vector<int> arr(MAX_DATA_SIZE);
    for(int i = 0;i<MAX_DATA_SIZE;i++) {
        arr[i] = -100 + (rand()%(200));
    }
    for(int i = 0;i<MAX_DATA_SIZE;i++) {
        input[i] = arr[i];
    }
}
void print() {  //打印输出，这里由于我是用的两个栈，所以有的时候部分元素会倒置，可以sort一下在最后
    Max_TopKHead->siz = 0;
    cnt += top_res;
}

void DataInit_input(){
    vector<double> a(MAX_DATA_SIZE+1);
    a[0] = 0;
    for(int i=1;i<=MAX_DATA_SIZE;++i){
        scanf("%lf",&a[i]);
    }

    for(int i = 1;i<MAX_DATA_SIZE;++i){
        input[i-1] = a[i+1] - a[i];
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
        MAX_DATA_SIZE=2000000;
        w=600000;
        s=50;
        k=20;
    }
    else{
        MAX_DATA_SIZE=std::atoi(argv[1]);
        w=std::atoi(argv[2]);
        s=std::atoi(argv[3]);
        k=std::atoi(argv[4]);
    }
    RR = w-1;


    choose_data(MAX_DATA_SIZE);
    DataInit_input();


    auto start = std::chrono::high_resolution_clock::now();

    while (1)
    {
        run2();
        print();
        

        top_wait = 0;
        top_res = 0; 
        
        LL += s;
        RR += s;
        if(RR >= MAX_DATA_SIZE - 2){
            RR = MAX_DATA_SIZE - 2;
            run2();
            print();
            break;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    
    return 0;
}
