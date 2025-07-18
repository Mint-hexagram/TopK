#include<vector>
#include<algorithm>
#include<iostream>
#include<chrono>
#include <fstream>

#define int long long
using namespace std;
typedef struct Node{
    double L; //端内最小前缀和
    double R; //段内最大前缀和
    double val; //最大解
    Node *lc = nullptr; //左子节点
    Node *rc = nullptr; //右子节点
    int idx_L; //L所在的索引
    
    int idx_R;
    int from; //本节点的开始
    int to;
    int val_from; //val所在范围的开始
    int val_to;
    bool is_hole = false; //是否存在hole
}node;
int n = 12,w,s,k=400; //n的大小

struct HeapNode{
    double value;
    int l,r;
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





vector<double> input; // 输入
vector<double> prefix; //前缀
//初始化的过程
void init() {
    //n = 12;
    input.resize(n);
    prefix.resize(n);
    int tmp[] = {3,51,-41,-57,52,59,-11,93,-55,-71,21,21};
    for (int i = 0;i<n;i++) {
        input[i] = tmp[i];
        if(i == 0) prefix[i] = input[i];
        else prefix[i] = prefix[i-1] + input[i];
    }
}
void init2() {
    n = 10000000;
    input.resize(n);
    prefix.resize(n);
    for(int i = 0;i<n;i++) {
        input[i] = (-100+(rand()%200));
        if(i == 0) prefix[i] = input[i];
        else prefix[i] = prefix[i-1] + input[i];
    }
}



node* build(int l,int r) {
    //叶子节点初始化
    if(l == r) {
        //node tmp = {l,r,input[l],nullptr,nullptr};
        node *tmp = new node;
        tmp->L = prefix[l] - input[l];
        tmp->R = prefix[l];
        tmp->val = input[l];
        tmp->from = l;
        tmp->to = r;
        tmp->val_from = l;
        tmp->val_to = r;
        tmp->idx_L = l;
        tmp->idx_R = r;
        return tmp;
    }
    //递归建树
    int mid = (l + r - 1) / 2;
    node *tmp1 = build(l,mid);
    node *tmp2 = build(mid + 1,r);
    //确定L和R
    node* newNode = new node;
    if (tmp1->L >= tmp2->L) {
        newNode->L = tmp2->L;
        newNode->idx_L = tmp2->idx_L;
    }else {
        newNode->L = tmp1->L;
        newNode->idx_L = tmp1->idx_L;
    }
    if (tmp1->R >= tmp2->R) {
        newNode->R = tmp1->R;
        newNode->idx_R = tmp1->idx_R;
    }else {
        newNode->R = tmp2->R;
        newNode->idx_R = tmp2->idx_R;
    }
    //确定val
    if (tmp1->val>tmp2->val||(tmp1->val == tmp2->val && tmp1->val_to - tmp1->val_from < tmp2->val_to - tmp2->val_from)) {
        newNode->val = tmp1->val;
        newNode->val_from = tmp1->val_from;
        newNode->val_to = tmp1->val_to;
    }else {
        newNode->val = tmp2->val;
        newNode->val_from = tmp2->val_from;
        newNode->val_to = tmp2->val_to;
    }
    if (tmp2->R-tmp1->L>newNode->val||
        (tmp2->R-tmp1->L == newNode->val && 
            (tmp2->idx_R - tmp1->idx_L < newNode->val_to - newNode->val_from||
                (tmp2->idx_R - tmp1->idx_L == newNode->val_to - newNode->val_from&&tmp1->idx_L > newNode->val_from)))) {
        newNode->val = tmp2->R-tmp1->L;
        newNode->val_from = tmp1->idx_L;
        newNode->val_to = tmp2->idx_R;
    }
    //确定其他变量值
    newNode->lc = tmp1;
    newNode->rc = tmp2;
    newNode->from = l;
    newNode->to = r;

    //cout<<l<<' '<<r<<' '<<newNode->val<<endl;
    return newNode;
}
void update(node *n,int hole_l,int hole_r) {
    //如果这段和洞毫无重叠
    if(n->from>hole_r||n->to<hole_l) return;
    //如果完全被覆盖
    if(n->from>=hole_l&&n->to<=hole_r) {
        n->is_hole = true;
        n->val = INT_MIN;
        n->L = INT_MAX;
        n->R = INT_MIN;
        return;
    }
    //如果是一个已经被别的洞完全覆盖的节点
    if(n->is_hole == true&&n->val == INT_MIN&&n->L == INT_MAX&&n->R == INT_MIN) return;
    //更新子节点
    update(n->lc,hole_l,hole_r);
    update(n->rc,hole_l,hole_r);
    //反过来更新父节点 这里是确定新的L和R的值
    if(n->rc->is_hole) n->L = n->rc->L,n->idx_L = n->rc->idx_L;
    else {
        //n->L = min(n->rc->L,n->lc->L);
        if(n->lc->L >= n->rc->L) {
            n->L = n->rc->L;
            n->idx_L = n->rc->idx_L;
        }else {
            n->L = n->lc->L;
            n->idx_L = n->lc->idx_L;
        }
    }
    if(n->lc->is_hole) n->R = n->lc->R,n->idx_R = n->lc->idx_R;
    else {
        //n->R = max(n->rc->R,n->lc->R);
        if(n->lc->R >= n->rc->R) {
            n->R = n->lc->R;
            n->idx_R = n->lc->idx_R;
        }else {
            n->R = n->rc->R;
            n->idx_R = n->rc->idx_R;
        }
    }
    //确定是否有洞
    if(n->lc->is_hole||n->rc->is_hole) n->is_hole = true;
    //确定新的val值 和val_from val_to
    node *tmp1 = n->lc;
    node *tmp2 = n->rc;
    if (tmp1->val>tmp2->val||(tmp1->val == tmp2->val && tmp1->val_to - tmp1->val_from < tmp2->val_to - tmp2->val_from)) {
        n->val = tmp1->val;
        n->val_from = tmp1->val_from;
        n->val_to = tmp1->val_to;
    }else {
        n->val = tmp2->val;
        n->val_from = tmp2->val_from;
        n->val_to = tmp2->val_to;
    }
    if (tmp2->R-tmp1->L>n->val||
        (tmp2->R-tmp1->L == n->val && 
            (tmp2->idx_R - tmp1->idx_L < n->val_to - n->val_from||
                (tmp2->idx_R - tmp1->idx_L == n->val_to - n->val_from&&tmp1->idx_L > n->val_from)))) {
        n->val = tmp2->R-tmp1->L;
        n->val_from = tmp1->idx_L;
        n->val_to = tmp2->idx_R;
    }
    //cout<<n->from<<' '<<n->to<<' '<<n->idx_L<<' '<<n->idx_R<<' '<<n->L<<' '<<n->R<<' '<<n->val<<endl;
}
//run!!!!!
void run() { 
    clock_t start = clock();
    node *root = build(0,n-1);
    clock_t end_1 = clock();
    //cout<<root->val;

    while (root->val>0) {
//        cout<<root->val_from<<' '<<root->val_to<<' '<<root->val<<endl;
        Max_TopKHead->cal_topk((HeapNode){root->val,root->val_from,root->val_to});
        update(root,root->val_from ,root->val_to);
        //cout<<root->val_from<<' '<<root->val_to<<' '<<root->val<<endl;
    }
    clock_t end_2 = clock();
    //cout<<"build time:"<<end_1 - start<<"ms run_time:"<<end_2-end_1<<"ms"<<endl;
}
void py_input(){
    cin>>n;
    input.resize(n);
    prefix.resize(n);

    for(int i = 0;i<n;i++){
        cin>>input[i];
        if(i == 0) prefix[i] = input[i];
        else prefix[i] = prefix[i-1] + input[i];
    }
}
void DataInit_input(){ //李浩民专属输入
//    n = 1000;
//    w = 1000;
//    s = 1;

    vector<double> a(n+1);
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
        //clock_t start = clock();
        node *root = build(LL,RR);
        //clock_t end = clock();
        while (root->val>0) {
            cout<<root->val_from<<' '<<root->val_to<<' '<<root->val<<endl;
            update(root,root->val_from ,root->val_to);
            //cout<<root->val_from<<' '<<root->val_to<<' '<<root->val<<endl;
        }
        LL += s;
        RR += s;
        Max_TopKHead->siz = 0;
        if(RR >= n - 2){
            RR = n - 2;
            node *root = build(LL,RR);
            while (root->val>0) {
//                cout<<root->val_from<<' '<<root->val_to<<' '<<root->val<<endl;
                Max_TopKHead->cal_topk((HeapNode){root->val,root->val_from,root->val_to});
                update(root,root->val_from ,root->val_to);
                //cout<<root->val_from<<' '<<root->val_to<<' '<<root->val<<endl;
            }
            break;
        }
        break;
    }
    //这里while是输出全部值大于0的输出，当然你也可以改成for循环找topk
    

}

void A_High_DataInit_input(){
    freopen("A_total.txt","r",stdin);
}

void B_High_DataInit_input(){
    freopen("B_total.txt","r",stdin);
}

void C_High_DataInit_input(){
    freopen("C_total.txt","r",stdin);
}

void D_High_DataInit_input(){
    freopen("D_total.txt","r",stdin);
}

void E_High_DataInit_input(){
    freopen("E_total.txt","r",stdin);
}
void F_High_DataInit_input(){
    freopen("input_new.txt","r",stdin);
}


void High_DataInit_input(){
    freopen("high_input.txt","r",stdin);
}

void normal_DataInit_input(){
    freopen("normal_input.txt","r",stdin);
}

void uniform_DataInit_input(){
    freopen("uniform_input.txt","r",stdin);
}

void trip_DataInit_input(){
    freopen("trip_counts_sequence.txt","r",stdin);
}



signed main()
{
        n=2000000;
        w=600000;
        s=50;
        k=20;

    C_High_DataInit_input();

    DataInit_input();
    freopen("OOOOOOFFFFFF.txt","a",stdout);

    auto start = std::chrono::high_resolution_clock::now();

    run2();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;


    cout << duration.count() << std::endl;


    
    return 0;
}
