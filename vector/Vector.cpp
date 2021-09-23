#include <algorithm>
using std::swap;
using std::rand;
using Rank = int;
template <typename T> class Vector {

protected:
    static constexpr int DEFAULT_CAPACITY = 3;
    Rank _size = 0; int _capacity = DEFAULT_CAPACITY; T* _elem; // 大小, 容量, 数据区
    void copyFrom(T const *A, Rank lo, Rank hi); // 复制数组区间A[lo, hi)
    void expand(); // 空间不足时扩容
    void shrink(); // 装填因子过小时压缩
    void bubble_sort(Rank lo, Rank hi); // 冒泡排序
    Rank max(Rank lo, Rank hi);
    void selection_sort(Rank lo, Rank hi);  // 选择排序
    void merge(Rank lo, Rank hi); // 归并
    void merge_sort(Rank lo, Rank hi); // 归并排序
    Rank parition(Rank lo, Rank hi); // 轴点构造
    void quick_sort(Rank lo, Rank hi); // 快速排序
    void heap_sort(Rank to, Rank hi); // 堆排序

public:

    // 构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0)
    { _elem = new T[_capacity]; for (_size = 0; _size < s; _elem[_size++] = v); } // 首先创建一个数组, 然后将它赋值

    Vector(Vector<T> && v) {
        _elem = v._elem;
        _size = v._size;
        _capacity = v._capacity;
        v._elem = nullptr;
    }
    Vector(T const* A, Rank lo, Rank hi) { copyFrom(A, lo, hi); } 
    Vector(T const* A, Rank n) { copyFrom(A, 0, n); }
    Vector(Vector<T> const & V, Rank lo, Rank hi) { copyFrom(V._elem, lo, hi); }
    Vector(Vector<T> const & V) { copyFrom(V._elem, 0, V._size)}

    // 析构函数
    ~Vector() { delete [] _elem; }
    
    // 只读接口
    Rank size() const { return _size; }
    bool empty() const { return !_size; }
    int disordered() const;
    Rank find(T const& e) const { return find(e, 0, _size); } // 无序查找
    Rank find(T const& e, Rank lo, Rank hi) const;
    Rank search(T const & e) const // 有序向量查找
    { return (0 >= _size) ? -1 : search(e, 0, _size); }

    Rank search(T const & e, Rank lo, Rank hi) const;

    // 可写接口
    T& operator[](Rank r); // 用于非const对象
    const T& operator[](Rank r) const; // 用于const对象
    Vector<T>& operator=(Vector<T> const &);
    T remove(Rank r);
    int remove(int lo, int hi);
    Rank insert(Rank r, T const & e);
    Rank insert(T const & e) { return insert(_size, e); }
    void sort(Rank lo, Rank hi);
    void sort() { sort(0, _size); }
    void unsort(Rank lo, Rank hi);
    void unsort() { unsort(0, _size); }
    void mergeSort(Rank lo, Rank hi);
    void merge(Rank lo, Rank mi, Rank hi);
    int deduplicate(); // 无序去重
    int uniquify(); // 有序去重
    void push_back(T const & e) { return insert(_size, e); }
    void pop_back() { remove(_size - 1); }

    // 遍历
    void traverse(void (*)(T&)); // 遍历, 使用函数指针
    template <typename VST> void traverse(VST&); // 遍历, 使用函数对象
};


/**
 * 为了实现动态扩容, 将数组的容量复制为之前的两倍
 */
template <typename T> void Vector<T>::copyFrom(T const *A, Rank lo, Rank hi) {
    _elem = new T[_capacity = 2 * (hi - lo)]; _size = 0;
    while (lo < hi) {
        _elem[_size++] = A[lo++];
    }
}


/**
 * 实现赋值构造函数
 */
template <typename T> Vector<T>& Vector<T>::operator=(Vector<T> const & v) {
    if (this == &v) return *this;
    if (_elem) delete [] _elem;
    copyFrom(v._elem, 0, v.size());
    return *this;
}

/**
 * 实现动态扩容
 */
