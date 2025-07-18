#include <bits/stdc++.h>
using namespace std;
const int maxn=1e7+1000;
struct Subsegment
{
    int l,r;
    double val;
}ans[maxn];
class Crest{
public:
    int L,R;
    Crest(int l,int r):L(l),R(r){};
};
double sum[maxn],a[maxn];
int n,w,s,k;
void DataInit_input(){
    freopen("input.txt","r",stdin);
    freopen("Basic_Moving.txt","w",stdout);
    for(int i=1;i<=n;++i){
        scanf("%lf",&a[i]);
    }
}

void getSubSet(int L,int R,std::shared_ptr<std::deque<Crest*>> S_tmp){
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
    if(now->L!=-1 && now->R==-1){
        //说明，直到该窗口结束，该段仍未结束递增过程
        //因此，窗口的右端点成为最后一个递增子段的波峰
        now->R = R;
        S_tmp->push_back(now);
    }

}

void MSGA(int L,int R,std::shared_ptr<std::deque<Crest*>> S_Max){
    std::shared_ptr<std::deque<Crest*>> S_tmp = std::make_shared<std::deque<Crest*>>();
    getSubSet(L,R,S_tmp);
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
        S_Max->push_front(now);
    }
};

bool cmp(Subsegment a,Subsegment b)
{
    return a.val > b.val;
}
std::shared_ptr<std::deque<Crest*>> S_Max;

void get_ans(){
    int cnt_ans=0;
    for(int i=0;i<S_Max->size();++i){
        ans[++cnt_ans].l = S_Max->at(i)->L;
        ans[cnt_ans].r = S_Max->at(i)->R;
        ans[cnt_ans].val = a[ans[cnt_ans].r] - a[ans[cnt_ans].l];
    }
    sort(ans+1,ans+cnt_ans+1,cmp);
    for(int i=1;i<=min(cnt_ans,k);++i){
        printf("val=%lf\n",ans[i].val);
    }
    printf("cnt_ans=%d\n",min(cnt_ans,k));
}

int main(int argc, char* argv[]) {
    if(argc<2){
        n=20;
        w=10;
        s=1;
        k=10;
    }
    else{
        n=std::atoi(argv[1]);
        w=std::atoi(argv[2]);
        s=std::atoi(argv[3]);
        k=std::atoi(argv[4]);
    }

    S_Max = std::make_shared<std::deque<Crest*>>();
    DataInit_input();
    for(int i=1;i+w-1<=n+s-1;i+=s){
        S_Max->clear();
        MSGA(i,min(i+w-1,n),S_Max);
        get_ans();
    }
    
    
    return 0;
}
