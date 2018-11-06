//
// Created by kier on 2018/7/18.
//

#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <cassert>
#include <module/module.h>


#include "module/module.h"
#include "utils/box.h"
#include "core/tensor_builder.h"

namespace ts {
    const char *const Bubble::Parameter = "<param>";
    const char *const Bubble::Const = "<const>";
    const char *const Bubble::Variable = "<var>";
    static const std::unordered_set<std::string> EndPoints = {Bubble::Parameter, Bubble::Const, Bubble::Variable};

    bool Bubble::IsEndPoint(const std::string &op) {
        return EndPoints.find(op) != EndPoints.end();
    }

    Bubble::Bubble(const std::string &op)
            : m_op(op) {
        update_retention_params();
    }

    Bubble::Bubble(const std::string &op, const std::string &name)
            : m_op(op)
            , m_name(name) {
        update_retention_params();
    }

    Bubble::Bubble(const std::string &op, int output_count)
            : m_op(op)
            , m_output_count(output_count) {
        assert(output_count >= 1);
        update_retention_params();
    }

    Bubble::Bubble(const std::string &op, const std::string &name, int output_count)
            : m_op(op)
            , m_name(name)
            , m_output_count(output_count) {
        assert(output_count >= 1);
        update_retention_params();
    }

    bool Bubble::has(const std::string &param) const {
        return this->m_params.find(param) != this->m_params.end();
    }

    void Bubble::set(const std::string &param, const Tensor &value) {
        this->m_params.insert(std::make_pair(param, value));
    }

    Tensor &Bubble::get(const std::string &param) {
        auto param_it = m_params.find(param);
        if (param_it == m_params.end()) {
            TS_LOG_ERROR << "Unidentified param \"" << param << "\", did you mean \"" << fuzzy_param_name(param) << "\""
                         << eject;
        }
        return param_it->second;
    }

    const Tensor &Bubble::get(const std::string &param) const {
        return const_cast<self *>(this)->get(param);
    }

    void Bubble::clear(const std::string &param) {
        auto param_it = m_params.find(param);
        if (param_it == m_params.end()) {
            TS_LOG_ERROR << "Unidentified param \"" << param << "\", did you mean \"" << fuzzy_param_name(param) << "\""
                         << eject;
        }
        this->m_params.erase(param_it);
    }

    void Bubble::clear_params() {
        std::vector<std::pair<std::string, Tensor>> retention_params;
        for (auto &param_tenosr_pair : m_params) {
            auto &param = param_tenosr_pair.first;
            bool is_retention_param = !param.empty() && param[0] == retention_param_sign;
            if (is_retention_param) {
                retention_params.emplace_back(param_tenosr_pair);
            }
        }
        m_params.clear();
        m_params.insert(retention_params.begin(), retention_params.end());
    }

    std::string Bubble::fuzzy_param_name(const std::string &name) {
        if (m_params.empty()) return "";
        int min_edit_distance = INT_MAX;
        std::string closest_name;
        for (auto &param_tensor_pair : m_params) {
            auto &target_name = param_tensor_pair.first;
            int dist = edit_distance(name, target_name);
            if (dist < min_edit_distance) {
                closest_name = target_name;
                min_edit_distance = dist;
            }
        }
        return closest_name;
    }

    void Bubble::update_retention_params() {
        assert(retention_param_sign == '#');
        set("#op", tensor::from(m_op));
        set("#name", tensor::from(m_name));
        set("#output_count", tensor::from(m_output_count));
    }

    void Module::load(Graph g) {
        auto nodes = g.nodes();
        std::vector<Node> outputs;
        for (auto &node : nodes) {
            if (node.outputs().empty()) {
                outputs.push_back(node);
            }
        }
        load(g, outputs);
    }

    void Module::load(Graph g, const std::vector<Node> &outputs) {
        auto inputs = graph_walker(g, outputs);

        m_inputs.insert(m_inputs.end(), inputs.begin(), inputs.end());
        m_outputs.insert(m_outputs.end(), outputs.begin(), outputs.end());
        m_graphs.push_back(g);
    }