template <typename T> void Vector<T>::expand() {
    if (_size < _capacity) return; // 如果空间尚余, 不处理
    if (_capacity < DEFAULT_CAPACITY) _capacity = DEFAULT_CAPACITY; // 不低于最低容量
    T *old = _elem; _elem = new T[_capacity <<= 1]; // 容量翻倍
    for (int i = 0; i < _size; i++)
        _elem[i] = old[i] // 复制原向量空间内容到新的地方
    delete [] old;
}

/**
 * 实现动态缩容
 */
template <typename T> void Vector<T>::shrink() {
    if (_capacity < DEFAULT_CAPACITY << 1) return; // 达不到收缩的目的
    if (_size << 2 > _capacity) return; // 以25%作为边界
    T *old = _elem; _elem = new T[_capacity >>= 1]; // 容量减半
    for (int i = 0; i < _size; i++) _elem[i] = old[i];
    delete [] old;
}

/**
 * 重载[]运算符进行元素引用 非const类型
 */
template <typename T> T & Vector<T>::operator[](Rank r) 
{ return _elem[r]; }


/**
 * 重载[]运算符进行元素引用 const类型
 */
template <typename T> const T & Vector<T>::operator[](Rank r) const
{ return _elem[r]; }


/**
 * 置乱器, 采用随机交换的方式
 */
template <typename T> void permulate(Vector<T> & v) {
    for (int i = v.size(); i > 0; i--) {
        swap(v[i - 1], v[rand() % i]); // v[i - 1] 和 v[0, i)中元素随意交换
    }
}

/**
 * 区间置乱接口
 */
template <typename T> void Vector<T>::unsort(Rank lo, Rank hi) {
    T *v = _elem + lo; // 相当于将lo处看作起始地址
    for (int i = hi - lo, i > 0; i--)
        swap(v[i - 1], v[rand() % i]);
}

template <typename T> static bool lt(T *a, T *b) { lt(*a, *b); }
template <typename T> static bool lt(T &a, T &b) { return a < b; }
template <typename T> static bool eq(T *a, T *b) { return eq(*a, *b); }
template <typename T> static bool eq(T &a, T &b) { return a == b; }


/**
 * 实现 逻辑==
 */
template <typename T> static 
bool operator==(const Vector<T> v1, const Vector<T> v2) {
    if (v1.size() != v2.size()) return false;
    for (int i = 0; i < v1.size(); i++)
        if (v1[i] != v2[i]) return false;
    return true;
}

/**
 * 实现 逻辑 !=
 */
template <typename T> static
bool operator!=(const Vector<T> v1, const Vector<T> v2) {
    return !(v1 == v2);
}

/**
 * 无序向量的顺序查找
 */
template <typename T> 
Rank Vector<T>::find(T const & e, Rank lo, Rank hi) const {
    while (lo < hi-- && e != _elem[hi]);
    return hi;
    // 这段代码写的十分简洁好看
    // 1. 首先返回R大的元素, 这样从尾到头比较就无须花费精力去比较舍弃等
    // 2. 不成功时返回lo - 1, 这点做的很巧妙啊
}

/**
 * 插入算法
 */
template <typename T> 
Rank Vector<T>::insert(Rank r, T const & e) {
    expand(); // 动态扩容
    for (int i = _size; i > r; i--) _elem[i] = _elem[i - 1]; // 元素后移
    _elem[r] = e; _size++; // 更新
    return r; // 返回R
}

/**
 * 删除区间 元素
 */
template <typename T> int Vector<T>::remove(Rank lo, Rank hi) {
    if (lo == hi) return 0; // 删除一个位置的元素单独进行退化处理
    while (hi < _size) _elem[lo++] = _elem[hi++]; // [lo, hi, hi + 1) 进行赋值
    _size = lo; // 更新规模, 直接丢弃尾部[lo, _size = hi], 因为lo总是指向元素尾部, 因此它可以表示尺寸;
    shrink(); // 进行收缩
    return hi - lo;  // 返回删除的元素数目

}

/**
 * 删除某一个位置的元素
 */
template <typename T> T Vector<T>::remove(Rank r) {
    T e = _elem[r]; // 备份元素
    remove(r, r + 1); // 删除处于位置为r的元素
    return e; // 返回删除的值
}


