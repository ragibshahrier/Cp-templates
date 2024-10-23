#include<bits/stdc++.h>
// #include <ext/pb_ds/assoc_container.hpp> 
// #include <ext/pb_ds/tree_policy.hpp> 
using namespace std;
// #define ordered_set tree<int, null_type,less<int>, rb_tree_tag,tree_order_statistics_node_update> 
// using namespace __gnu_pbds; 

typedef long long ll;
 
#define vi vector<int>
#define vb vector<bool>
#define vvi vector<vi>
#define vl vector<ll>
#define vvl vector<vl>
#define vpi vector<pair<int,int>>

#define pi pair<int,int>
#define pll pair<ll,ll>

#define pb push_back
#define pf push_front

#define ff first
#define ss second
#define FOR(ii,st, n) for(int ii=st; ii<n; ii++)
#define rep(ii,st, n) for(int ii=st; ii<n; ii++)
#define gp " "


//#define DEBG

#define debug(n)
#define debugc(a)
#define debugcc(a)
#ifdef DEBG
#define debug(n) cout<<__LINE__<<gp<<#n<<gp<<n<<endl;
#define debugc(a) cout<<__LINE__<<gp<<#a<<gp<<'['<<gp;for(auto el:a){cout<<el<<gp;}cout<<']'<<endl;
#define debugcc(a) cout<<__LINE__<<gp<<#a<<gp<<'['<<gp;for(auto el:a){cout<<'{'<<gp<<el.ff<<','<<el.ss<<gp<<'}'<<gp;}cout<<']'<<endl;
#endif

#define fastcin() ios_base::sync_with_stdio(false); cin.tie(NULL);
#define endl '\n'


#define All(a) a.begin(),a.end()
template<typename T> void get_vector(T&a){for(auto&e:a)cin>>e;}
template<typename T> void put_vector(T a){for(auto e:a)cout<<e<<" ";cout<<endl;}


const ll INF = 2e18;
const ll inf = INT_MAX;
const ll M = 1e9 + 7;
const ll N = 2e5 + 7;
const ll modinvof2 = 5e8+4;

//==============================CODE STARTS HERE==============================//




void preprocessing(){

}

void solve(int testcase){
    

}

int main(){
    fastcin();

    int t=1;
    cin>>t;
    preprocessing();
    rep(i,1,t+1)solve(t);
    return 0;
}