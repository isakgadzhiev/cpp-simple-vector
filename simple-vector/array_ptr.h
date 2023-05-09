#include <cassert>
#include <cstdlib>
#include <algorithm>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size == 0) {
            raw_ptr_ = nullptr;
        } else {
            raw_ptr_ = new Type[size];
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr& operator=(const ArrayPtr& rhs) = delete;

    ArrayPtr(ArrayPtr&& other) noexcept
        : raw_ptr_(other.raw_ptr_) {
        other.raw_ptr_ = nullptr;
    }

    ArrayPtr& operator=(ArrayPtr&& rhs) {
        if (this == &rhs) {
            return *this;
        }
        swap(rhs);
        return *this;
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    [[nodiscard]] Type* Release() noexcept {
        Type* temp = raw_ptr_;
        raw_ptr_ = nullptr;
        return temp;
    }

    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    explicit operator bool() const {
        if (raw_ptr_ == nullptr) {
            return false;
        } else {
            return true;
        }
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        auto temp = this->raw_ptr_;
        this->raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = temp;
    }

private:
    Type* raw_ptr_ = nullptr;
};