/**
 * 唯一化, 
 * 思路是不断增加计数i, 
 * 每次循环中在[0-i)的范围内删除ele[i]相同的元素
 * 如此一来每次循环中[0 i)中的元素都是独一无二的
 */
template <typename T> int Vector<T>::deduplicate() {
    int old_size = _size; // 记录原始的尺寸
    Rank i = 1; // 从下标为1开始
    while (i < _size) { // 
        (find(_elem[i], 0, i) < 0) ? // 在0 - i范围内寻找和自己相同的元素并进行移除
        i++ : remove(i); // 如果有相同的元素进行删除, 否则考查其后继
    }
    return old_size - size; // 返回变化量
}

/**
 * 遍历元素, 并提供接口进行操作
 */
template <typename T> void Vector<T>::traverse(void (*visit)(T &)) // 利用函数指针遍历
{ for (int i = 0; i < _size; i++) visit(_elem[i]); }

template <typename T> template <typename VST>
void Vector<T>::traverse(VST& visit) // 利用函数对象进行遍历
{ for (int i = 0; i < _size; i++) visit(_elem[i]); }


/**
 * 返回向量中逆序相邻元素对的总数
 */
template <typename T> int Vector<T>::disordered() const {
    int n = 0;
    for (int i = 1; i < _size; i++)
        if (_elem[i - 1] > _elem[i]) n++;
    return m;
}

// /**
//  * 唯一化, 低效版
//  */
// template <typename T> int Vector<T>::uniquify() {
//     // 有序元素, 相同的元素定是前后关系,
//     int old = _size, i = 1;
//     while (i < _size)
//         _elem[i - 1] == _elem[i] ? remove(i) : i++;
//     return old - _size;

// }

/**
 * 唯一化, 高效版
 */
template <typename T> int Vector<T>::uniquify() {
    Rank i = 0, j = 0; // i指向当前可以被取代的位置
    while (++j < _size)
        // 如果元素保持相同, 就不进行移动

        if (_elem[i] != _elem[j]) // 如果元素不同, 就进行移动赋值
            _elem[++i] = _elem[j]; // 进行移动
    _size = ++i; shrink(); // 直接截取尾部多余元素, 收缩
    return j - i;
}

int uniquify(Vector<int> v) {
    auto ineft = [&]() -> int {
        int old_size = v.size(), i = 1;
        while (i < v.size())
            v[i - 1] == v[i] ? v.remove(i) : i++;
        return old_size - v.size();
    };
}

// template <typename T>
// static Rank binSearch(T *A, T const & e, Rank lo, Rank hi) {
//     int middle;
//     // [] 采用都闭得格式
//     while (lo <= hi) {
//         middle = (lo + hi) / 2
//         if (A[middle] > e) hi = middle - 1;
//         else if (A[middle] < e) lo = middle + 1;
//         else return middle;
//     }
//     return -1;
// }

// template <typename T>
// static Rank binSearch(T *A, T const & e, Rank lo, Rank hi) {
//     int middle;
//     // [) 采用都闭得格式
//     while (lo < hi) {
//         middle = (lo + hi) / 2
//         if (A[middle] > e) hi = middle;
//         else if (A[middle] < e) lo = middle + 1;
//         else return middle;
//     }
//     return -1;
// }

/**
 * 采用一段寻找的方式的二分查找, 必须要明确的是我们最后在哪个位置上进行比较值
 * 如果直接存为[]肯定会出问题, 所以最好的方式是存为[)
 * 因此最好的二分查找采用[)的方式
 */
template <typename T>
static Rank binSearch(T *A, T const & e, Rank lo, Rank hi) {
    int middle;
    while (lo < hi - 1) {
        middle = (lo + hi) / 2;
        if (A[middle] > e) hi = middle;
        else  lo = middle;
    }
    return (e == A[lo]) ? lo : -1;
}



template <typename T>
static Rank fibSearch(T *A, T const & e, Rank lo, Rank hi) {
    
}

