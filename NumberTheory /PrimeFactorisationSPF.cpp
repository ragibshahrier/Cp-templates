#include<bits/stdc++.h>
 
 using namespace std;
 
typedef long long ll;
typedef long double ld;
typedef vector<int> vi;
typedef vector<ll> vl;
typedef vector<double> vd;
#define All(a) a.begin(),a.end()
#define FOR(i,a,b) for(int i=a;i<b;i++)
template<typename T> void get_vector(T&a){for(auto&e:a)cin>>e;}
template<typename T> void put_vector(T a){for(auto e:a)cout<<e<<" ";cout<<endl;}

const int MAX = 1e7+2;

vector<int> primes;
int spf[MAX + 1];  //SPF = Smallest prime factor

//sieve variant of storing smallest prime factor as wess
void sieve(){
    for(int i = 2; i<MAX; i+=2)spf[i] = 2;
    for (int i = 3; i <= MAX; ++i) {
        if(spf[i] == 0){
            //primes.push_back(i); // if necessary
            spf[i] = i;
            for(ll j = 1LL*i*i; j<=MAX; j+=i*2){
                if(spf[j]==0)spf[j] = i;
            }
        }
    }
}


//prime factorisation in O(log(n)) by precalculating SPF
//works when n<=1e7

void prime_factorisation(int x){
    map<int,int>divs;
    while(x>1){
        int y = spf[x];
        while(x%y==0){
            x/=y;
            divs[y]++;
        }
    }
    for(auto e:divs)cout << e.first << " " << e.second << endl;
}



int main(){

    sieve();    
    
    
    int t;
    cin>>t;
    while(t--){
        int x;
        cin>>x;
        prime_factorisation(x);
    }
    
}
