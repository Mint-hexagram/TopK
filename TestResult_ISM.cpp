#include <bits/stdc++.h>
const int maxn=2e7+10000;
#define ll long long
ll n,w,s;
int k=20;
double ori[maxn],sum[maxn];
ll heap_cost = 0;
ll ans_cnt = 0;
ll cnt_window = 0;
ll cnt_partition = 0;

ll cnt_use = 0;
ll cnt_create = 0;

class TopKHeap{
public:
    double* heap;
    ll siz;
    TopKHeap(){
        heap = new double[k+1];
        siz = 0;
    }
    void Push(double x){
        int now = ++siz;
        while(now>1){
            int p = now/2;
            if(heap[p] <= x) break;
            heap[now] = heap[p];
            now = p;
        }
        heap[now] = x;
    }
    double Pop(){
        double ans = heap[1];
        heap[1] = heap[siz];
        --siz;
        int now=1;
        while((now<<1)<=siz){
            int son = now<<1;
            if(son+1<=siz && heap[son+1]<heap[son]) ++son;
            if(heap[son]>=heap[now]) break;
            std::swap(heap[son],heap[now]);
            now = son;
        }
        return ans;
    }


    void cal_topk(double x){
        ++cnt_use;
        if(siz<k) Push(x);
        else{
            if(x<=heap[1]) return;
            else{
                Pop();
                Push(x);
            }
        } 
    }

    void print_topk(){
        int cnt_ans=0;
        while(siz!=0){
            Pop();
            ++cnt_ans;
        }
    }
};
TopKHeap* topkheap = new TopKHeap();

class Crest{//波峰类
public:
    int crestPlace;
    int troughPlace;
    int domainCrest;
    int l;
    int r;//维护crest的右侧边界，即mergecrests中的最右侧波谷
    std::vector<Crest*> merge_crests;
    std::vector<int> merge_place;
    Crest(){
        crestPlace = -1;
        troughPlace = -1;
        domainCrest = -1;
        l = -1;
        r = -1;//-1表示没有被更新
        ++cnt_create;
    }

    void display(){
        printf("crest_display\n");
        printf("起点=%d 终点=%d 支配点=%d val=%lld\n\n",l,r,domainCrest,ori[troughPlace] - ori[crestPlace]);
        printf("原终点=%d\n",troughPlace);
        for(auto crest : merge_crests){
            printf("子段=起点=%d 终点=%d 支配点=%d val=%lld\n\n",crest->l,crest->r,crest->domainCrest,ori[crest->troughPlace] - ori[crest->crestPlace]);
        }
    }
};

std::shared_ptr<std::deque<Crest*>> S_Max;
//对于整个窗口而言，用于进行整体划分的栈结构
std::shared_ptr<std::vector<Crest*>> S_Init;

std::shared_ptr<std::deque<Crest*>> S_trans;

struct Subsegment{
    int l,r;
    ll val;
}all[maxn];
bool cmp(double a,double b){return a > b;}

double max(double a,double b){
    return a>b?a:b;
}

//暂时存储所有段的值
double ans[maxn];
int L[maxn],R[maxn];
int cnt_ans=0;//段的数量

int new_Domain_Init(int L,int R,std::shared_ptr<std::vector<Crest*>> crests){
    int r=R;
    int l=L;

    while(ori[r]==ori[r-1] && r>=l+1) --r;
    //去掉右侧连续的0

    Crest* nowCrest = new Crest();
    int A=-1,B=r;


    for(int i=r;i>=l;){
        int ip=i-1;
        while(ori[ip]==ori[ip+1] &&ip>=l) --ip;

        if(ip==l-1){
            if(A!=-1){
                nowCrest->troughPlace = B;
                nowCrest->crestPlace = A;
                nowCrest->domainCrest = A;
                nowCrest->l = A;
                nowCrest->r = B;
                crests->push_back(nowCrest);
                break;
            }
            else{
                return l;
            }
        }

        if(ori[ip]<ori[ip+1]){
            if(A==-1) B=ip+1;
            A = ip;
        }
        else{
            if(A==-1) B=ip;
            else{
                nowCrest->troughPlace = B;
                nowCrest->crestPlace = A;
                nowCrest->domainCrest = A;
                nowCrest->l = A;
                nowCrest->r = B;
               crests->push_back(nowCrest);
                nowCrest = new Crest();
                A=-1;
                B=ip;
            }
        }
        i=ip;
    }

    return -1;
}

