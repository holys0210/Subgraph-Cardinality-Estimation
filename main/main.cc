#include <algorithm>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

#include "global/timer.h"
#include "include/daf_candidate_space.h"
#include "include/daf_dag.h"
#include "include/daf_data_graph.h"
#include "include/daf_query_graph.h"
#include "include/treesampling.h"
using namespace daf;
const int UNIFORMRANDOM = 1;

//std::string dataset, ans_file_name, data_root;
std::string dataset = "yeast", ans_file_name = "yeast_ans", data_root = "../../dataset/";
std::string data_name = "../../dataset/yeast/data_graph/yeast.graph";
std::string query_name = "../../dataset/yeast/query_graph/query_dense_8_1.graph";
std::vector<std::string> query_names = {query_name};

int num_samples = 1000000;

std::unordered_map<std::string, double> true_cnt;
void run_treesample (DataGraph &data, QueryGraph &query) {
    Timer total_timer, sample_timer;
    std::cout << "Query Graph: " << query_name << "\n";
    total_timer.Start();
    query.LoadAndProcessGraph(data);
    total_timer.Stop();

    daf::DAG dag(data, query);

    total_timer.Start();
    dag.BuildDAG();
    total_timer.Stop();

    total_timer.Start();
    daf::TreeSampling treesampling(data, query, dag);
    total_timer.Stop();

    for (auto u = 0; u < query.GetNumVertices(); ++u) {
        if (treesampling.CS.GetCandidateSetSize(u) == 0) {
            std::cout << "Total time: " << total_timer.GetTime() << " ms\n";
            exit(1);
        }
    }
    sample_timer.Start();
    double est = treesampling.EstimateEmbeddings(num_samples);
    sample_timer.Stop();

    total_timer.Add(sample_timer);
    std::cout << "Total time: " << total_timer.GetTime() << " ms\n";
    std::cout << "Search time: " << sample_timer.GetTime() << " ms\n";
    std::cout << "#TRUE : " << std::fixed << std::setprecision(6) << true_cnt[query_name] << std::endl;
    std::cout << "#Matches(Approx) : " << std::fixed << std::setprecision(6) << est << std::endl;
    std::cout << "#Tree : " << std::fixed << std::setprecision(6) << treesampling.total_trees_ << std::endl;
    std::cout << "Query Finished" << std::endl;
}


void loadFullDataset() {
    std::cerr << "Loading dataset " << dataset << std::endl;
    data_name = data_root+dataset+"/data_graph/"+dataset+".graph";
    ans_file_name = data_root+dataset+"/"+dataset+"_ans.txt";
    std::cerr << data_name << std::endl;
    std::cerr << ans_file_name << std::endl;
    std::ifstream ans_in(ans_file_name);
    query_names.clear();
    while (!ans_in.eof()) {
        std::string name, t, c;
        ans_in >> name >> t >> c;
        if (name.empty() || t.empty() || c.empty()) continue;
        name = data_root+dataset+"/query_graph/"+name;
        query_names.push_back(name);
        std::string::size_type sz = 0;
        true_cnt[name] = atoll(c.c_str()) * 1.0;
    }
}
int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'R':
                    data_root = argv[i + 1];
                    break;
                case 'D':
                    dataset = argv[i + 1];
                    break;
                case 'd':
                    data_name = argv[i + 1];
                    break;
                case 'q':
                    query_name = argv[i + 1];
                    query_names = {query_name};
                    break;
                case 's':
                    num_samples = std::atoi(argv[i + 1]);
            }
        }
    }
    if (dataset.length() > 0) {
        loadFullDataset();
    }

    std::cout << "Loading data graph...\n";
    DataGraph data(data_name);
    data.LoadAndProcessGraph();

    std::cout << "Loading query graph...\n";
    int q_cnt = 0;
    for (std::string &qname : query_names) {
        q_cnt++;
        fprintf(stderr, "%-10s\t%-10s\tQ%04d/%04lu:\t%s...\n",
                "TreeSampling", dataset.c_str(),
                q_cnt, query_names.size(), qname.c_str());
        query_name = qname;
        QueryGraph query(qname);
        run_treesample(data, query);
    }
}
