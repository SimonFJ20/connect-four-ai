#ifndef NN_MODEL_HPP
#define NN_MODEL_HPP

#include <format>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

namespace connect_four {

#define ASSERT_EQ(LHS, RHS)                                                    \
    {                                                                          \
        if ((LHS) != RHS) {                                                    \
            std::cerr << std::format(                                          \
                "assertion failed: {} != {} in {}\n", #LHS, #RHS, __func__);   \
            std::exit(EXIT_FAILURE);                                           \
        }                                                                      \
    }

class Mx1 {
public:
    Mx1(size_t cols)
        : m_cols(cols)
        , m_data(cols, 0.0)
    {
    }

    Mx1(std::vector<double>&& data)
        : m_cols(data.size())
        , m_data(std::move(data))
    {
    }

    auto operator[](size_t col) const -> double
    {
        return m_data[col];
    }

    auto operator[](size_t col) -> double&
    {
        return m_data[col];
    }

    auto apply(auto func)
    {
        for (auto& v : m_data)
            v = func(v);
    }

    void operator+=(const Mx1& rhs)
    {
        ASSERT_EQ(m_cols, rhs.m_cols);
        for (size_t i = 0; i < m_data.size(); ++i)
            m_data[i] += rhs.m_data[i];
    }
    void operator-=(const Mx1& rhs)
    {
        ASSERT_EQ(m_cols, rhs.m_cols);
        for (size_t i = 0; i < m_data.size(); ++i)
            m_data[i] -= rhs.m_data[i];
    }
    void operator*=(const Mx1& rhs)
    {
        ASSERT_EQ(m_cols, rhs.m_cols);
        for (size_t i = 0; i < m_data.size(); ++i)
            m_data[i] *= rhs.m_data[i];
    }

    void operator+=(double rhs)
    {
        for (auto& v : m_data)
            v += rhs;
    }
    void operator-=(double rhs)
    {
        for (auto& v : m_data)
            v -= rhs;
    }
    void operator*=(double rhs)
    {
        for (auto& v : m_data)
            v *= rhs;
    }

    auto dot(const Mx1& rhs) const -> double
    {
        ASSERT_EQ(m_cols, rhs.cols());
        double res = 0;
        for (size_t col = 0; col < m_cols; ++col) {
            res += m_data[col] * rhs[col];
        }
        return res;
    }

    auto sum() const -> double
    {
        return std::accumulate(m_data.begin(), m_data.end(), 0.0);
    }

    void print() const;

    auto cols() const -> size_t
    {
        return m_cols;
    }

private:
    size_t m_cols;
    std::vector<double> m_data;
};

class Mx2 {
public:
    Mx2(size_t rows, size_t cols)
        : m_rows(rows)
        , m_cols(cols)
        , m_data(rows * cols, 0.0)
    {
    }

    auto at(size_t row, size_t col) const -> double
    {
        return m_data[row * m_cols + col];
    }

    auto at(size_t row, size_t col) -> double&
    {
        return m_data[row * m_cols + col];
    }

    auto apply(auto func)
    {
        for (auto& v : m_data)
            v = func(v);
    }

    void operator+=(const Mx2& rhs)
    {
        ASSERT_EQ(m_rows, rhs.m_rows);
        ASSERT_EQ(m_cols, rhs.m_cols);
        for (size_t i = 0; i < m_data.size(); ++i)
            m_data[i] += rhs.m_data[i];
    }
    void operator-=(const Mx2& rhs)
    {
        ASSERT_EQ(m_rows, rhs.m_rows);
        ASSERT_EQ(m_cols, rhs.m_cols);
        for (size_t i = 0; i < m_data.size(); ++i)
            m_data[i] -= rhs.m_data[i];
    }
    void operator*=(const Mx2& rhs)
    {
        ASSERT_EQ(m_rows, rhs.m_rows);
        ASSERT_EQ(m_cols, rhs.m_cols);
        for (size_t i = 0; i < m_data.size(); ++i)
            m_data[i] *= rhs.m_data[i];
    }

    void operator+=(const Mx1& rhs)
    {
        ASSERT_EQ(m_cols, rhs.cols());
        for (size_t col = 0; col < m_cols; ++col) {
            for (size_t row = 0; row < m_rows; ++row) {
                m_data[row * m_cols + col] += rhs[col];
            }
        }
    }
    void operator-=(const Mx1& rhs)
    {
        ASSERT_EQ(m_cols, rhs.cols());
        for (size_t col = 0; col < m_cols; ++col) {
            for (size_t row = 0; row < m_rows; ++row) {
                m_data[row * m_cols + col] -= rhs[col];
            }
        }
    }
    void operator*=(const Mx1& rhs)
    {
        ASSERT_EQ(m_cols, rhs.cols());
        for (size_t col = 0; col < m_cols; ++col) {
            for (size_t row = 0; row < m_rows; ++row) {
                m_data[row * m_cols + col] *= rhs[col];
            }
        }
    }

    void operator+=(double rhs)
    {
        for (auto& v : m_data)
            v += rhs;
    }
    void operator-=(double rhs)
    {
        for (auto& v : m_data)
            v -= rhs;
    }
    void operator*=(double rhs)
    {
        for (auto& v : m_data)
            v *= rhs;
    }

    auto dot(const Mx1& rhs) const -> Mx1
    {
        ASSERT_EQ(m_cols, rhs.cols());
        auto res = Mx1(m_rows);
        for (size_t row = 0; row < m_rows; ++row) {
            res[row] = 0;
            for (size_t col = 0; col < m_cols; ++col) {
                res[row] += at(row, col) * rhs[col];
            }
        }
        return res;
    }

    auto sum() const -> Mx1
    {
        auto sum = Mx1(m_cols);
        for (size_t col = 0; col < m_cols; ++col) {
            sum[col] = 0;
            for (size_t row = 0; row < m_rows; ++row) {
                sum[col] += m_data[row * m_cols + col];
            }
        }
        return sum;
    }

    void transpose()
    {
        auto data_clone = m_data;
        std::swap(m_rows, m_cols);
        for (size_t row = 0; row < m_rows; ++row)
            for (size_t col = 0; col < m_cols; ++col)
                m_data[row * m_cols + col] = data_clone[col * m_rows + row];
    }

    void print() const;

    auto rows() const -> size_t
    {
        return m_rows;
    }
    auto cols() const -> size_t
    {
        return m_cols;
    }

private:
    size_t m_rows;
    size_t m_cols;
    std::vector<double> m_data;
};

double randd(double min, double max);
double randd_dec(void);
double relu(double x);
double relu_deriv(double x);
double sigmoid(double x);
double sigmoid_deriv(double x);

class Model {
public:
    Model(std::vector<size_t> layers);

    auto feed(const Mx1& input) -> Mx1;
    void mutate();

    struct DataEntry {
        Mx1 input;
        Mx1 correct;
    };
    using Data = std::vector<DataEntry>;
    struct TrainOpts {
        size_t epochs;
        size_t batch_size;
        double learn_rate;
    };

    /// mini-batch stochastic gradient descent
    void train_sgd(Data train_data, const Data& test_data, TrainOpts opts);

private:
    auto backprop(const Mx1& input, const Mx1& correct)
        -> std::tuple<std::vector<Mx2>, std::vector<Mx1>>;
    auto zeroed_weights_biases()
        -> std::tuple<std::vector<Mx2>, std::vector<Mx1>>;

    std::vector<size_t> m_layers;
    std::vector<Mx2> m_weights;
    std::vector<Mx1> m_biases;
};

}

#endif
