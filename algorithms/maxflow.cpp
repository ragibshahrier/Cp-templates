#include<bits/stdc++.h>
// #include <ext/pb_ds/assoc_container.hpp>
// #include <ext/pb_ds/tree_policy.hpp> 
using namespace std;
// #define ordered_set tree<int, null_type,less<int>, rb_tree_tag,tree_order_statistics_node_update>
// using namespace __gnu_pbds;
 
typedef long long ll;
 
#define int long long
 
#define vi vector<int>
#define vvi vector<vi>
#define vll vector<ll>
#define vvll vector<vll>
#define pi pair<int,int>
#define pll pair<ll,ll>
#define ff first
#define ss second
#define vpi vector<pair<int,int>>
#define rep(ii,st, n) for(int ii=st; ii<n; ii++)
#define gp " "

// #define DEBG

#define debug(n)
#define debugc(a)
#define debugcc(a)

#define fastcin() ios_base::sync_with_stdio(false); cin.tie(NULL);
#define endl '\n'


#define All(a) a.begin(),a.end()
template<typename T> void get_vector(T&a){for(auto&e:a)cin>>e;}
template<typename T> void put_vector(T a){for(auto e:a)cout<<e<<" ";cout<<endl;}


const ll INF = 2e18;
const ll inf = INT_MAX;
const ll M = 1e9 + 7;
const ll N = 2e5 + 7;
const ll modinvof2 = 500000004;



vvi dayroutes;

int bfs(int n, int s, int t, vpi& par, vector<vpi>& adj, vector<vvi>& capacity){

    queue<pair<int,int>>q;

    q.push({s, INF});
    
    vi visited(n);

    debugcc(par)

    while(!q.empty()){
        auto x = q.front();
        q.pop();

        
        int nd = x.ff;
        
        if(visited[nd])continue;
        visited[nd] = 1;

        int flow = x.ss;

        if(nd==t){
            return flow;
        }
        for(auto ell:adj[nd]){
            int el = ell.ff;
            int pth = ell.ss;

            debug(nd)
            if(!visited[el]){
                if(par[el].ff==-1 && capacity[nd][el][pth]>0){
                    debug(el)
                    int flow2 = min(capacity[nd][el][pth], flow);
                    par[el] = {nd,pth};
                    q.push({el, flow2});
                }
            }
        }
    }

    return 0;
}



int maxflow(int n, int m, vector<vvi>& capacity, vector<vpi> adj, int s, int t){
    vpi par(n,{-1,0});

    int maxflow = 0;
    int flow;
    while(flow = bfs(n, s,t,par, adj, capacity)){

        int cur = t;
        debug(flow)

        debugcc(par)

        
        while(cur!=s){
            
            int pr = par[cur].ff;
            int pth = par[cur].ss;
            capacity[pr][cur][pth] -= flow;
            capacity[cur][pr][1-pth] += flow;
            
            cur = par[cur].ff;
        }
        debug(maxflow)
        
        maxflow += flow;
        
        par.assign(n, {-1,0});
    }
    
    debugcc(par)
    
    return maxflow;

}

void routr(int n, int m, int s, int t, vector<vvi>& capacity, vector<vpi>& adj){
    while(true){
        vi path;

        queue<pi>q;
        q.push({s,-1});
        vi visited(n);
        vi par(n);
        int f =0;
        while(!q.empty()){
            auto x = q.front();
            q.pop();

            int nd = x.ff;
            int pr = x.ss;

            if(visited[nd])continue;

            visited[nd] = 1;
            

            if(nd!=s){
                par[nd] = pr;
            }

            if(nd==t){
                f = 1;
                break;
            }

            for(auto el:adj[nd]){
                debug(el.ss)
                debug(capacity[el.ff][nd][1])
                debug(nd)
                debug(el.ff)
                if(capacity[el.ff][nd][1]>0){
                    q.push(make_pair(el.ff,nd));
                }
            }

        }   
        if(!f)break;
        int cur = t;

        while(cur!=s){
            path.push_back(cur);
            capacity[cur][par[cur]][1]--;
            cur = par[cur];
        }
        path.push_back(cur);

        reverse(All(path));

        dayroutes.push_back(path);
    }
}

void solve(int testcases){
    int n,m;
    cin>>n>>m;

    vector<vpi>adj(n+1);

    vector<vvi> capacity(n+1, vvi(n+1, vi(2)));

    rep(i,0,m){
        int x,y;
        cin>>x>>y;

        x--;y--;

        adj[x].push_back({y,0});

        adj[y].push_back({x,1});

        capacity[x][y][0] = 1;
        capacity[y][x][0] = 1;
    }

    int s = 0;
    int t = n-1;
    int mxflow = maxflow(n, m, capacity, adj, s, t);


    routr(n, m, s, t, capacity, adj);

    cout<<dayroutes.size()<<endl;

    for(auto& el:dayroutes){
        cout<<el.size()<<endl;
        for(auto ell:el){
            cout<<ell+1<<" ";
        }
        cout<<endl;
        
    }
}

int32_t main(){
    fastcin();

    int t=1;
    // cin>>t;
    rep(i,1,t+1){
       solve(i);
    }
    return 0;
}
