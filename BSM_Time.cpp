#include <bits/stdc++.h>
using namespace std;
#define ll long long
const int maxn=1e7+1000;
long long cnt_creat = 0;
long long cnt_use = 0;
struct Subsegment
{
    int l,r;
    double val;
}ans[maxn];
class Crest{
public:
    int L,R;
    Crest(int l,int r):L(l),R(r){
        ++cnt_creat;
    };
};
double sum[maxn],a[maxn];
int n,w,s;
void DataInit_random(){
    int k = time(0);
    srand(k);

    printf("time0=%d\n",k);
    for(int i=1;i<=n;++i){
        a[i] = (rand()%2000001);
        sum[i] = sum[i-1]+a[i];
    }
}
int k = 400;
struct HeapNode{
    double value;
    int l,r;
};
bool cmp_pos_L(HeapNode a,HeapNode b)
{
    return a.l < b.l;
}

HeapNode all_ans[maxn];

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
        ++cnt_use;
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

    void print_topk(){
        int cnt_ans=0;
        while(siz!=0){
            all_ans[++cnt_ans] = Pop();
        }
        std::sort(all_ans+1,all_ans+cnt_ans+1,cmp_pos_L);
        for(int i=1;i<=cnt_ans;++i){
//        printf("l=%d r=%d val=%lld\n",all_ans[i].l,all_ans[i].r,all_ans[i].value);
        printf("val=%lld\n",all_ans[i].value);
    }
    printf("cnt_ans=%d\n",cnt_ans);
    }
};
TopKHeap* Max_TopKHead = new TopKHeap();

void DataInit_input(){
//    n = 1000;
//    w = 1000;
//    s = 1;
    freopen("input.txt","r",stdin);
    freopen("KTwoPass.txt","w",stdout);
    for(int i=1;i<=n;++i){
        scanf("%lld",&a[i]);
    }
}

int extra_left = 0;

void getSubSet(int L,int R,std::shared_ptr<std::deque<Crest*>> S_tmp,int mod){
    //mod=0表示用于重构
    //mod=1表示用于增添新段
    int maxx_R=0;

    int l=L;
    Crest* now = new Crest(-1,-1);
    while(a[l]>=a[l+1] && l<=R-1) ++l;
    now->L = l;
    //对前导的负段进行判断去除
    for(int i=l;i<=R-1;++i){
        if(a[i]>=a[i+1]){
            if(now->L==-1) continue;
            //开始下降，则L为一个波峰的终止
            //或者是仍然处于下降过程中
            now->R=i;
            maxx_R = max(maxx_R,now->R);

            S_tmp->push_back(now);
            now = new Crest(-1,-1);
        }
        else{
            //开始上升，标志这一个递增段的开始
            //或者已经持续一段事件处于增长过程中
            if(now->L==-1) {
                now->L = i;
            }
        }
    }
    if(now->L!=-1 && now->R==-1 && now->L!=R){
        //说明，直到该窗口结束，该段仍未结束递增过程
        //因此，窗口的右端点成为最后一个递增子段的波峰
        now->R = R;
        maxx_R = max(maxx_R,now->R);
        S_tmp->push_back(now);
    }
    if(mod) extra_left = maxx_R;
//    printf("siz=%d\n",S_tmp->size());
//    printf("now_L=%d now_R=%d\n",now->L,now->R);

}

void MSGA_start(int L,int R,std::shared_ptr<std::deque<Crest*>> S_Max){
    std::shared_ptr<std::deque<Crest*>> S_tmp = std::make_shared<std::deque<Crest*>>();
    getSubSet(L,R,S_tmp,1);
    //获得原子递增段的完整集合
    Crest* now;
    Crest* tmp;
    while(!S_tmp->empty()){
        now=S_tmp->front();
        S_tmp->pop_front();
        int place=-1;//当前能够合并的最远位置，如果为-1则说明没有发生合并
        for(int i=0;i<S_tmp->size();++i){
            tmp = S_tmp->at(i);
            if(a[now->L]<a[tmp->L] && a[now->R]<a[tmp->R]){
                place=i;
                now->R=tmp->R;
            }
            if(a[now->L] >= a[tmp->L]) break;
        }
        if(place!=-1){
            now->R = S_tmp->at(place)->R;
            for(int i=0;i<=place;++i) S_tmp->pop_front();
        }
        S_Max->push_back(now);
    }
}

