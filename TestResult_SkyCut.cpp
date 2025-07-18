#include <bits/stdc++.h>
using namespace std;
const int maxn=1e7+10000;
#define ll long long
ll n,w,s;
int k=20;

ll cnt_use = 0;
ll cnt_create = 0;

double ori[maxn];
//nxt指向左侧
//pre指向右侧

ll cnt_partition = 0;

ll ans_cnt = 0;

ll cnt_operating_crest = 0;
ll cnt_create_crest = 0;

struct MapNode{
    double value;
    ll id;

    bool operator < (const MapNode& other) const{
        if(value!=other.value) value > other.value;
        return id > other.id;
    }
};

struct HeapNode{
    double value;
    int l,r;
};
bool cmp_pos_L(HeapNode a,HeapNode b){return a.value > b.value;}

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
        for(int i=1;i<=min(cnt_ans,k);++i){
           printf("val=%lf\n",all_ans[i].value);
        }
        printf("cnt_ans=%d\n",cnt_ans);
    }
};
TopKHeap* Max_TopKHead = new TopKHeap();


class Crest{//波峰类
public:
    int crestPlace;
    int troughPlace;
    int domainCrest;
    int l;
    int r;//维护crest的右侧边界，即mergecrests中的最右侧波谷
//    std::shared_ptr<std::deque<Crest*>> merge_crests =  std::make_shared<std::deque<Crest*>>();;
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


};


class Partition{
public:
    ll id;
    double value;
    int l,r;
    int maxPlace;
    TopKHeap* topkheap;
    bool Init_State = false;//表示是否未构成crest结构
    bool deleted;//表示crest结构部分是否发生了删除，同时因为懒标记原因，没有处理删除结果
    bool updateHeap = false;
    //    TopKHeap* topkheap;

    //false表示disMerge
    //true表示isMerge
    //合并形成的Partition的左右两个来源(的指针）
    //合并过程中，两个合并段中间被屏蔽掉的若干partition

    //类似于线段树的lazy_tag

    std::shared_ptr<std::deque<Crest*>> merge_crests;

    Partition(){
        value = 0;
        id = ++cnt_partition;
        deleted = false;
        maxPlace = -1;
        topkheap =  new TopKHeap();
        merge_crests = std::make_shared<std::deque<Crest*>>();
    }

};
std::map<MapNode,Partition*> mp_Partition;

std::shared_ptr<std::deque<Crest*>> S_Init;

std::shared_ptr<std::deque<Partition*>> S_MaxPartition;
//对于整个窗口而言，用于进行整体划分的栈结构

struct Subsegment{
    int l,r;
    ll val;
}all[maxn];
bool cmp_pos_l(Subsegment a,Subsegment b)
{
    return a.l < b.l;
}

ll max(ll a,ll b){
    return a>b?a:b;
}

int cnt_ans=0;//段的数量

void MIP(int L,int R,std::shared_ptr<std::deque<Partition*>> S_MaxPartition_tmp,int mod){
    int W=R;
    while(W>L && ori[W-1]>=ori[W]) --W;

    int MaxEnd = W;
    int minStart = -1;
    Partition *nowPartition = new Partition();
    for(int i=W-1;i>=L;--i){
        if(ori[i]>=ori[MaxEnd]){
            if(minStart==-1){
                MaxEnd=i;
                continue;
            }
            //该partition应当结束
            nowPartition->r = MaxEnd;
            nowPartition->l = i+1;
            nowPartition->maxPlace = minStart;
            S_MaxPartition_tmp->push_front(nowPartition);

            nowPartition->value = ori[nowPartition->r] - ori[nowPartition->maxPlace];
            if(mod==1) mp_Partition[(MapNode){nowPartition->value,nowPartition->id}] = nowPartition;
            
            nowPartition = new Partition();
            MaxEnd = i;
            minStart = -1;
        }
        else{
            if(minStart==-1) minStart=i;
            else{
                if(ori[i] < ori[minStart]) minStart = i;
            }
        }
    }
    if(minStart!=-1){
        nowPartition->r = MaxEnd;
        nowPartition->l = L;
        nowPartition->maxPlace = minStart;
        nowPartition->value = ori[nowPartition->r] - ori[nowPartition->maxPlace];
        S_MaxPartition_tmp->push_front(nowPartition);
        if(mod==1) mp_Partition[(MapNode){nowPartition->value,nowPartition->id}] = nowPartition;
    }
}

