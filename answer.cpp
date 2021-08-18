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
//const double TIME = 30;

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

/// num of edges
ll n_edges;

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

    n_edges = cnt;
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
        int idx = 0;
        for(auto& c : nex_path){                
            if(idx % 2 == 0){
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
            }

            if(c == 'U') y--;
            else if(c == 'D') y++;
            else if(c == 'R') x++;
            else if(c == 'L') x--;

            //idx++;
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

/// return [left, right)
int myrand(const int left, const int right){
    static uniform_int_distribution<> rand(0, INT_MAX);
    int ret = rand(engine);
    assert(right - left > 0);
    ret %= right - left;
    ret += left;
    return ret;
}

string get_path(vector<ll>& indices){
    string path;
    int v = 0;
    for(auto& nv : indices){
        auto nex_path = VP[v][nv];
        path += nex_path;
        v = nv;
    }
    path += VP[v][0];
    return path;
}

ll calc_score(vector<ll>& indices){
    ll score = 0;
    int v = 0;
    for(auto& nv : indices){
        score += VD[v][nv];
        v = nv;
    }
    score += VD[v][0];
    return score;
}

unordered_set<int> calc_ncross(vector<ll>& indices){
    unordered_set<int> crossed;
    int v = 0;
    for(auto& nv : indices){
        auto [y, x] = vers[v];
        //assert(CE[y][x] != -1);
        //assert(RE[y][x] != -1);
        //if(CE[y][x] == -1 || RE[y][x] == -1){
        //    printf("Error\n");
        //}
        if(RE[y][x] != -1)
            for (auto &vv : E2V[RE[y][x]]){
                crossed.insert(vv);
            }
        if(CE[y][x] != -1)
            for (auto &vv : E2V[CE[y][x]]){
                crossed.insert(vv);
            }
        v = nv;
    }
    return crossed;
}

unordered_set<int> calc_edges(vector<ll>& indices){
    unordered_set<int> edges;
    int v = 0;
    for(auto& nv : indices){
        auto [y, x] = vers[v];
        if(RE[y][x] != -1)
            edges.insert(RE[y][x]);
        if(CE[y][x] != -1)
            edges.insert(CE[y][x]);
        v = nv;
    }
    return edges;
}

//// heuristic 2-opt, ...
string heuristic_path_delete(const double endTime, const auto startTime, const double startTemp, const double endTemp){
    int ncross = vers.size();

    //// [1, ..., ncross-1]
    vector<ll> indices((ncross-1)/2);
    REP(i,(ncross-1)/2) indices[i] = i+1;
    shuffle(indices.begin(), indices.end(), engine);
    //vector<ll> indices{1, 2, 3};
    auto min_indices = indices;

    const double penalty = 5000.0;

    auto edges = calc_edges(indices);
    double min_score = calc_score(indices) + (n_edges - edges.size()) * penalty;
    double cur_score = min_score;
    ll cnt = 0;
    while(1){
        cnt++;
        const double time = duration_cast<microseconds>(system_clock::now() - startTime).count() * 1e-6;
        if(time > endTime)
            break;

        function restore = [](){};
        double score;

        //fprintf(stderr, "%d: %d\n", cnt, indices.size());
        auto prob = rand01();
        if (prob < 0.25) {
            /// insert
            int idx1 = myrand(1, ncross);
            int idx2 = myrand(0, indices.size());

            ///TODO
            bool contain = false;
            for(auto& x : indices){
                if(idx1 == x) {
                    contain = true;
                    break;
                }
            }
            if(contain) continue;

            //fprintf(stderr, "insert %d, size=%d\n", idx, indices.size());

            indices.insert(indices.begin() + idx2, idx1);
            restore = [&indices, idx2]() { indices.erase(indices.begin() + idx2); };

            edges = calc_edges(indices);
            score = calc_score(indices) + (n_edges - edges.size()) * penalty;
        } else if(prob < 0.5){
            /// remove
            int idx = myrand(0, indices.size());

            //fprintf(stderr, "remove %d, size=%d\n", idx, indices.size());

            auto pos = indices[idx];
            indices.erase(indices.begin() + idx);
            restore = [&indices, idx, pos]() { indices.insert(indices.begin() + idx, pos); };

            edges = calc_edges(indices);
            //score = calc_score(indices) + (ncross - crossed.size()) * 10.0;
            score = calc_score(indices) + (n_edges - edges.size()) * penalty;
        } else if(prob < 0.75){
            /// replace
            int idx1 = myrand(1, ncross);
            int idx2 = myrand(0, indices.size());

            ///TODO
            bool contain = false;
            for(auto& x : indices){
                if(idx1 == x) {
                    contain = true;
                    break;
                }
            }
            if(contain) continue;

            auto pos = indices[idx2];
            indices[idx2] = idx1;
            restore = [&indices, idx2, pos]() { indices[idx2] = pos; };

            edges = calc_edges(indices);
            score = calc_score(indices) + (n_edges - edges.size()) * penalty;
        } else {
            /// 2-swap
            int idx0 = myrand(0, indices.size());
            int idx1 = myrand(0, indices.size());
            if (idx0 == idx1) continue;
            if (idx0 > idx1) swap(idx0, idx1);

            int p0, pre_p0, post_p0;
            int p1, pre_p1, post_p1;

            score = cur_score;
            if (idx0 == 0) {
                p0 = indices[idx0];
                pre_p0 = 0;
                post_p0 = indices[idx0 + 1];
            } else {
                p0 = indices[idx0];
                pre_p0 = indices[idx0 - 1];
                post_p0 = indices[idx0 + 1];
            }

            if (idx1 == (int)indices.size() - 1) {
                p1 = indices[idx1];
                pre_p1 = indices[idx1 - 1];
                post_p1 = 0;
            } else {
                p1 = indices[idx1];
                pre_p1 = indices[idx1 - 1];
                post_p1 = indices[idx1 + 1];
            }

            // TODO
            // score -= VD[pre_p0][p0] + VD[p0][post_p0];
            // score -= VD[pre_p1][p1] + VD[p1][post_p1];
            // score += VD[pre_p0][p1] + VD[p1][post_p0];
            // score += VD[pre_p1][p0] + VD[p0][post_p1];

            reverse(indices.begin() + idx0, indices.begin() + idx1);
            restore = [&indices, idx0, idx1]() { reverse(indices.begin() + idx0, indices.begin() + idx1); };

            //edges = calc_edges(indices);
            score = calc_score(indices) + (n_edges - edges.size()) * penalty;
        }

        //fprintf(stderr, "score=%f (%lld, %f)\n", score, calc_score(indices), (n_edges - edges.size()) * penalty);

        const double progressRatio = time / endTime;
        const double temp = startTemp + (endTemp - startTemp) * progressRatio;
        const double deltaScore = -1.0 * (score - cur_score) + EPS;
        const double probability = exp(deltaScore / temp);
        const bool force_next = probability > (double)(randxor() % 100000) / 100000;
        ///fprintf(stdout, "%.2f, %.2f, %.2f: %.3f, %.3f, %.3f\n", time, endTime, progressRatio, deltaScore, temp, probability);
        //if(cnt % 5000 == 0)
        //    fprintf(stderr, "%.2f: %.3f, %.3f\n", progressRatio, deltaScore, probability);

        if(cur_score > score || force_next){
            cur_score = score;
            //tmp = path;
            if(min_score > cur_score){
                min_score = cur_score;
                min_indices = indices;
                fprintf(stderr, "%.2f: %.1f, %lld\n", progressRatio, min_score, (n_edges - edges.size()));
            }
        } else {
            restore();
        }
    }

    fprintf(stderr, "Iteration: %lld\n", cnt);
    //auto test = get_path(indices);
    //while((int)indices.size() > 0){
    //    auto t = calc_ncross(indices);
    //    fprintf(stderr, "%d: %d\n", (int)indices.size(), (int)t.size());
    //    indices.pop_back();
    //}
    //return test;
    return get_path(min_indices);
}

int main(int argc, char* argv[]){
    //double startTemp = 5.996794213525072;
    //double endTemp = 1.4780189300526465;
    //double startTemp = 100;
    double startTemp = 100;
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

    const int NSTART = 1;
    string answer(10000, 'U');
    for(int i = 0; i < NSTART; i++){
        string tmp = heuristic_path_delete(TIME * (i+1) / NSTART, startTime, startTemp, endTemp);
        if(tmp.size() < answer.size())
            answer = tmp;
    }

    cout << answer << endl;
    return 0;
}