    void Module::load(Graph g, const std::vector<std::string> &outputs) {
        std::unordered_map<std::string, Node> map_name_found_node;
        size_t found_count = 0;
        Graph local_graph;
        Node empty_node = local_graph.make<Bubble>("_empty");
        for (auto &output_name : outputs) {
            map_name_found_node.insert(std::make_pair(output_name, empty_node));
        }
        auto nodes = g.nodes();
        for (auto &node : nodes) {
            auto &bubble = node.ref<Bubble>();
            auto name_it = map_name_found_node.find(bubble.name());
            if (name_it == map_name_found_node.end()) {
                continue;
            }
            if (name_it->second != empty_node) {
                TS_LOG_ERROR << "Found duplicate Node " << node.str() << ", with Node " << name_it->second.str()
                             << eject;
            }
            ++found_count;
            name_it->second = node;
        }
        if (found_count != map_name_found_node.size()) {
            std::ostringstream oss;
            oss << "Can not found those names in graph: ";
            size_t not_found_name_count = 0;
            for (auto &name_found_node_pair : map_name_found_node) {
                if (name_found_node_pair.second != empty_node) continue;
                if (not_found_name_count) oss << ", ";
                oss << name_found_node_pair.first;
                ++not_found_name_count;
            }
            TS_LOG_ERROR(oss.str()) << eject;
        }
        std::vector<Node> found_nodes;
        found_nodes.reserve(outputs.size());
        for (auto &output_name : outputs) {
            Node &found_node = map_name_found_node.at(output_name);
            found_nodes.emplace_back(found_node);
        }
        this->load(g, found_nodes);
    }

    std::vector<Node> Module::graph_walker(Graph g, const std::vector<Node> &outputs) {
        // calculate inputs and outputs
        auto nodes = g.nodes();
        std::unordered_set<Node> nodes_set;
        for (auto &node : nodes) {
            nodes_set.insert(node);
        }
        // valid graph, get inputs
        std::vector<Node> inputs;
        std::queue<Node> walker;
        std::unordered_set<Node> walked;
        for (auto &node : outputs) walker.push(node);
        while (!walker.empty()) {
            auto node = walker.front();
            walker.pop();
            if (walked.find(node) != walked.end()) continue;
            walked.insert(node);
            if (nodes_set.find(node) == nodes_set.end()) {
                TS_LOG_ERROR << "Found unlinked node in graph: " << node.str() << eject;
            }
            auto &op = node.ref<Bubble>();
            if (op.op() == Bubble::Parameter) {
                inputs.push_back(node);
                continue;
            }
            if (op.op() == Bubble::Const) {
                continue;
            }
            if (node.inputs().empty()) {
                TS_LOG_ERROR << "Found not computable node: " << node.str() << eject;
            }
            for (auto &input : node.inputs()) {
                walker.push(input);
            }
        }
        // remove duplicate inputs
        std::unordered_set<Node> input_nodes_set;
        for (auto &input : inputs) input_nodes_set.insert(input);

        // return non-duplicate nodes
        return std::vector<Node>(input_nodes_set.begin(), input_nodes_set.end());
    }

    void Module::clear() {
        m_inputs.clear();
        m_outputs.clear();
        m_graphs.clear();
    }

    void Module::sort_inputs(const std::vector<Node> &inputs) {
        std::unordered_set<Node> sorted_input_set(inputs.begin(), inputs.end());
        for (auto &had_input : m_inputs) {
            if (sorted_input_set.find(had_input) == sorted_input_set.end()) {
                TS_LOG_ERROR << "The sorted inputs must content " << had_input.str() << eject;
            }
        }
        m_inputs = inputs;
    }

    void Module::sort_inputs(const std::vector<std::string> &input_names) {
        std::unordered_map<std::string, Node> map_name_input_node;
        // map nodes
        for (auto &input : m_inputs) {
            auto &bubble = input.ref<Bubble>();
            if (map_name_input_node.find(bubble.name()) != map_name_input_node.end()) {
                auto it = map_name_input_node.find(bubble.name());
                TS_LOG_ERROR << "Can not sort inputs with duplicate names: " << input.str() << " and "
                             << it->second.str() << eject;
            }
            map_name_input_node.insert(std::make_pair(bubble.name(), input));
        }
        // check inputs node
        std::unordered_set<Node> used_inputs;   // make sure all inputs must be used after used
        std::vector<Node> sorted_inputs;
        for (auto &input_name : input_names) {
            auto name_node_it = map_name_input_node.find(input_name);
            if (name_node_it == map_name_input_node.end()) {
                TS_LOG_ERROR << "Can not recognize name " << input_name << eject;
            }
            auto &node = name_node_it->second;
            sorted_inputs.emplace_back(node);
            used_inputs.insert(node);
        }
        if (used_inputs.size() < map_name_input_node.size()) {
            std::ostringstream oss;
            oss << "All inputs must be used after sorted, missing: ";
            size_t missing_count = 0;
            for (auto &name_node_pair : map_name_input_node) {
                auto &node = name_node_pair.second;
                if (used_inputs.find(node) != used_inputs.end()) continue;
                if (missing_count) oss << ", ";
                oss << name_node_pair.first;
                ++missing_count;
            }
            TS_LOG_ERROR(oss.str()) << eject;
        }
        m_inputs = sorted_inputs;
    }

    void Module::sort_inputs(const std::initializer_list<std::string> &input_names) {
        this->sort_inputs(std::vector<std::string>(input_names.begin(), input_names.end()));
    }
}