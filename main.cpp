#include <iostream>
#include <forward_list>
#include <vector>

class alloc {
public:
    alloc() : mem_free(0), mem_offset(nullptr) {};
    ~alloc() = default;
    void* allocate(std::size_t n) { // Выделение памяти
        if (n > mem_free) {
            buf.emplace_front(100 > n ? 100 : n); // 100.000 байт или n байт (что больше)
            mem_free = buf.front().size();
            mem_offset = &buf.front().front();
        }
        char *cur_p = mem_offset; // Указатель на начало выделенной памяти
        mem_offset += n;
        mem_free -= n;
        return cur_p;
    };
    void dellocate(void *p, std::size_t n) { // Удаление последнего добавленного объекта (если это он)
        if ((char*)p + n == mem_offset) {
            mem_offset -= n;
            mem_free += n;
        }
    }

protected:
    std::forward_list<std::vector<char>> buf;
    std::size_t mem_free;
    char* mem_offset;
};

template <class T>
class MyAllocator {
public:
    using value_type = T;

    MyAllocator(alloc &A) : A(&A) {};
    template<class U>
            MyAllocator(MyAllocator<U> &elem) : A(elem.A) {};

    T* allocate(std::size_t n) {
        return (T*)A->allocate(n * sizeof(T)); // Возвращаем void* и делаем его T*
    }

    void deallocate(void *p, size_t n) {
        A->dellocate(p, n * sizeof(T));
    }

    alloc *A;
};


template<class T>
using T_vector = std::vector<T, MyAllocator<T>>;

int main() {
    alloc al;
    MyAllocator<char> M_al(al);
    T_vector<int> v_int({1, 2, 3}, M_al);
    T_vector<int> v_int2({4, 5, 6}, M_al);
    T_vector<int> v_int3({7, 8, 9}, M_al);
    int* ar = (int*)M_al.allocate(10 * sizeof(int));
    for (auto it = v_int.begin(); it != v_int3.end(); it++)
        std::cout << *it << std::endl;
    for (int i = 0; i < 10; i++)
        ar[i] = i;
    for (int i = 0; i < 10; i++)
        std::cout << ar[i] << ' ';
    return 0;
}