void Domain_Calculate(std::shared_ptr<std::deque<Crest*>> S_Max,std::shared_ptr<std::vector<Crest*>> S_Init){

    for(int i=S_Init->size()-1;i>=0;--i){
        Crest* right = S_Init->back();
        S_Init->pop_back();

        for(int j=S_Max->size()-1;j>=0;--j){
            Crest* left = S_Max->back();
            if((ori[left->domainCrest] >= ori[right->domainCrest]) && (ori[left->troughPlace] <= ori[right->troughPlace])){
                //不可以合并，但是可以发生屏蔽
                right->merge_crests.push_back(left);
                right->l = left->l;
                S_Max->pop_back();
            }
            else{
                if(ori[left->domainCrest] < ori[right->domainCrest] && ori[left->troughPlace] < ori[right->troughPlace]){
                    //不可以屏蔽，而是发生合并
                    right->merge_crests.push_back(left);
                    right->merge_place.push_back(left->domainCrest);
                    right->domainCrest = left->domainCrest;
                    right->l = left->l;

                    S_Max->pop_back();
                }
                else{
                    break;
                }
            }
        }
        S_Max->push_back(right);
    }
}


void Get_TopK(Crest* crest,int mod){
    if(mod==1){
        topkheap->cal_topk(ori[crest->troughPlace] - ori[crest->domainCrest]);
        ans_cnt++;
    }

    Crest* nowCrest;

    for(int i=crest->merge_crests.size()-1;i>=0;--i){
        nowCrest = crest->merge_crests.at(i);
        if(nowCrest->crestPlace < crest->domainCrest){
            Get_TopK(nowCrest,1);
        }
        else{
            Get_TopK(nowCrest,2);
            break;
        }
    }

}

void special_DataInit_input(){
    for(int i=1;i<=n;++i){
        if(i%2==0) ori[i] = i;
        else ori[i] = n-i+1;
    }
}

long long Rebuild_Cost = 0;

void deleteCrests(int r,Crest* crest,std::shared_ptr<std::deque<Crest*>> S_Max,Crest* Upper){
    if(r < crest->domainCrest){

        int cnt = crest->merge_crests.size()-1;
        Crest* nowCrest;
        for(int i=cnt;i>=0;--i){
            nowCrest = crest->merge_crests.back();
            if(nowCrest->l > r){
                break;
            }
            else{
                if(nowCrest->r <= r+1){
                    crest->merge_crests.pop_back();
                }
                else{
                    deleteCrests(r,nowCrest,S_Max,crest);
                    //递归地对被切开的段进行处理
                }
            }
        }
    }
    else{

        if(r >= crest->crestPlace){
            crest->merge_crests.clear();
            crest->merge_place.clear();
            crest->crestPlace = r+1;
            crest->domainCrest = r+1;
            crest->l = r+1;
        }//修改一个原子段
        else{
            //该段的最右侧原子段(crestplace,r)未被切开，仅切开被合并的部分
            int cnt = crest->merge_crests.size()-1;
            Crest* nowCrest;
            for(int i=cnt;i>=0;--i){
                nowCrest = crest->merge_crests.back();
                if(nowCrest->r <=r+1){
                    //该段被完全删除
                    crest->merge_crests.pop_back();
                    if(nowCrest->domainCrest == crest->domainCrest){
                        crest->merge_place.pop_back();
                        if(!crest->merge_place.empty()){
                            crest->domainCrest = crest->merge_place.back();

                        }
                        else{
                            crest->domainCrest = crest->crestPlace;
                        }
                    }
                }
                else{

                    bool need_modify = false;
                    if(nowCrest->domainCrest == crest->domainCrest) need_modify = true;

                    //该段并未被完全删除(删除不能越过该段，即后面的栈底的段不在会被删除)
                    deleteCrests(r,nowCrest,S_Max,crest);
                    //首先递归地对该段完成修改操作
                    int jugdePlace;//上一次的合并位置
                    if(!need_modify) break;
                    crest->merge_place.pop_back();
                    int judge = 0;
                    if(!crest->merge_place.empty()){
                        jugdePlace = crest->merge_place.back();
                    }
                    else{
                        jugdePlace = crest->crestPlace;
                    }
                    //接下来判断能否继续完成合并操作
                    if(ori[nowCrest->domainCrest] < ori[jugdePlace] && ori[nowCrest->troughPlace] < ori[crest->troughPlace]){
                        //第二个判断其实毫无必要，只是为了保持格式的工整
                        crest->domainCrest = nowCrest->domainCrest;
                        crest->merge_place.push_back(nowCrest->domainCrest);
                    }
                    else{
                        crest->domainCrest = jugdePlace;
                    }
                    break;
                }
            }
        }

    }
    crest->l = r+1;
}

