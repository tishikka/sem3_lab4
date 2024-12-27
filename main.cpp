#include <iostream>
#include <map>
#include <chrono>
#include "lib.h"
#include <memory>
#include <cassert>
#include <cstring>

//аллокатор
template <typename T, std::size_t ChunkSize = 10>
class MyAllocator {
public:
    using value_type = T;

    MyAllocator() : pool(nullptr), current(nullptr), pool_end(nullptr) {}

    template <typename U>
    MyAllocator(const MyAllocator<U>&) noexcept : MyAllocator() {}

    ~MyAllocator() {
        deallocate_all();
    }

    T* allocate(std::size_t n) {
        if (n > ChunkSize) {
            throw std::bad_alloc();
        }

        if (!pool || current + n > pool_end) {
            expand_pool();
        }

        T* result = current;
        current += n;
        return result;
    }

    void deallocate(T* p, std::size_t n) noexcept {
        for (std::size_t i = 0; i < n; ++i) {
            p[i].~T();
        }
    }

    void deallocate_all() noexcept {
        if (pool) {
            ::operator delete(pool);
            pool = nullptr;
            current = nullptr;
            pool_end = nullptr;
        }
    }

    template <typename U>
    struct rebind {
        using other = MyAllocator<U, ChunkSize>;
    };

    bool operator==(const MyAllocator&) const noexcept { return true; }
    bool operator!=(const MyAllocator&) const noexcept { return false; }

private:
    void expand_pool() {
        std::size_t size = ChunkSize * sizeof(T);
        pool = static_cast<T*>(::operator new(size));
        current = pool;
        pool_end = reinterpret_cast<T*>(reinterpret_cast<char*>(pool) + size);
    }

    T* pool;
    T* current;
    T* pool_end;
};


template <typename T>
class my_alocator{
    public:
    using value_type = T;
    my_alocator() = default;
    T* allocate(std::size_t n){
        if(n > 100){
            throw std::bad_alloc();
        }
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    void deallocate(T* p, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) {
            p[i].~T();
        }
    }
    T* reallocate(T* p, std::size_t old_size, std::size_t new_size) {
        if (new_size > 100) {
            throw std::bad_alloc();
        }

        T* new_ptr = allocate(new_size);
        std::size_t elements_to_copy = std::min(old_size, new_size);
        std::copy(p, p + elements_to_copy, new_ptr); 
        deallocate(p, old_size); 

        return new_ptr;
    }
}; 

//контейнер
template <typename T, typename Allocator = my_alocator<T>>
class MyContainer {
public:
    using allocator_type = Allocator;
    using value_type = T;
    using size_type = std::size_t;

    MyContainer() : alloc(), data(nullptr), count(0), capacity(0) {}

    ~MyContainer() {
        clear();
        alloc.deallocate(data, capacity);
    }

    void push_back(const T& value) {
        if (count == capacity) {
            expand();
        }
        new (data + count) T(value);
        ++count;
    }

    size_type size() const noexcept {
        return count;
    }

    bool empty() const noexcept {
        return count == 0;
    }

    void clear() noexcept {
        for (size_type i = 0; i < count; ++i) {
            data[i].~T();
        }
        count = 0; 
    }

    class iterator {
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        iterator(pointer ptr) : ptr(ptr) {}

        reference operator*() const { return *ptr; }
        pointer operator->() { return ptr; }
        iterator& operator++() {
            ++ptr;
            return *this;
        }
        iterator operator++(int) {
            iterator temp = *this;
            ++ptr;
            return temp;
        }
        bool operator==(const iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const iterator& other) const { return ptr != other.ptr; }

    private:
        pointer ptr;
    };

    iterator begin() { return iterator(data); }
    iterator end() { return iterator(data + count); }

private:
    void expand() {
        size_type new_capacity = capacity == 0 ? 1 : 2 * capacity;
        T* new_data = alloc.allocate(new_capacity);
        for (size_type i = 0; i < count; ++i) {
            new (new_data + i) T(std::move(data[i]));
            data[i].~T(); 
        }
        alloc.deallocate(data, capacity);
        data = new_data;
        capacity = new_capacity;
    }

    allocator_type alloc;
    T* data;
    size_type count;
    size_type capacity;
};

int main() {
    // 1. Создание экземпляра std::map<int, int>
    std::map<int, int> my_map2;

    // 2. Заполнение 10 элементами, где ключ – это число от 0 до 9, а значение – факториал ключа
    for (int i = 0; i < 10; i++) {
        my_map2[i] = factarial(i);
    }

    // 3. Создание экземпляра std::map<int, int> с новым аллокатором
    std::map<int, int, std::less<int>, MyAllocator<std::pair<int, int>>> my_map1;

    // 4. Заполнение 10 элементами, где ключ – это число от 0 до 9, а значение – факториал ключа
    for (int i = 0; i < 10; i++) {
        my_map1[i] = factarial(i);
    }

    // 5. Вывод на экран всех значений (ключ и значение разделены пробелом)
    for (const auto& pair : my_map1) {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }

    // 6. Создание экземпляра своего контейнера для хранения значений типа int
    MyContainer<int> container;

    // 7. Заполнение 10 элементами от 0 до 9
    for (int i = 0; i < 10; ++i) {
        container.push_back(i);
    }

    // 8. Создание экземпляра своего контейнера для хранения значений типа int с новым аллокатором
    MyContainer<int> container_with_allocator;

    // 9. Заполнение 10 элементами от 0 до 9
    for (int i = 0; i < 10; ++i) {
        container_with_allocator.push_back(i);
    }

    // 10. Вывод на экран всех значений, хранящихся в контейнере
    for (auto it = container_with_allocator.begin(); it != container_with_allocator.end(); ++it) {
        std::cout << *it << " ";
    }
        return 0;
}