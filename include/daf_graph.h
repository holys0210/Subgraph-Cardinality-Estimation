#ifndef GRAPH_H_
#define GRAPH_H_

#include <set>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "global/global.h"

namespace daf {
    class Graph {
    public:
        explicit Graph(const std::string &filename);

        ~Graph();

        Graph &operator=(const Graph &) = delete;

        Graph(const Graph &) = delete;

        inline Size GetNumLabels() const;

        inline Size GetNumVertices() const;

        inline Size GetNumEdges() const;

        inline Size GetMaxDegree() const;

        inline Label GetLabel(Vertex v) const;

        inline Size GetStartOffset(Vertex v) const;

        inline Size GetEndOffset(Vertex v) const;

        inline Size GetDegree(Vertex v) const;

        inline Size GetCoreNum(Vertex v) const;

        inline Label GetLabelFrequency(Label l) const;

        inline Vertex GetNeighbor(Size i) const;

        inline bool CheckEdgeExist(Vertex u, Vertex v) const;

        std::set<std::pair<Vertex, Vertex>> edge_exists;
        Size num_label_;
    protected:
        std::vector<std::vector<Vertex>> adj_list;
        std::unordered_map<Label, Label> transferred_label_map;
        Label *true_label_;
        void relabel();

        void LoadRoughGraph(std::vector<std::vector<Vertex>> *graph);

        void computeCoreNum();

        Size num_vertex_;
        Size num_edge_;

        Size max_degree_;

        Label *label_;
        Size *start_off_;
        Vertex *linear_adj_list_;
        Size *label_frequency_;

        Size *core_num_;

        const std::string &filename_;
        std::ifstream fin_;
    };

    inline Size Graph::GetNumLabels() const { return num_label_; }

    inline Size Graph::GetNumVertices() const { return num_vertex_; }

    inline Size Graph::GetNumEdges() const { return num_edge_; }

    inline Size Graph::GetMaxDegree() const { return max_degree_; }

    inline Label Graph::GetLabel(Vertex v) const { return label_[v]; }

    inline Size Graph::GetStartOffset(Vertex v) const { return start_off_[v]; }

    inline Size Graph::GetEndOffset(Vertex v) const { return start_off_[v + 1]; }

    inline Size Graph::GetDegree(Vertex v) const {
        return start_off_[v + 1] - start_off_[v];
    }

    inline Size Graph::GetCoreNum(Vertex v) const { return core_num_[v]; }

    inline Label Graph::GetLabelFrequency(Label l) const {
        return label_frequency_[l];
    }

    inline Vertex Graph::GetNeighbor(Size i) const { return linear_adj_list_[i]; }


    inline bool Graph::CheckEdgeExist(Vertex u, Vertex v) const {
        return (edge_exists.find(std::make_pair(u, v)) != edge_exists.end())
               || (edge_exists.find(std::make_pair(v, u)) != edge_exists.end());
    }

}

#endif  // GRAPH_H_
