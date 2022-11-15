// 动态开点
const int N = 3000010; // N = log(10 ^ 9) *  10 ^ 5 = 3 * 10^ 6
int root, tot = 1;
struct Node {
    int lson, rson;
    // TODO: 需要维护的信息和懒标记
    int v, lazy;
    Node():lson(0), rson(0), v(0), lazy(0){}
}tr[N];

void pushup(Node& u, Node& l, Node& r){
    // TODO: 利用左右儿子信息维护当前节点的信息

}

void pushup(int u){
    pushup(tr[u], tr[tr[u].lson], tr[tr[u].rson]);
}

void pushdown(int u, int l, int r, int m) {
    // TODO: 将懒标记下传
    // if(tr[u].lazy == *) return;
    if(! tr[u].lson) tr[u].lson = ++ tot;  // !!!
    if(! tr[u].rson) tr[u].rson = ++ tot;
    auto &root = tr[u], &left = tr[tr[u].lson], &right = tr[tr[u].rson];
    // 置空
    if(tr[u].lazy){
        // left: m - l + 1 , right : r - m

    }
}

void update(int& u, int l, int r, int L, int R, int val) { 
    if(r < L || l > R) return ;
    else if (l >= L && r <= R) {   
        // TODO: 修改区间 并 处理懒标记

    }
    else {
        int mid = l + r >> 1;
        pushdown(u, l, r, mid);
        update(tr[u].lson, l, mid, L, R, val);
        update(tr[u].rson, mid + 1, r, L, R, val);
        pushup(u);
    }
}

Node query(int u, int l, int r, int L, int R){
    if(r < L || l > R) return Node();
    else if(l >= L && r <= R) {
        return tr[u]; 
    }
    else{
        int mid = l + r >> 1;
        pushdown(u, l, r, mid);

        if(r <= mid) return query(tr[u].lson, l, mid, L, R);
        else if(l > mid) return query(tr[u].rson, mid + 1, r, L, R);
        else {
            Node res;
            auto left = query(tr[u].lson, l, mid, L, R);
            auto right = query(tr[u].rson, mid + 1, r, L, R);
            pushup(res, left, right);
            return res;
        }
    }
}

// 
root = 1;
memset(tr, 0, sizeof tr);
update(root, 1, 1e9, L, R, 1);
query(root, 1, 1e9, L, R);


/**
 * Your CountIntervals object will be instantiated and called as such:
 * CountIntervals* obj = new CountIntervals();
 * obj->add(left,right);
 * int param_2 = obj->count();
 */