#include<bits/stdc++.h>
 
using namespace std;
 
 
vector<bool>mark;
const long long inf = 1e18;

struct Node{
    long long  at;
    long long cost;
};

bool operator<(Node A, Node B){  
    return A.cost>B.cost;  // The node with higher cost is defined as smaller
}

struct edge{
    long long at;
    long long cost;
};

vector<long long> distance(vector<vector<edge>>G,int source){


    priority_queue<Node>Q;
    vector<long long>distance(G.size(),inf);
 
    Q.push({source,0});
    distance[source] = 0;

    while(!Q.empty()){
        Node u = Q.top();
        Q.pop();

        if(mark[u.at])continue;
        mark[u.at] = 1;

        for(auto v:G[u.at]){
            if(distance[v.at]>distance[u.at]+v.cost){
                distance[v.at] = distance[u.at]+v.cost;
                Q.push({v.at,distance[v.at]});
            }
        }

    }
    return distance;
}
 
int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);


    int n;
    cin>>n;
    mark.resize(n);
    vector<vector<edge>>graph(n);

    int q;
    cin>>q;
    
    while(q--){
        long long u,v,w;
        cin>>u>>v>>w;
        graph[u-1].push_back({v-1,w});
    }

    vector<long long>res = distance(graph,0);
    for(int i = 0; i<n; i++)cout << res[i]<< ' ';
    cout << endl;
 
 
}