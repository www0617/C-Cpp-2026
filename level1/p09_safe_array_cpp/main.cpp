#include <iostream>
#include <stdexcept>

// SafeArray: a class that wraps a plain array and throws on out-of-range access.
// (1) basic int array with bounds checking
// (2) operator[] overloading
// (3) template version, works for any type
template <typename T>
class SafeArray {
public:
    // create n elements, all initialized to val
    explicit SafeArray(std::size_t n, const T& val = T{})
        : size_(n), data_(new T[n]) {
        for (std::size_t i = 0; i < n; ++i) data_[i] = val;
    }

    // deep copy, so two objects never share memory
    SafeArray(const SafeArray& other)
        : size_(other.size_), data_(new T[other.size_]) {
        for (std::size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
    }

    // copy assignment, handles self-assignment
    SafeArray& operator=(const SafeArray& other) {
        if (this != &other) {
            T* fresh = new T[other.size_];
            for (std::size_t i = 0; i < other.size_; ++i) fresh[i] = other.data_[i];
            delete[] data_;
            data_ = fresh;
            size_ = other.size_;
        }
        return *this;
    }

    ~SafeArray() { delete[] data_; }

    // index access, throws std::out_of_range when out of bounds
    T& operator[](std::size_t i) {
        if (i >= size_) throw std::out_of_range("index out of range");
        return data_[i];
    }

    // const version (read-only)
    const T& operator[](std::size_t i) const {
        if (i >= size_) throw std::out_of_range("index out of range");
        return data_[i];
    }

    std::size_t size() const { return size_; }

private:
    std::size_t size_;   // number of elements
    T* data_;            // heap memory
};

int main() {
    SafeArray<int> a(5);
    for (std::size_t i = 0; i < a.size(); ++i) a[i] = static_cast<int>(i * 10);

    std::cout << "int array:";
    for (std::size_t i = 0; i < a.size(); ++i) std::cout << ' ' << a[i];
    std::cout << '\n';

    try {
        std::cout << a[5] << '\n';   // out of range on purpose
    } catch (const std::out_of_range& e) {
        std::cout << "caught: " << e.what() << '\n';
    }

    SafeArray<double> d(3, 1.5);
    d[1] = 2.25;
    std::cout << "double array:";
    for (std::size_t i = 0; i < d.size(); ++i) std::cout << ' ' << d[i];
    std::cout << '\n';

    return 0;
}