int new_Domain_Init(int L,int R,std::shared_ptr<std::deque<Crest*>> crests){
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

                ++cnt_create_crest;

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

                ++cnt_create_crest;

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

void Domain_Calculate(std::shared_ptr<std::deque<Crest*>> S_Max,std::shared_ptr<std::deque<Crest*>> S_Init){

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

void Domain_Calculate_Transfer(std::shared_ptr<std::deque<Crest*>> S_Max,std::shared_ptr<std::deque<Crest*>> S_Init){

    if(S_Init->empty()) return;
    int cnt = S_Init->size()-1;
    for(int i=0;i<=cnt;++i){
        Crest* right = S_Init->front();
        S_Init->pop_front();

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

void deleteCrests(int r,Crest* crest){
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
                    delete nowCrest;
                }
                else{
                    deleteCrests(r,nowCrest);
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
//                    if(r==3 && need_modify) printf("444444right\n");
                    //该段并未被完全删除(删除不能越过该段，即后面的栈底的段不在会被删除)
                    deleteCrests(r,nowCrest);
                    //首先递归地对该段完成修改操作
                    int jugdePlace;//上一次的合并位置
                    if(!need_modify) break;
                    crest->merge_place.pop_back();
                    int judge = 0;
                    if(!crest->merge_place.empty()){
                        jugdePlace = crest->merge_place.back();
//                        judge = 1;
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

void deletePartitionCrests(std::shared_ptr<std::deque<Crest*>> merge_crests,int r){
    //处理r落在rest的散段范围中的情况
    Crest* nowCrest;
    if(merge_crests->empty()) return;
    int cnt = merge_crests->size()-1;
    for(int i=0;i<=cnt;++i){
        nowCrest = merge_crests->front();
        if(nowCrest->r <= r+1){
            merge_crests->pop_front();
            delete nowCrest;
        }
        else{
            break;
        }//该段被r切开，因此不能被整体删除
    }//先删除所有被覆盖在[l,r]中的crest
    if(merge_crests->empty()){
        return;
        //如果此时merge_crests已经被清空，则结束删除
    }
    if(nowCrest->l > r){
        return;
    }//如果该段没有被r切开，结束操作
    //否则继续对该段进行内部的细节删除处理


    deleteCrests(r,nowCrest);
}

ll version1_rebuild_len = 0;
ll rebuild_len = 0;

void deleteSinglePartition(Partition* partition,int r){ 
    partition->l = r+1;

    if(r < partition->maxPlace){
        
        partition->deleted = true;
        return;
//        deletePartitionCrests(partition->merge_crests,r);
    }
    else{
        partition->merge_crests->clear();
        int new_minn = partition->r;
        for(int i=partition->r;i>r;--i){
            if(ori[i] < ori[new_minn]){
                new_minn = i;
            }
        }
        partition->maxPlace = new_minn;

        double old_value = partition->value;
        partition->value = ori[partition->r] - ori[partition->maxPlace];
        mp_Partition.erase((MapNode){old_value,partition->id});
        mp_Partition[(MapNode){partition->value,partition->id}] = partition;
        //对partition的top-1值变动，在平衡树中同步修改（修改=删除+插入新值）
        partition->deleted = true;

        rebuild_len += partition->r - partition->maxPlace;

        S_Init->clear();
        new_Domain_Init(partition->l,partition->maxPlace-1,S_Init);
        Domain_Calculate(partition->merge_crests,S_Init);
    }
    
}

ll Transfer_rebuild_cost = 0;

void Transfer_Single_Partition(Partition* p_left,Partition* p_right){
    if(p_left->deleted){
        deletePartitionCrests(p_left->merge_crests,p_left->l-1);
        p_left->deleted = false;
        //完成删除+清空懒标记
    }//处理左邻近段的delete懒标记

    //情况四，两个true
    if(!p_left->Init_State && !p_right->Init_State){
        p_right->l = p_left->l;
        mp_Partition.erase((MapNode){p_left->value,p_left->id});
        return;
    }

    if(!p_left->Init_State){
        S_Init->clear();
        new_Domain_Init(p_left->l,p_left->r,S_Init);
        Domain_Calculate(p_left->merge_crests,S_Init);
    }
    else{
        S_Init->clear();
        new_Domain_Init(p_left->maxPlace,p_left->r,S_Init);
        Domain_Calculate(p_left->merge_crests,S_Init);
    }

    if(!p_right->Init_State){
        S_Init->clear();
        new_Domain_Init(p_right->l,p_right->maxPlace-1,S_Init);
        Domain_Calculate(p_right->merge_crests,S_Init);
        p_right->Init_State = true;
    }

    if(p_right->merge_crests->empty()){
        p_right->merge_crests = p_left->merge_crests;
    }
    else{
        Crest* rightCrest = p_right->merge_crests->front();
        p_right->merge_crests->pop_front();
        S_Init->clear();
        new_Domain_Init(rightCrest->l,rightCrest->troughPlace,S_Init);
        Domain_Calculate(p_left->merge_crests,S_Init);
        int cnt=p_left->merge_crests->size()-1;
        for(int i=cnt;i>=0;--i){
            p_right->merge_crests->push_front(p_left->merge_crests->back());
            p_left->merge_crests->pop_back();
        }
    }

    
    Transfer_rebuild_cost += p_right->maxPlace-1 - p_left->maxPlace + 1;
    p_right->l = p_left->l;
}

void Get_ans_stack(Crest* crest,int mod){
    if(mod==1){
        all[++cnt_ans].val = ori[crest->troughPlace] - ori[crest->domainCrest];
       all[cnt_ans].l = crest->domainCrest;
        all[cnt_ans].r = crest->troughPlace;
    }

    Crest* nowCrest;

    for(int i=crest->merge_crests.size()-1;i>=0;--i){
        nowCrest = crest->merge_crests.at(i);
        if(nowCrest->crestPlace < crest->domainCrest){
            Get_ans_stack(nowCrest,1);
        }
        else{
            Get_ans_stack(nowCrest,2);
            break;
        }
    }

}

void Get_ans(Partition* partition){
    all[++cnt_ans].val = ori[partition->r] - ori[partition->maxPlace];
    all[cnt_ans].l = partition->maxPlace;
    all[cnt_ans].r = partition->r;

    if(partition->merge_crests->empty()){
        return;
    }

    for(auto crest : *(partition->merge_crests)){
        Get_ans_stack(crest,1);
    }
}


void addRange(int l,int r){
    l-=1;

    //至多利用到上一个区间的最后一个点
    std::shared_ptr<std::deque<Partition*>> Tmp_Deque = std::make_shared<std::deque<Partition*>>();
    MIP(l,r,Tmp_Deque,2);
    
    if(Tmp_Deque->empty()) return;

    Partition* right = Tmp_Deque->front();

    for(int i=S_MaxPartition->size()-1;i>=0;--i){
            

        Partition* left = S_MaxPartition->back();
        
        
        int can_merge = 0;
        if(ori[left->maxPlace] < ori[right->maxPlace] && ori[left->r] < ori[right->r]){
            can_merge = 1;
        }

        if(ori[left->r] >= ori[right->r]){
            //从右向左的合并潜力被打断，终止所有合并过程
            //开始deque_modify过程
            break;
        }
        
        if(can_merge){
            //发生合并
            mp_Partition.erase((MapNode){left->value,left->id});

            left->r = right->r;
            delete right;
            right = left;
            S_MaxPartition->pop_back();
        }
        else{
            //发生屏蔽
            //Transfer
            mp_Partition.erase((MapNode){left->value,left->id});
            Transfer_Single_Partition(left,right);
            right->updateHeap = true;
            S_MaxPartition->pop_back();
        }
    }
    mp_Partition[(MapNode){right->value,right->id}] = right;
    S_MaxPartition->push_back(right);
    Tmp_Deque->pop_front();
    int cnt=Tmp_Deque->size()-1;
    for(int i=0;i<=cnt;++i){
        right = Tmp_Deque->front();
        S_MaxPartition->push_back(right);
        mp_Partition[(MapNode){right->value,right->id}] = right;
        Tmp_Deque->pop_front();
    }
}

void deletePartition(int r){
    //删除范围的右端点
    for(int i=0;i<S_MaxPartition->size();++i){
        Partition* partition = S_MaxPartition->at(i);
        if(partition->r <= r || partition->r == r+1){
            S_MaxPartition->pop_front();
            mp_Partition.erase((MapNode){partition->value,partition->id});
            delete partition;            
            --i;
        }
        else if(partition->l > r) break;
        else{
            
            partition->l = r+1;
            //打上懒标记
            deleteSinglePartition(partition,r);
        }
    }
}


void print_ans(){
    cnt_ans = 0;
    for(auto partition : (*S_MaxPartition)){
        Get_ans(partition);
    }
    std::sort(all+1,all+cnt_ans+1,cmp_pos_l);
    for(int i=1;i<=cnt_ans;++i){
        printf("val=%lld\n",all[i].val);
    }
    printf("cnt_ans=%d\n",cnt_ans);
}

void Get_TopK_crests(Crest* crest,int mod,Partition* partition){
    if(mod==1){
        if(!partition->topkheap->cal_topk((HeapNode){ori[crest->troughPlace] - ori[crest->domainCrest],crest->domainCrest,crest->troughPlace})) return;
    }

    Crest* nowCrest;

    for(int i=crest->merge_crests.size()-1;i>=0;--i){
        nowCrest = crest->merge_crests.at(i);
        if(nowCrest->crestPlace < crest->domainCrest){
            Get_TopK_crests(nowCrest,1,partition);
        }
        else{
            Get_TopK_crests(nowCrest,2,partition);
            break;
        }
    }

}

void Get_TopK_partition(Partition* partition){
    partition->topkheap->siz = 0;
   for(auto crest : *(partition->merge_crests)){
        Get_TopK_crests(crest,1,partition);
    }
}

void calc_start(){
    Max_TopKHead->siz = 0;
    auto it = mp_Partition.begin();
    std::vector<Partition*> topk_Partition;
    int cnt=0;
    for (int i=0; i < k && it != mp_Partition.end(); ++i, ++it) {
        Partition* now = it->second;

        topk_Partition.push_back(now);

        Max_TopKHead->cal_topk((HeapNode){ori[now->r] - ori[now->maxPlace],now->maxPlace,now->r}); 
        ++cnt;       
    }

    for(int i=0;i<=cnt-1;++i){
        Partition* now = topk_Partition.at(i);
        
        
        
        bool rebuild_heap = false;

        if(!now->Init_State){
            S_Init->clear();
            new_Domain_Init(now->l,now->maxPlace-1,S_Init);
            Domain_Calculate(now->merge_crests,S_Init);
            now->Init_State = true;
            rebuild_heap = true;
        }

        if(now->deleted){

            deletePartitionCrests(now->merge_crests,now->l-1);
            now->deleted = false;
            rebuild_heap = true;
        }
        
        if(rebuild_heap || now->updateHeap){
            Get_TopK_partition(now);
            now->updateHeap = false;
        }

        if(Max_TopKHead->siz==k   &&   Max_TopKHead->heap[0].value >= ori[now->r] - ori[now->maxPlace]) break;

        for(int j=1;j<=now->topkheap->siz;++j){
            Max_TopKHead->cal_topk((HeapNode){now->topkheap->heap[j].value,now->topkheap->heap[j].l,now->topkheap->heap[j].r});
        }

//        Get_TopK_crests(now);
    }
    Max_TopKHead->print_topk();
}


void judge_random_input(){
    freopen("input.txt","r",stdin);
    freopen("SkyCut.txt","w",stdout);
}


void special_DataInit_input(){
    for(int i=1;i<=n;++i){
        if(i%2==0) ori[i] = i;
        else ori[i] = n-i+1;
    }
}
void Data_Init(){for(int i=1;i<=n;++i){scanf("%lf",&ori[i]);}}


int main(int argc, char* argv[]){
    if(argc<2){
        n=2000;
        w=1000;
        s=10;
        k=20;   
    }
    else{

        n=std::atoi(argv[1]);
        w=std::atoi(argv[2]);
        s=std::atoi(argv[3]);
            
    }
    judge_random_input();
    Data_Init();

    auto start = std::chrono::high_resolution_clock::now();
    S_MaxPartition = std::make_shared<std::deque<Partition*>>();

    MIP(1,w,S_MaxPartition,1);

    S_Init = std::make_shared<std::deque<Crest*>>();
        
    calc_start();

    for(int i=w+1;i<=n;i=i+s){

        deletePartition(i-w+s-1);
        addRange(i,std::min(i+s-1,n));

        calc_start();
    }
    return 0;
}  