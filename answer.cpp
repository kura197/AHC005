#include <bits/stdc++.h>

using namespace std;
using namespace chrono;

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
const double EPS = 1e-10;

const int SEED = 1000;

const double TIME = 2.85;
//const double TIME = 20;

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

mt19937 engine;

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

/// indicesにしたがって移動
string traverse(vector<ll>& indices){
    int sz = vers.size();
    string path;

    vector<ll> rem_edges(sz, 3);
    int v = 0;
    auto local_E2V = E2V;
    for(int i = 0; i < sz-1; i++){
        int nv = indices[i];
        if(rem_edges[nv] == 0) continue;
        auto nex_path = VP[v][nv];
            
        auto [y, x] = vers[v];
        for(auto& c : nex_path){                
            if(RE[y][x] != -1){
                for(auto& vv : local_E2V[RE[y][x]]){
                    rem_edges[vv] &= 0b01;
                }
                local_E2V[RE[y][x]].clear();
            }
            if(CE[y][x] != -1){
                for(auto& vv : local_E2V[CE[y][x]]){
                    rem_edges[vv] &= 0b10;
                }
                local_E2V[CE[y][x]].clear();
            }

            if(c == 'U') y--;
            else if(c == 'D') y++;
            else if(c == 'R') x++;
            else if(c == 'L') x--;
        }

        //auto [ny, nx] = vers[nv];
        //assert(ny == y);
        //assert(nx == x);

        v = nv;
        path += nex_path;
    }
    path += VP[v][0];
    return path;
}

/// xor128
unsigned int randxor()
{
    static unsigned int x=123456789, y=362436069, z=521288629, w=88675123;
    unsigned int t;
    t = (x^(x<<11));
    x = y;
    y = z;
    z = w; 
    return( w=(w^(w>>19))^(t^(t>>8)) );
}

/// return [0,1)
double rand01(){
    return (randxor() + 0.5) * (1.0 / UINT_MAX);
}

//// heuristic 2-opt, ...
string heuristic_path_delete(const double endTime, const auto startTime, const double startTemp, const double endTemp){
    int sz = vers.size();

    vector<ll> indices(sz-1);
    REP(i,sz-1) indices[i] = i+1;
    string ret = traverse(indices);
    ll cnt = 0;
    while(1){
        cnt++;
        const double time = duration_cast<microseconds>(system_clock::now() - startTime).count() * 1e-6;
        if(time > endTime)
            break;

        if(rand01() < 0.05){
            reverse(indices.begin(), indices.end());
            string path = traverse(indices);

            const double progressRatio = time / endTime;
            const double temp = startTemp + (endTemp - startTemp) * progressRatio;
            const double deltaScore = (int)ret.size() - (int)path.size() + EPS;
            const double probability = exp(deltaScore / temp);
            const bool force_next = probability > (double)(randxor() % 100000) / 100000;
            if(ret.size() > path.size() || force_next){
                ret = path;
            } else {
                reverse(indices.begin(), indices.end());
            }
        } else {
            //// 2-opt
            static uniform_int_distribution<> rand(0, sz-2);
            static uniform_int_distribution<> rand2(1, 5);
            const int CNT = rand2(engine);
            //const int CNT = 1;
            vector<int> idx0(CNT, -1), idx1(CNT, -1);
            for(int i = 0; i < CNT; i++){
                while(idx0[i] == idx1[i]){
                    idx0[i] = rand(engine);
                    idx1[i] = rand(engine);
                }
                swap(indices[idx0[i]], indices[idx1[i]]);
            }

            string path = traverse(indices);

            const double progressRatio = time / endTime;
            const double temp = startTemp + (endTemp - startTemp) * progressRatio;
            const double deltaScore = (int)ret.size() - (int)path.size() + EPS;
            const double probability = exp(deltaScore / temp);
            const bool force_next = probability > (double)(randxor() % 100000) / 100000;
            ///fprintf(stdout, "%.2f, %.2f, %.2f: %.3f, %.3f, %.3f\n", time, endTime, progressRatio, deltaScore, temp, probability);
            //fprintf(stdout, "%.2f: %.3f, %.3f\n", progressRatio, deltaScore, probability);

            if(ret.size() > path.size() || force_next){
                ret = path;
            } else {
                for(int i = CNT-1; i >= 0; i--){
                    swap(indices[idx0[i]], indices[idx1[i]]);
                }
            }
        }
    }
    fprintf(stderr, "Iteration: %lld\n", cnt);
    return ret;
}

int main(int argc, char* argv[]){
    double startTemp = 10.0;
    double endTemp = 1.0;
    if(argc == 3){
        startTemp = stoi(argv[1]);
        endTemp = stoi(argv[2]);
    }

    const auto startTime = system_clock::now();
    cin >> N >> si >> sj;
    REP(i,N){
        string s;
        cin >> s;
        C.push_back(s);
    }
    engine = mt19937(SEED);

    make_distance();
    allocate_edges();
    allocate_vers();
    make_e2v();
    calc_vers_dist();

    string answer = heuristic_path_delete(TIME, startTime, startTemp, endTemp);

    cout << answer << endl;
    return 0;
}
