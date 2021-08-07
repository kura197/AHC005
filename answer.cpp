#include <bits/stdc++.h>

using namespace std;

typedef long long ll;
typedef unsigned long long ull;
#define REP(i, n) for(int i=0; i<n; i++)
#define REPi(i, a, b) for(int i=int(a); i<int(b); i++)
#define MEMS(a,b) memset(a,b,sizeof(a))
#define mp make_pair
#define MOD(a, m) ((a % m + m) % m)
template<class T>bool chmax(T &a, const T &b) { if (a<b) { a=b; return 1; } return 0; }
template<class T>bool chmin(T &a, const T &b) { if (b<a) { a=b; return 1; } return 0; }
const ll MOD = 1e9+7;
const ll INF = 1e15;

int N, si, sj;
vector<string> C;

using Pos = pair<int, int>;

/// distance for vers
vector<vector<ll>> D;

/// col / row edge idx
vector<vector<ll>> CE;
vector<vector<ll>> RE;

/// edge idx to vers
vector<vector<ll>> E2V;

/// vertices
vector<Pos> vers;

/// dist between vertices
vector<vector<ll>> VD;

/// path between vertices
vector<vector<string>> VP;

void make_distance(){
    D.resize(N);
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(C[i][j] == '#')
                D[i].push_back(INF);
            else
                D[i].push_back(C[i][j] - '0');
        }
    }

    assert((int)D.size() == N);
    assert((int)D[0].size() == N);
    assert((int)D[N-1].size() == N);
}

//// 全ての頂点に辺番号をふる
void allocate_edges(){
    CE.resize(N);
    RE.resize(N);
    REP(i,N){
        CE[i].resize(N, -1);
        RE[i].resize(N, -1);
    }

    int cnt = 0;
    int cur_edge = -1;
    /// row
    for(int i = 0; i < N; i+=2){
        for(int j = 0; j < N; j++){
            if(cur_edge != -1){
                if(D[i][j] == INF){
                    cur_edge = -1;
                    cnt++;
                }
            } else {
                if(j+1 < N && D[i][j] != INF && D[i][j+1] != INF){
                    cur_edge = cnt; 
                }
            }

            RE[i][j] = cur_edge;
        }

        if(cur_edge != -1){
            cur_edge = -1;
            cnt++;
        }
    }

    /// col
    for(int j = 0; j < N; j+=2){
        for(int i = 0; i < N; i++){
            if(cur_edge != -1){
                if(D[i][j] == INF){
                    cur_edge = -1;
                    cnt++;
                }
            } else {
                if(i+1 < N && D[i][j] != INF && D[i+1][j] != INF){
                    cur_edge = cnt; 
                }
            }

            CE[i][j] = cur_edge;
        }

        if(cur_edge != -1){
            cur_edge = -1;
            cnt++;
        }
    }
}

//// 頂点番号
void allocate_vers(){
    vers.push_back(Pos(si, sj));
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(RE[i][j] != -1 && CE[i][j] != -1)
                vers.push_back(Pos(i, j));
        }
    }
}

/// E2V 配列を作成
void make_e2v(){
    E2V.resize(vers.size());
    for(size_t v = 0; v < vers.size(); v++){
        auto [y, x] = vers[v];
        auto re = RE[y][x];
        auto ce = CE[y][x];
        if(re != -1)
            E2V[re].push_back(v);
        if(ce != -1)
            E2V[ce].push_back(v);
    }
}

void dijkstra(ll y, ll x, vector<vector<ll>>& dist){
    dist[y][x] = 0;
    using T = tuple<ll, ll, ll>;
    priority_queue<T, vector<T>, greater<T>> que;
    que.push(T(0, y, x));
    while(!que.empty()){
        auto [d, y, x] = que.top();
        que.pop();
        if(dist[y][x] < d) continue;
        const int dx[] = {-1, 1, 0, 0};
        const int dy[] = {0, 0, 1, -1};
        REP(i,4){
            auto ny = y + dy[i];
            auto nx = x + dx[i];
            if(ny < 0 || nx < 0) continue;
            if(ny >= N || nx >= N) continue;
            if(D[ny][nx] == INF) continue;
            auto nd = d + D[ny][nx];

            if(nd < dist[ny][nx]){
                dist[ny][nx] = nd;
                que.push(T(nd, ny, nx));
            }
        }
    }
}

//// (y, x) へ至るパスを復元
string restore_path(ll y, ll x, vector<vector<ll>>& dist){
    string path; 
    while(dist[y][x] != 0){
        /// left, right, down, up
        const int dx[] = {-1, 1, 0, 0};
        const int dy[] = {0, 0, 1, -1};
        //const char dir[] = {'R', 'L', 'D', 'U'};
        const char dir[] = {'R', 'L', 'U', 'D'};
        REP(i,4){
            auto ny = y + dy[i];
            auto nx = x + dx[i];
            if(ny < 0 || nx < 0) continue;
            if(ny >= N || nx >= N) continue;
            if(D[ny][nx] == INF) continue;
            if(dist[ny][nx] == INF) continue;

            auto nd = dist[y][x] - D[y][x];
            if(nd == dist[ny][nx]){
                y = ny, x = nx;
                path.push_back(dir[i]);
                break;
            }
        }
    }

    reverse(path.begin(), path.end());
    return path;
}

/// 頂点間距離を計算
void calc_vers_dist(){
    VD.resize(vers.size());
    VP.resize(vers.size());
    for(size_t i = 0; i < vers.size(); i++){
        auto [y, x] = vers[i];
        vector<vector<ll>> dist(N, vector<ll>(N, INF));
        dijkstra(y, x, dist);

        VD[i].resize(vers.size());
        VP[i].resize(vers.size());
        for(size_t j = 0; j < vers.size(); j++){
            auto [ny, nx] = vers[j];
            VD[i][j] = dist[ny][nx];
            auto path = restore_path(ny, nx, dist);
            VP[i][j] = path;
        }
    }
}

string naive_path(){
    int sz = vers.size();
    string path;
    for(int nv = 1; nv < sz; nv++){
        int v = nv-1;
        //printf("%d: (%lld, %lld)\n", nv, vers[nv].first, vers[nv].second);
        path += VP[v][nv];
    }
    path += VP[sz-1][0];
    return path;
}

//// 通るひつようのない頂点を消しながら移動
string naive_path_delete(){
    int sz = vers.size();
    string path;

    vector<ll> rem_edges(sz, 2);
    int v = 0;
    for(int nv = 1; nv < sz; nv++){
        if(rem_edges[nv] == 0) continue;
        path += VP[v][nv];

        auto [ny, nx] = vers[nv];
        for(auto& vv : E2V[RE[ny][nx]]){
            rem_edges[vv]--;
        }
        for(auto& vv : E2V[CE[ny][nx]]){
            rem_edges[vv]--;
        }

        E2V[RE[ny][nx]].clear();
        E2V[CE[ny][nx]].clear();

        v = nv;
    }
    path += VP[v][0];
    return path;
}

int main(){
    cin >> N >> si >> sj;
    REP(i,N){
        string s;
        cin >> s;
        C.push_back(s);
    }

    make_distance();
    allocate_edges();
    allocate_vers();
    make_e2v();
    //printf("Ver: %d\n", vers.size());
    calc_vers_dist();

    string answer = naive_path_delete();

    cout << answer << endl;
    return 0;
}
