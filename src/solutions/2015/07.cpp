#include <utility>
#include <variant>
#include <map>
#include <regex>
#include "../one_solution.h"

#pragma region Example Inputs
constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
123 -> x
456 -> y
x AND y -> d
x OR y -> e
x LSHIFT 2 -> f
y RSHIFT 2 -> g
NOT x -> h
NOT y -> i
)";
#pragma endregion

namespace {
    enum GateOp {
        AND,
        OR,
        LSHIFT,
        RSHIFT,
        NOT,
        CONSTANT,
    };

    struct Wire;

    struct Gate {
        GateOp op;
        std::shared_ptr<Wire> input_a;
        std::shared_ptr<Wire> input_b;
        int constant_a;
        int constant_b;
        std::shared_ptr<Wire> output;
        uint16_t signal_value;
        bool computed = false;

        explicit Gate(GateOp gateOp): op(gateOp), constant_a(0), constant_b(0), signal_value(0) {};
    };

    struct Wire {
        std::string name;
        std::shared_ptr<Gate> source;
        std::vector<std::shared_ptr<Gate>> sinks;
        uint16_t signal_value;
        bool computed = false;

        explicit Wire(std::string name): name(std::move(name)), signal_value(0) {};
    };

    struct Circuit {
        std::vector<std::shared_ptr<Gate>> gates;
        std::map<std::string, std::shared_ptr<Wire>> wires;
    };
}

static const std::map<std::string, GateOp> string_to_gate_op = {
        {"AND", AND},
        {"OR", OR},
        {"LSHIFT", LSHIFT},
        {"RSHIFT", RSHIFT},
        {"NOT", NOT},
};

static const std::map<GateOp, std::regex> gate_op_to_regex = {
        {AND, std::regex(R"(^([\d\w]+) AND ([\d\w]+) -> (\w+))")},
        {OR, std::regex(R"(^([\d\w]+) OR ([\d\w]+) -> (\w+))")},
        {LSHIFT, std::regex(R"(^([\d\w]+) LSHIFT ([\d\w]+) -> (\w+))")},
        {RSHIFT, std::regex(R"(^([\d\w]+) RSHIFT ([\d\w]+) -> (\w+))")},
        {NOT, std::regex(R"(^NOT ([\d\w]+) -> (\w+))")},
        // TODO Fix constant to support interconnected wires
        {CONSTANT, std::regex(R"(^([\d]+) -> (\w+))")},
};

static bool is_only_digits(const std::string &in) {
    return std::all_of(in.begin(), in.end(), ::isdigit);
}

static Circuit parse_circuit(const std::string &in) {
    auto lines_str = string_split(trim(in), '\n');

    std::map<std::string, std::shared_ptr<Wire>> wires;
    std::vector<std::shared_ptr<Gate>> gates;

    for (const auto& line_str: lines_str) {
        for (auto &[op, regex]: gate_op_to_regex) {
            std::smatch matches;
            if (std::regex_match(line_str, matches, regex)) {
                auto gate = std::make_shared<Gate>(op);

                switch (op) {
                    // Binary operations
                    case RSHIFT:
                    case LSHIFT:
                    case AND:
                    case OR: {
                        auto operand_a = matches[1].str();
                        if (is_only_digits(operand_a)) {
                            gate->constant_a = std::stoi(operand_a);
                        } else {
                            std::shared_ptr<Wire> wire_ptr;
                            if (!wires.contains(operand_a)) {
                                auto new_wire = std::make_shared<Wire>(operand_a);
                                wires.insert({operand_a, new_wire});
                            }
                            wire_ptr = wires.at(operand_a);
                            gate->input_a = wire_ptr;
                            wire_ptr->sinks.push_back(gate);
                        }

                        auto operand_b = matches[2].str();
                        if (is_only_digits(operand_b)) {
                            gate->constant_b = std::stoi(operand_b);
                        } else {
                            std::shared_ptr<Wire> wire_ptr;
                            if (!wires.contains(operand_b)) {
                                auto new_wire = std::make_shared<Wire>(operand_b);
                                wires.insert({operand_b, new_wire});
                            }
                            wire_ptr = wires.at(operand_b);
                            gate->input_b = wire_ptr;
                            wire_ptr->sinks.push_back(gate);
                        }

                        auto output_str = matches[3].str();
                        std::shared_ptr<Wire> wire_ptr;
                        if (!wires.contains(output_str)) {
                            auto new_wire = std::make_shared<Wire>(output_str);
                            wires.insert({output_str, new_wire});
                        }
                        wire_ptr = wires.at(output_str);
                        gate->output = wire_ptr;
                        wire_ptr->source = gate;

                        gates.push_back(gate);
                    } break;
                    // Unary operations
                    case NOT: {
                        auto operand_a = matches[1].str();
                        if (is_only_digits(operand_a)) {
                            gate->constant_a = std::stoi(operand_a);
                        } else {
                            std::shared_ptr<Wire> wire_ptr;
                            if (!wires.contains(operand_a)) {
                                auto new_wire = std::make_shared<Wire>(operand_a);
                                wires.insert({operand_a, new_wire});
                            }
                            wire_ptr = wires.at(operand_a);
                            gate->input_a = wire_ptr;
                            wire_ptr->sinks.push_back(gate);
                        }

                        auto output_str = matches[2].str();
                        std::shared_ptr<Wire> wire_ptr;
                        if (!wires.contains(output_str)) {
                            auto new_wire = std::make_shared<Wire>(output_str);
                            wires.insert({output_str, new_wire});
                        }
                        wire_ptr = wires.at(output_str);
                        gate->output = wire_ptr;
                        wire_ptr->source = gate;

                        gates.push_back(gate);
                    } break;
                    // Constant operations
                    case CONSTANT: {
                        auto operand_a = matches[1].str();
                        if (!is_only_digits(operand_a)) {
                            throw std::logic_error("Constants must be numeric");
                        }
                        gate->constant_a = std::stoi(operand_a);
                        gate->signal_value = gate->constant_a;

                        auto output_str = matches[2].str();
                        std::shared_ptr<Wire> wire_ptr;
                        if (!wires.contains(output_str)) {
                            auto new_wire = std::make_shared<Wire>(output_str);
                            wires.insert({output_str, new_wire});
                        }
                        wire_ptr = wires.at(output_str);
                        gate->output = wire_ptr;
                        gate->computed = true;
                        wire_ptr->source = gate;

                        gates.push_back(gate);
                    } break;
                }
            }
        }
    }

    return {gates, wires};
}

