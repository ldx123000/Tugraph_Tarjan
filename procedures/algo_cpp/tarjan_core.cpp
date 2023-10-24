#include "./algo.h"
#include "lgraph/olap_base.h"

using namespace std;
using namespace lgraph_api;
using namespace lgraph_api::olap;

void TARJANCore(OlapBase<Empty> &graph, size_t u, tar_pack &pack) {
    pack.dfn[u] = ++pack.para_datas[DEEP];
    pack.low[u] = pack.para_datas[DEEP];
    pack.flag[u] = 1;
    pack.a[++pack.para_datas[TOP]] = u;
    for (auto &edge : graph.OutEdges(u)) {//使用tugraph api遍历每个出节点
        size_t dst = edge.neighbour;
        printf("dst:%ld\n", dst);
        if (pack.dfn[dst] == 0) {
            TARJANCore(graph, dst, pack);
            pack.low[u] = min<int>(pack.low[u], pack.low[dst]);
        } else if (pack.flag[dst])
            pack.low[u] = min<int>(pack.low[u], pack.low[dst]);
    }
    if (pack.dfn[u] == pack.low[u]) { //找到一个强连通分量，将该强连通分量所包含的节点信息进行处理
        pack.color[u] = ++pack.para_datas[SUM];
        pack.map[pack.para_datas[SUM]].push_back(u);
        printf("map:%ld %ld %ld\n", pack.para_datas[SUM],
               pack.map[pack.para_datas[SUM]][pack.map[pack.para_datas[SUM]].size() - 1],
               pack.map[1][1]);
        pack.flag[u] = 0;
        while (pack.a[pack.para_datas[TOP]] != u) {
            pack.color[pack.a[pack.para_datas[TOP]]] = pack.para_datas[SUM];
            pack.map[pack.para_datas[SUM]].push_back(pack.a[pack.para_datas[TOP]]);
            pack.flag[pack.a[pack.para_datas[TOP]--]] = 0;
        }
        pack.para_datas[TOP]--;
    }
}

// void TARJANCore(OlapBase<Empty> &graph, size_t u, tar_pack &pack) {
//     printf("coming in core node:%ld!\n", u);
//     pack.dfn[u] = ++pack.para_datas[DEEP];
//     printf("dfn node:%ld %ld %ld\n", u, pack.dfn[u], graph.NumVertices());
//     pack.low[u] = pack.para_datas[DEEP];
//     pack.flag[u] = 1;
//     pack.a[++pack.para_datas[TOP]] = u;
//     printf("xxx\n");
//     auto active = graph.AllocVertexSubset();
//     auto active_m = graph.AllocVertexSubset();
//     active.Clear();
//     printf("size : %ld\n", active.Size());
//     printf(" in core node:%ld!\n", u);
//     for (auto &edge : graph.OutEdges(u)) {
//         size_t dst = edge.neighbour;
//         printf("dst:%ld\n", dst);
//         if (pack.dfn[dst] == 0) active.Add(dst);
//     }
//     printf("size : %ld\n", active.Size());
//     graph.ProcessVertexActive<size_t>(
//         [&](size_t vi) {
//             if (pack.dfn[vi] == 0) {
//                 TARJANCore(graph, vi, pack);
//                 pack.low[u] = min<int>(pack.low[u], pack.low[vi]);
//             } else if (pack.flag[vi])
//                 pack.low[u] = min<int>(pack.low[u], pack.low[vi]);
//             return vi;
//         },
//         active);
//     if (pack.dfn[u] == pack.low[u]) {
//         pack.color[u] = ++pack.para_datas[SUM];
//         pack.flag[u] = 0;
//         while (pack.a[pack.para_datas[TOP]] != u) {
//             pack.color[pack.a[pack.para_datas[TOP]]] = pack.para_datas[SUM];
//             pack.flag[pack.a[pack.para_datas[TOP]--]] = 0;
//         }
//         pack.para_datas[TOP]--;
//     }
//     printf("last of core!\n");
// }
