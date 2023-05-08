#pragma once

#include <cassert>
#include <stdexcept>
#include <initializer_list>
#include <algorithm>

#include "array_ptr.h"

class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t capacity_to_reserve)
            : capacity_(capacity_to_reserve) {}
    size_t capacity_;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() = default; // конструктор по умолчанию

    explicit SimpleVector(size_t size)
            : items_(size), size_(size), capacity_(size) {
        std::fill(begin(), end(), Type{});
    }

    explicit SimpleVector(ReserveProxyObj obj) {
        Reserve(obj.capacity_);
    }

    SimpleVector(size_t size, const Type& value)
            : items_(size), size_(size), capacity_(size) {
        std::fill(begin(), end(), value);
    }

    SimpleVector(std::initializer_list<Type> init)
            : items_(init.size()), size_(init.size()), capacity_(init.size()) {
        std::copy(init.begin(), init.end(), begin());
    }

    SimpleVector(const SimpleVector& other)
            : items_(other.size_), size_(other.size_), capacity_(other.capacity_) {
        std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector temp(rhs);
            swap(temp);
        }
        return *this;
    }

    SimpleVector(SimpleVector&& other)
            : items_(other.size_), size_(other.size_), capacity_(other.capacity_) {
        items_ = std::move(other.items_);
        other.size_ = 0;
        other.capacity_ = 0;
    };

    SimpleVector& operator=(SimpleVector&&) = default;

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range ("Index more than size");
        } else {
            return items_[index];
        }
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range ("Index more than size");
        } else {
            return items_[index];
        }
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            capacity_ = std::max(capacity_ * 2, new_size);
            ArrayPtr<Type> new_items(capacity_);
            std::move(begin(), end(), new_items.Get());
            for (auto it = new_items.Get() + size_; it != new_items.Get() + new_size; ++it) {
                *it = Type {};
            }
            items_.swap(new_items);
        } else if (new_size > size_) {
            for (auto it = end(); it != begin() + new_size; ++it) {
                *it = Type {};
            }
        }
        size_ = new_size;
    }

    void PushBack(const Type& item) {
        if (size_ < capacity_) {
            items_[size_] = item;
            ++size_;
        } else {
            capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_items (capacity_);
            std::copy(begin(), end(), new_items.Get());
            new_items[size_] = item;
            items_.swap(new_items);
            ++size_;
        }
    }

    void PushBack(Type&& item) {
        if (size_ < capacity_) {
            items_[size_] = std::move(item);
            ++size_;
        } else {
            capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_items (capacity_);
            std::move(begin(), end(), new_items.Get());
            new_items[size_] = std::move(item);
            items_.swap(new_items);
            ++size_;
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos <= end() && pos >= begin());
        auto index = pos - begin();
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_*2;
            ArrayPtr<Type> new_items (new_capacity);
            std::copy(begin(), begin() + index, new_items.Get());
            new_items[index] = value;
            std::copy(begin() + index, end(), new_items.Get() + index + 1);
            ++ size_;
            capacity_ = new_capacity;
            items_.swap(new_items);
        } else {
            std::copy_backward(begin() + index, end(), end());
            items_[index] = value;
            ++size_;
        }
        return begin() + index;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos <= end() && pos >= begin());
        auto index = pos - begin();
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_*2;
            ArrayPtr<Type> new_items (new_capacity);
            std::move(begin(), begin() + index, new_items.Get());
            new_items[index] = std::move(value);
            std::move(begin() + index, end(), new_items.Get() + index + 1);
            ++ size_;
            capacity_ = new_capacity;
            items_.swap(new_items);
        } else {
            std::move_backward(begin() + index, end(), end());
            items_[index] = std::move(value);
            ++size_;
        }
        return begin() + index;
    }

    void PopBack() noexcept {
        assert(!IsEmpty());
        --size_;
    }

    Iterator Erase(ConstIterator pos) {
        assert(!IsEmpty());
        assert(pos < end() && pos >= begin());
        auto index = pos - begin();
        std::move(begin() + index + 1, end(), begin() + index);
        --size_;
        return Iterator(begin() + index);
    }

    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> temp_items(new_capacity);
            std::copy(begin(), end(), temp_items.Get());
            items_.swap(temp_items);
            capacity_ = new_capacity;
        }
    }

    Iterator begin() noexcept {
        return items_.Get();
    }

    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }
private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() == rhs.GetSize()) {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    } else {
        return false;
    }
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}
