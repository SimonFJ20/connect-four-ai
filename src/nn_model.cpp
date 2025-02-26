#include "nn_model.hpp"
#include <cmath>
#include <cstdlib>
#include <format>
#include <iostream>

using namespace connect_four;

void Mx1::print() const
{

    std::cout << std::format("\u250c       \u2510\n");
    for (size_t col = 0; col < m_cols; ++col) {
        std::cout << std::format("\u2502 {:5.2f} \u2502\n", m_data[col]);
    }
    std::cout << std::format("\u2514       \u2518\n");
}

void Mx2::print() const
{
    std::cout << "\u250c ";
    for (size_t row = 0; row < m_rows; ++row) {
        std::cout << "      ";
    }
    std::cout << "\u2510\n";
    for (size_t col = 0; col < m_cols; ++col) {
        std::cout << "\u2502 ";
        for (size_t row = 0; row < m_rows; ++row) {
            double v = this->at(row, col);
            if (v == 0) {
                std::cout << "\x1b[90m";
            }
            std::cout << std::format("{: 5.2f} \x1b[0m", v);
        }
        std::cout << "\u2502\n";
    }
    std::cout << "\u2514 ";
    for (size_t row = 0; row < m_rows; ++row) {
        std::cout << "      ";
    }
    std::cout << "\u2518\n";
}

Model::Model(std::vector<size_t> layers)
    : m_layers(std::move(layers))
    , m_weights()
    , m_biases()
{
    m_weights.reserve(m_layers.size() - 1);
    m_biases.reserve(m_layers.size() - 1);

    for (size_t i = 0; i < m_layers.size() - 1; i++) {
        m_weights.push_back(Mx2(m_layers[i], m_layers[i + 1]));
        m_weights[i].apply([](double) { return randd_dec(); });
        m_biases.push_back(Mx1(m_layers[i + 1]));
        m_biases[i].apply([](double) { return randd_dec(); });
    }
}

auto Model::feed(const Mx1& inputs) -> Mx1
{
    auto outputs = inputs;
    for (size_t i = 0; i < m_layers.size() - 1; ++i) {
        auto l1 = m_weights[i];
        l1.transpose();
        l1 *= outputs;
        l1.transpose();
        l1 += m_biases[i];
        Mx1 l3 = l1.sum();
        l3.apply(sigmoid);
        outputs = l3;
    }
    return outputs;
}

static inline double mutation_dec(double in)
{
    (void)in;
    double r = randd_dec();
    if (r > 0.5) {
        double v = pow(r * 2.0, 2.0) / 4.0;
        return v > 0.5 ? v : 0.0;
    } else {
        double v = pow((1.0 - r) * 2.0, 2.0) / 4.0;
        return v > 0.5 ? -v : 0.0;
    }
}

void Model::mutate()
{
    double learning_rate = 0.5;

    for (auto& layer_weights : m_weights) {
        auto mutation = Mx2(layer_weights.rows(), layer_weights.cols());
        mutation.apply(mutation_dec);
        mutation *= learning_rate;
        layer_weights += mutation;
    }

    for (auto& layer_bias : m_biases) {
        auto mutation = Mx1(layer_bias.cols());
        mutation.apply(mutation_dec);
        mutation *= learning_rate;
        layer_bias += mutation;
    }
}

double connect_four::randd(double min, double max)
{
    return min + (double)std::rand() / (RAND_MAX / (max - min));
}

double connect_four::randd_dec(void)
{
    return randd(0.0, 1.0);
}

double connect_four::relu(double x)
{
    return x > 0 ? x : 0;
}

double connect_four::relu_deriv(double x)
{
    return x > 0 ? 1 : 0;
}

double connect_four::sigmoid(double x)
{
    return 0.5 * (x / (1 + std::fabs(x)) + 1);
}

double connect_four::sigmoid_deriv(double x)
{
    return x * (1 - x);
}
