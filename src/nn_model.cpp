#include "nn_model.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <format>
#include <iostream>
#include <print>
#include <random>

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
    // return 0.5 * (x / (1 + std::fabs(x)) + 1);
    return 1.0 / (1.0 + std::exp(x));
}

double connect_four::sigmoid_deriv(double x)
{
    // return x * (1 - x);
    return sigmoid(x) * (1 - sigmoid(x));
}

Model::Model(std::vector<size_t> layers)
    : m_layers(std::move(layers))
    , m_weights()
    , m_biases()
{
    m_weights.reserve(m_layers.size() - 1);
    m_biases.reserve(m_layers.size() - 1);

    auto random_dev = std::random_device();
    auto random_gen = std::mt19937(random_dev());
    auto normal_dist = std::normal_distribution(0.0, 0.5);

    for (size_t i = 0; i < m_layers.size() - 1; i++) {
        m_weights.emplace_back(m_layers[i + 1], m_layers[i]);
        m_weights[i].apply([&](double) { return normal_dist(random_gen); });
        m_biases.emplace_back(m_layers[i + 1]);
        m_biases[i].apply([&](double) { return normal_dist(random_gen); });
    }
}

auto Model::feed(const Mx1& inputs) -> Mx1
{
    auto outputs = inputs;
    for (size_t i = 0; i < m_layers.size() - 1; ++i) {
        auto l1 = m_weights[i].dot(outputs);
        l1 += m_biases[i];
        l1.apply(sigmoid);
        outputs = l1;
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

void Model::train_sgd(Data train_data, const Data& test_data, TrainOpts opts)
{
    // http://neuralnetworksanddeeplearning.com/chap1.html
    auto random_dev = std::random_device();
    auto random_gen = std::mt19937(random_dev());

    for (size_t epoch = 0; epoch < opts.epochs; ++epoch) {
        std::shuffle(train_data.begin(), train_data.end(), random_gen);

        //  mini_batches = [
        //      training_data[k:k+mini_batch_size]
        //      for k in xrange(0, n, mini_batch_size)]
        //  for mini_batch in mini_batches:
        //      self.update_mini_batch(mini_batch, eta)
        //
        for (size_t batch = 0; batch < train_data.size();
            batch += opts.batch_size) {

            auto [n_weights, n_biases] = zeroed_weights_biases();

            for (const auto& [input, correct] : train_data) {
                auto [dn_weights, dn_biases] = backprop(input, correct);

                // nabla_w = [nw+dnw for nw, dnw in zip(nabla_w, delta_nabla_w)]
                for (size_t i = 0; i < n_weights.size(); ++i)
                    n_weights[i] += dn_weights[i];

                // nabla_b = [nb+dnb for nb, dnb in zip(nabla_b, delta_nabla_b)]
                for (size_t i = 0; i < n_biases.size(); ++i)
                    n_biases[i] += dn_biases[i];
            }

            auto batch_learn_rate = opts.learn_rate * (double)opts.batch_size;

            for (size_t i = 0; i < m_weights.size(); ++i) {
                auto v = n_weights[i];
                v *= batch_learn_rate;
                m_weights[i] -= v;
            }

            for (size_t i = 0; i < m_biases.size(); ++i) {
                auto v = n_biases[i];
                v *= batch_learn_rate;
                m_biases[i] -= v;
            }
        }
        double sqaure_error = 0;
        for (const auto& [input, correct] : test_data) {
            auto output = feed(input);
            output -= correct;
            sqaure_error += std::pow(output.sum(), 2);
        }
        auto mse = sqaure_error / static_cast<double>(test_data.size());
        std::println(
            "epoch {}/{} done, loss mse: {:.3f}", epoch, opts.epochs, mse);
    }
}

auto Model::backprop(const Mx1& input, const Mx1& correct)
    -> std::tuple<std::vector<Mx2>, std::vector<Mx1>>
{
    auto [n_weights, n_biases] = zeroed_weights_biases();

    // forward pass
    auto activation = input;
    auto activations = std::vector { input };
    auto zs = std::vector<Mx1>();
    for (size_t layer = 0; layer < m_layers.size() - 1; ++layer) {
        auto z = m_weights[layer].dot(activation);
        z += m_biases[layer];
        zs.push_back(z);
        z.apply(sigmoid);
        activation = z;
        activations.push_back(activation);
    }

    // backward pass
    auto delta = activations[activations.size() - 1];
    delta -= correct;
    auto last_z = zs[zs.size() - 1];
    last_z.apply(sigmoid_deriv);
    delta *= last_z;

    auto new_n_weights = Mx2(n_weights[n_weights.size() - 1].rows(),
        n_weights[n_weights.size() - 1].cols());
    new_n_weights += activations[activations.size() - 2];
    new_n_weights *= delta;

    n_weights[n_weights.size() - 1] = new_n_weights;
    n_biases[n_biases.size() - 1] = delta;

    for (size_t layer = 2; layer < m_layers.size(); ++layer) {
        auto z = zs[zs.size() - layer];
        auto sp = z;
        sp.apply(sigmoid_deriv);
        auto weights_T = m_weights[m_weights.size() - layer + 1];
        weights_T.transpose();
        delta = weights_T.dot(delta);
        delta *= sp;

        auto& last_n_weights = n_weights[m_weights.size() - layer];
        auto v = Mx2(last_n_weights.rows(), last_n_weights.cols());
        v += activations[activations.size() - layer + 1];
        v.transpose();
        v *= delta;

        last_n_weights = v;
        n_biases[n_biases.size() - layer] = delta;
    }

    return { n_weights, n_biases };
}

auto Model::zeroed_weights_biases()
    -> std::tuple<std::vector<Mx2>, std::vector<Mx1>>
{

    // nabla_w = [np.zeros(w.shape) for w in self.weights]
    auto n_weights = std::vector<Mx2>();
    n_weights.reserve(m_weights.size());
    for (auto& weights : m_weights)
        n_weights.emplace_back(weights.rows(), weights.cols());

    // nabla_b = [np.zeros(b.shape) for b in self.biases]
    auto n_biases = std::vector<Mx1>();
    n_biases.reserve(m_weights.size());
    for (auto& biases : m_biases)
        n_biases.emplace_back(biases.cols());
    return { n_weights, n_biases };
}