/**
 * 二分查找C版本: 在有序向量的区间[lo, hi)内查找e, 0 <= lo <= hi <= _size
 * [........|.........]
 */
template <typename T> static Rank binSearch(T *A, T const & e, Rank lo, Rank hi) {
    while (lo < hi) { // 每步查找仅作一次判断, 有两个分支
        Rank mi = (lo + hi) >> 1; // 以中点为轴点
        (e < A[mi]) ? hi = mi : lo = mi + 1; // 经比较后确定深入[lo, mi), (mi, hi)
    }
    return --lo; // 循环结束时, lo为大于e的元素的最小的秩,
}


/**
 * 有序向量的查找
 */
template <typename T>
Rank Vector<T>::search(T const & e, Rank lo, Rank hi) const {
    binSearch(_elem, e, lo, hi)
}

/**
 * 排序算法的接口[lo, hi)
 */
template <typename T> void Vector<T>::sort(Rank lo, Rank hi) {
    switch (rand() % 5) {
        case 1: bubble_sort(lo, hi); break;
        case 2: selection_sort(lo, hi); break;
        case 3: merge_sort(lo, hi); break;
        case 4: heap_sort(lo, hi); break;
        default: quick_sort(lo, hi); break;
    }
}

/**
 * 冒泡排序
 */
template <typename T>
void Vector<T>::bubble_sort(Rank lo, Rank hi) {
    auto bubble = [&]() -> bool {
        for (int i = lo; i < hi - 2; i++)
            for (int j = hi - 1; j > lo; j--)
                if (_elem[j] < _elem[j - 1])
                    swap(_elem[j], _elem[j - 1]);
    };
    bubble();
}

/**
 * 选择排序
 */
template <typename T> void
Vector<T>::selection_sort(Rank lo, Rank hi) {
    for (int i = lo; i < hi - 1; i++) {
        T mini = i;
        for (int j = lo + 1; j < hi; j++)
            if (_elem[mini] > _elem[j]) mini = j;
        swap(_elem[mini], _elem[i]);
    }
}

/**
 * 划分
 */
template <typename T> static
Vector<T>  divide(Vector<T> & v) {
    Vector<T> d;
    int size = (v.size() + 1) / 2;
    for (int i = size; i < v.size(); i++)
        d.push_back(v[i])
    
    for (int i = 0; i < d.size(); i++)
        v.pop_back();
    rteurn d;
}

/**
 * 合并
 */
template <typename T> static
void combine(Vector<T> & v, Vector<T> & d) {
    Vector<T> combined;
    int i = 0, j = 0;
    while (i < v.size() && j < v.size()) {
        T t;
        if (v[i] < d[j]) {
            t = v[i]; i++;
        } else {
            t = d[j]; j++;
        }
        combined.push_back(t);
    }
    while (i < v.size()) combined.push_back(t);
    while (j < d.size()) combined.push_back(t);
    v = std::move(combined);
}



/**
 * 归并排序
 */
template <typename T> static
void merge_sort(Vector<T> & v) {
    if (v.size() > 1) {
        Vector<T> div = divide(v);
        merge_sort(div);
        merge_sort(v);
        combine(v, div)
    }
}

/**
 * 基于下标的merge_sort
 */
template <typename T> void
Vector<T>::mergeSort(Rank lo, Rank hi) {
    if (hi > lo) {
        int middle = (lo + hi) >> 1;
        mergeSort(lo, mi); mergeSort(mi, hi); merge(lo, mi, hi);
    }
}

/**
 * 归并
 */
template <typename T> void 
Vector<T>::merge(Rank lo, Rank mi, Rank hi) {
    T* A = new T[hi - lo];
    int i = 0, middle = mi; T t;
    while (lo < middle && mi < hi) {
        if (_elem[lo] < _elem[mi]) {
            t = _elem[lo]; lo++;
        } else {
            t = _elem[mi]; mi++;
        }
        A[i++] = t;
    }
    while (lo < middle) A[i++] = _elem[lo++];
    while (mi < hi) A[i++] = _elem[mi++];
    for (int j = lo; j < hi; j++)
        _elem[j] = A[j - lo];
    delete [] A;
}