void MSGA_new(int L,int R,std::shared_ptr<std::deque<Crest*>> S_Max){
    std::shared_ptr<std::deque<Crest*>> S_tmp = std::make_shared<std::deque<Crest*>>();
    getSubSet(L,R,S_tmp,1);
    //获得原子递增段的完整集合
    Crest* now;
    Crest* tmp;
    while(!S_tmp->empty()){
        now=S_tmp->front();
//        if(L==10) printf("AAAAAAAAAAAAA L=%d R=%d\n",now->L,now->R);
        S_tmp->pop_front();
        int place=-1;//当前能够合并的最远位置，如果为-1则说明没有发生合并
//        int cnt_place = 0;
        for(int i=S_Max->size()-1;i>=0;--i){
            tmp = S_Max->at(i);
            if(a[now->L] > a[tmp->L] && a[now->R] > a[tmp->R]){
                place=i;
                now->L = tmp->L;
//                ++cnt_place;
            }
            if(a[now->R] <= a[tmp->R]) break;
        }
        if(place!=-1){
/*            if(L==10){
                printf("???\n");
                printf("AAAAAAAAAAAAA L=%d R=%d\n",tmp->L,tmp->R);
                printf("AAAAAAAAAAAAA L=%d R=%d\n",now->L,now->R);
            }*/

            now->L = S_Max->at(place)->L;
            for(int j=S_Max->size()-1;j>=place;--j) S_Max->pop_back();
        }
//        if(L==10) printf("AAAAAAAAAAAAA L=%d R=%d\n",now->L,now->R);
        S_Max->push_back(now);
    }
}

void MSGA_rebuild(int L,int R,std::shared_ptr<std::deque<Crest*>> S_Max){
    std::shared_ptr<std::deque<Crest*>> S_tmp = std::make_shared<std::deque<Crest*>>();
    getSubSet(L,R,S_tmp,1);
    std::shared_ptr<std::deque<Crest*>> S_trans = std::make_shared<std::deque<Crest*>>();
    //获得原子递增段的完整集合
    Crest* now;
    Crest* tmp;
    while(!S_tmp->empty()){
        now=S_tmp->front();
        S_tmp->pop_front();
        int place=-1;//当前能够合并的最远位置，如果为-1则说明没有发生合并
        for(int i=0;i<S_tmp->size();++i){
            tmp = S_tmp->at(i);
            if(a[now->L]<a[tmp->L] && a[now->R]<a[tmp->R]){
                place=i;
                now->R=tmp->R;
            }
            if(a[now->L] >= a[tmp->L]) break;
        }
        if(place!=-1){
            now->R = S_tmp->at(place)->R;
            for(int i=0;i<=place;++i) S_tmp->pop_front();
        }
        S_trans->push_back(now);
    }
    S_Max->insert(S_Max->begin(),S_trans->begin(),S_trans->end());
}

bool cmp_pos_l(Subsegment a,Subsegment b)
{
    return a.l < b.l;
}
std::shared_ptr<std::deque<Crest*>> S_Max;

void get_ans(){
    int cnt_ans=0;
    Max_TopKHead->siz=0;
    for(int i=0;i<S_Max->size();++i){
        Crest* now = S_Max->at(i);
        Max_TopKHead->cal_topk((HeapNode){a[now->R] - a[now->L],now->L,now->R});
//        printf("vvvvvvvvvvvvvvvvv=%d\n",a[now->R] - a[now->L]);
    }
//    Max_TopKHead->print_topk();
//    printf("AAAAAAAAAAA\n");
//    printf("cnt_ans=%d\n",Max_TopKHead->siz);
}

void delete_Crest(int r,std::shared_ptr<std::deque<Crest*>> S_Max){
    int cnt = S_Max->size()-1;

/*    if(r==10 || 1){
        for(auto nowCrest : *S_Max){
            printf("L=%d R=%d\n",nowCrest->L,nowCrest->R);
        }
            
    }*/

    for(int i=0;i<=cnt;++i){
        auto nowCrest = S_Max->front();
        

        if(nowCrest->R <= r+1){
            S_Max->pop_front();
        }
        else if(nowCrest->L > r) break;
        else{
            S_Max->pop_front();
            int new_L = r+1;
            int new_R = nowCrest->R;
            MSGA_rebuild(new_L,new_R,S_Max);
        }
    }
}

void Add_Crest(int l,int r,std::shared_ptr<std::deque<Crest*>> S_Max){
//    printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
//    printf("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n");
//    get_ans();
//    printf("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n");
    l = max(1,l-1);
    MSGA_new(l,r,S_Max);
//    printf("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n");
    get_ans();
//    printf("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n");
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

void Data_Init(){
    for(int i=1;i<=n;++i){
        scanf("%lf",&a[i]);
    }
}

int main(int argc, char* argv[]) {
    if(argc<2){
        n=6000000;
        w=600000;
        s=50;                                   
    }
    else{
        n=std::atoi(argv[1]);
        w=std::atoi(argv[2]);
        s=std::atoi(argv[3]);
        k=std::atoi(argv[4]);
    }

    choose_data(n);
    Data_Init();

    

    auto start = std::chrono::high_resolution_clock::now();

    S_Max = std::make_shared<std::deque<Crest*>>();
    
    MSGA_start(1,w,S_Max);
    get_ans();

    cout << sizeof(Crest) << endl;
    printf("cnt=%d\n",cnt_creat);


    return 0;

    for(int i=w+1;i<=0;i=i+s){
        int R=i-w+s-1;
//        S_Max->clear();
        delete_Crest(R,S_Max);
//        printf("R=%d\n",R);
//        printf("l=%d\n",std::min(i+s-1,n));
        Add_Crest(i,std::min(i+s-1,n),S_Max);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Elapsed time: " << duration.count() << " seconds" << std::endl;

    printf("cnt_creat=%lld\n",cnt_creat);
    printf("cnt_use=%lld\n",cnt_use);
    return 0;
}