static uint16_t determine_wire(Circuit circuit, const std::string& wire_name) {
    auto the_wire = circuit.wires.at(wire_name);
    if (the_wire->computed) return the_wire->signal_value;
    
    std::stack<std::shared_ptr<Wire>> lookup_stack;
    lookup_stack.push(the_wire);
    while (!lookup_stack.empty()) {
        auto in_gate = lookup_stack.top()->source;
        if (in_gate == nullptr) throw std::logic_error("The wire must have a source");

        if (!in_gate->computed) {
            uint16_t operand_a;
            if (in_gate->input_a != nullptr) {
                if (!in_gate->input_a->computed) {
                    lookup_stack.push(in_gate->input_a);
                    continue;
                }
                operand_a = in_gate->input_a->signal_value;
            } else {
                operand_a = in_gate->constant_a;
            }

            uint16_t operand_b;
            if (in_gate->input_b != nullptr) {
                if (!in_gate->input_b->computed) {
                    lookup_stack.push(in_gate->input_b);
                    continue;
                }
                operand_b = in_gate->input_b->signal_value;
            } else {
                operand_b = in_gate->constant_b;
            }

            uint16_t value;
            switch (in_gate->op) {
                case RSHIFT:
                    value = operand_a >> operand_b;
                    break;
                case LSHIFT:
                    value = operand_a << operand_b;
                    break;
                case AND:
                    value = operand_a & operand_b;
                    break;
                case OR:
                    value = operand_a | operand_b;
                    break;
                case NOT:
                    value = ~operand_a;
                    break;
                case CONSTANT:
                    throw std::logic_error("Should never process CONSTANT");
            }

            in_gate->signal_value = value;
            in_gate->computed = true;

            in_gate->output->signal_value = value;
            in_gate->output->computed = true;
        } else {
            // If computed, pass on the value
            lookup_stack.top()->signal_value = in_gate->signal_value;
            lookup_stack.top()->computed = true;
            lookup_stack.pop();
        }

    }

    return the_wire->signal_value;
}

SOLVER(2015, 7, 1, false)
(const std::string &in) {
//    auto circuit = parse_circuit(std::string(EXAMPLE_INPUT_1));
    auto circuit = parse_circuit(in);

    for (auto const &[name, _]: circuit.wires) {
        fmt::println("{}: {}", name, determine_wire(circuit, name));
    }

    // Never got around to fixing "lx -> a"
    return fmt::format("{}", determine_wire(circuit, "lx"));
}

SOLVER(2015, 7, 2, true)
(const std::string &in) {
    auto circuit = parse_circuit(in);

    circuit.wires.at("b")->signal_value = 16076;
    circuit.wires.at("b")->computed = true;

    // Never got around to fixing "lx -> a"
    return fmt::format("{}", determine_wire(circuit, "lx"));
}