void deletePartitionCrests(std::shared_ptr<std::deque<Crest*>> S_Max,int r){
    Crest* nowCrest;
    if(S_Max->empty()) return;
    int cnt = S_Max->size()-1;
    for(int i=0;i<=cnt;++i){
        nowCrest = S_Max->front();
        if(nowCrest->r <= r+1){
            S_Max->pop_front();
        }
        else{
            break;
        }//该段被r切开，因此不能被整体删除
    }//先删除所有被覆盖在[l,r]中的crest
    if(S_Max->empty()){
        return;
        //如果此时merge_crests已经被清空，则结束删除
    }

    if(nowCrest->l > r){
        return;
    }//如果该段没有被r切开，结束操作
    //否则继续对该段进行内部的细节删除处理

    deleteCrests(r,nowCrest,S_Max,nullptr);
}

void addRange_Modify(int l,int r){
    S_Init = std::make_shared<std::vector<Crest*>>();

    new_Domain_Init(l,r,S_Init);

    if(S_Init->empty()) return;

    Domain_Calculate(S_Max,S_Init);
}

//One_Pass_Moving_modify

void print_ans(){
    cnt_ans = 0;
    double ans[k+1];
    while(topkheap->siz!=0){
        ans[++cnt_ans] = topkheap->Pop();
    }
    std::sort(ans+1,ans+cnt_ans+1,cmp);
    for(int i=1;i<=cnt_ans;++i){
        printf("val=%lf\n",ans[i]);
    }
    printf("cnt_ans=%d\n",cnt_ans);
}


void judge_random_input(){
    freopen("input.txt","r",stdin);
    freopen("ISM.txt","w",stdout);
}
void Data_Init(){
    for(int i=1;i<=n;++i){
        scanf("%lf",&ori[i]);
    }
}

int main(int argc, char* argv[]){
    if(argc<2){
        n=20;
        w=10;
        s=1;
        k=20;
    }
    else{
        n=std::atoi(argv[1]);
        w=std::atoi(argv[2]);
        s=std::atoi(argv[3]);                                                                         
    }
    judge_random_input();
    Data_Init();

    S_Max = std::make_shared<std::deque<Crest*>>();
    S_Init = std::make_shared<std::vector<Crest*>>();
    S_trans = std::make_shared<std::deque<Crest*>>();

    new_Domain_Init(1,w,S_Init);

    Domain_Calculate(S_Max,S_Init);
    
    topkheap->siz = 0;
    for(auto crest : (*S_Max)){
        Get_TopK(crest,1);
    }
    print_ans();

    for(int i=w+1;i<=n;i=i+s){
        int L=i-1;
        int R=i+s-1;
        deletePartitionCrests(S_Max,i-w+s-1);

        addRange_Modify(L,R);
        topkheap->siz = 0;
        for(auto crest : (*S_Max)){
            Get_TopK(crest,1);
        }
        print_ans();
        
    }

    return 0;
}  