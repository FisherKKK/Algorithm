using Rank = int;
template <typename T> class Vector {

protected:
    static constexpr int DEFAULT_CAPACITY = 3;
    Rank _size = 0; int _capacity = DEFAULT_CAPACITY; T* _elem; // 大小, 容量, 数据区
    void copyFrom(T const *A, Rank lo, Rank hi); // 复制数组区间A[lo, hi)
    void expand(); // 空间不足时扩容
    void shrink(); // 装填因子过小时压缩
    bool bubble(Rank lo, Rank hi);
    void bubble_sort(Rank lo, Rank hi); // 冒泡排序
    Rank max(Rank lo, Rank hi);
    void selection_sort(Rank lo, Rank hi);  // 选择排序
    void merge(Rank lo, Rank mi, Rank hi); // 归并
    void merge_sort(Rank lo, Rank hi); // 归并排序
    Rank parition(Rank lo, Rank hi); // 轴点构造
    void quick_sort(Rank lo, Rank hi); // 快速排序
    void heap_sort(Rank to, Rank hi); // 堆排序

public:

    // 构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0)
    { _elem = new T[_capacity]; for (_size = 0; _size < s; _elem[_size++] = v); } // 首先创建一个数组, 然后将它赋值

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
    Rank search(T const & e) const; // 有序向量查找
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
    int deduplicate(); // 无序去重
    int uniquify(); // 有序去重

    // 遍历
    void traverse(void (*)(T&)); // 遍历, 使用函数指针
    template <typename VST> void traverse(VST&); // 遍历, 使用函数对象
};


/**
 * 为了实现动态扩容, 将数组的容量复制为之前的两倍
 */
template <typename T>
void Vector<T>::copyFrom(T const *A, Rank lo, Rank hi) {
    _elem = new T[_capacity = 2 * (hi - lo)]; _size = 0;
    while (lo < hi) {
        _elem[_size++] = A[lo++];
    }
}


/**
 * 实现赋值构造函数
 */
template <typename T>
Vector<T>& Vector<T>::operator=(Vector<T> const & v) {
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