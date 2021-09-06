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

    Vector(int c, int s, T v = 0)
    { _elem = new T[_capacity]; for (_size)} 



public:




};