#include <cstdio>
#include <iostream>
#include <vector>

#include "./algo.h"
#include "olap/olap_on_disk.h"
#include "tools/json.hpp"

using namespace std;
using namespace lgraph_api;
using namespace lgraph_api::olap;
using json = nlohmann::json;

class MyConfig : public ConfigBase<Empty> {
 public:
    std::string root = "0";
    std::string name = std::string("tarjan");
    void AddParameter(fma_common::Configuration &config) {
        ConfigBase<Empty>::AddParameter(config);
        config.Add(root, "root", true).Comment("the root of bfs");
    }
    void Print() {
        ConfigBase<Empty>::Print();
        std::cout << "  name: " << name << std::endl;
        std::cout << "  root: " << root << std::endl;
    }

    MyConfig(int &argc, char **&argv) : ConfigBase<Empty>(argc, argv) {
        fma_common::Configuration config;
        AddParameter(config);
        config.ExitAfterHelp(true);
        config.ParseAndFinalize(argc, argv);
        Print();
    }
};

int main(int argc, char **argv) {
    double start_time;
    MemUsage memUsage;
    memUsage.startMemRecord();
    // prepare
    start_time = get_time();
    MyConfig config(argc, argv);
    OlapOnDisk<Empty> graph;
    size_t root_vid = 0;
    graph.Load(config, DUAL_DIRECTION);
    if (config.id_mapping) {
        root_vid = graph.hash_list_.find(config.root);
    } else {
        root_vid = std::stoi(config.root);
    }

    printf("root_vid=%ld\n", root_vid);
    memUsage.print();
    memUsage.reset();
    auto prepare_cost = get_time() - start_time;
    printf("prepare_cost = %.2lf(s)\n", prepare_cost);

    // core
    start_time = get_time();
    auto parent = graph.AllocVertexArray<size_t>();

    auto flag = graph.AllocVertexArray<size_t>();
    auto low = graph.AllocVertexArray<size_t>();
    auto dfn = graph.AllocVertexArray<size_t>();
    auto a = graph.AllocVertexArray<size_t>();
    auto color = graph.AllocVertexArray<size_t>();
    auto cnt = graph.AllocVertexArray<size_t>();
    auto num_vertex = graph.NumVertices();
    tar_pack pack = tar_pack(flag, low, dfn, a, color, cnt, num_vertex);

    printf("%ld\n", graph.NumVertices());
    for (size_t i = 0; i < graph.NumVertices(); i++)
        if (pack.dfn[i] == 0) TARJANCore(graph, i, pack);
    printf("end of core!\n");
    memUsage.print();
    memUsage.reset();
    auto core_cost = get_time() - start_time;
    printf("core_cost = %.2lf(s)\n", core_cost);

    // output
    start_time = get_time();
    // TODO(any): write to file
    for (size_t i = 0; i < graph.NumVertices(); i++) {   //统计各个强连通分量所含点的个数
        printf("node:%ld  color:%ld\n", i, pack.color[i]);
        pack.cnt[pack.color[i]]++;
    }
    for (size_t i = 1; i <= pack.para_datas[SUM]; i++) {    //打印各个强连通分量的信息，包括index、所包含的节点个数、所包含的节点编号
        printf("strongly connected components index:%ld   number of nodes:%ld nodes:", i,
               pack.cnt[i]);
        for (size_t j = 0; j < pack.cnt[i]; j++) printf("%ld ", pack.map[i][j]);
        printf("\n");
    }
    auto output_cost = get_time() - start_time;
    printf("output_cost = %.2lf(s)\n", output_cost);

    printf("total_cost = %.2lf(s)\n", prepare_cost + core_cost + output_cost);
    printf("DONE.");

    return 0;
}
