#pragma once


class CListNodeDefaultTag
{
public:
    
};


template<class T, class tag = CListNodeDefaultTag>
class CListNode
{
public:
    CListNode(const CListNode&) = delete;
    const CListNode& operator=(const CListNode&) = delete;


    inline CListNode(void)
    : next(nullptr)
    , prev(nullptr)
    , data((T*)this)
    {
        ;
    };


    inline bool is_linked(void) const
    {
        return (next && prev);
    };


    inline void unlink(void)
    {
        prev->next = next;
        next->prev = prev;

        next = prev = nullptr;
    };


    CListNode<T, tag>* next;
    CListNode<T, tag>* prev;
    T* data;
};


template<class T, class tag = CListNodeDefaultTag>
class CList : public CListNode<T, tag>
{
public:
    typedef T           value_type;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef T*          pointer;
    typedef const T*    const_pointer;


    template<class Ty>
    class iterator_base : public std::iterator<std::bidirectional_iterator_tag, T>
    {
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;

        
        inline iterator_base(void)
            : m_list(nullptr)
            , m_node(nullptr)
        {
            ;
        };


        inline iterator_base(Ty* list, Ty* node)
            : m_list(list)
            , m_node(node)
        {
            ;
        };


        inline iterator_base(const iterator_base& it)
            : m_list(it.m_list)
            , m_node(it.m_node)
        {
            ;
        };


        inline bool is_end(void)
        {
            return (m_node == m_list);
        };


        inline Ty* node(void)
        {
            return m_node;
        };


        inline iterator_base prev(void)
        {
            return iterator_base(m_list, m_node->prev);
        };


        inline iterator_base next(void)
        {
            return iterator_base(m_list, m_node->next);
        };


        inline bool operator==(const iterator_base& it) const
        {
            return ((m_list == it.m_list) && (m_node == it.m_node));
        };


        inline bool operator!=(const iterator_base& it) const
        {
            return ((m_list == it.m_list) && (m_node != it.m_node));
        };


        inline iterator_base operator++(int)
        {
            auto self = *this;
            *this = next();
            return self;
        };


        inline iterator_base operator--(int)
        {
            auto self = *this;
            *this = prev();
            return self;
        };


        inline iterator_base& operator++(void)
        {
            *this = next();
            return *this;
        };


        inline iterator_base& operator--(void)
        {
            *this = prev();
            return *this;
        };


        inline explicit operator bool(void)
        {
            return (!is_end());
        };


        inline reference operator*(void) const
        {
            return *m_node->data;
        };


        inline pointer operator->(void) const
        {
            return m_node->data;
        };


    protected:
        Ty* m_node;
        Ty* m_list;
    };


    typedef iterator_base<CListNode<T, tag>> iterator;
    typedef iterator_base<const CListNode<T, tag>> const_iterator;


public:
    inline CList(void)
    {
        clear();
    };


    inline ~CList(void)
    {
        ;
    };


    inline CList(const CList<T, tag>& other)
    {
        *this = other;
    };


    inline const CList<T, tag>& operator=(const CList<T, tag>& other)
    {
        next = other.next;
        prev = other.prev;
        data = other.data;
        return *this;
    };


    inline void clear(void)
    {
        data = nullptr;
        next = prev = this;
    };


    inline bool empty(void) const
    {
        return (prev == this);
    };


    inline pointer front(void)
    {
		if (empty())
			return nullptr;
		else
			return static_cast<T*>(next);
    };


    inline pointer front(void) const
    {
		if (empty())
			return nullptr;
		else
			return static_cast<T*>(next);
    };


    inline pointer back(void)
    {
		if (empty())
			return nullptr;
		else
			return static_cast<T*>(prev);
    };


    inline pointer back(void) const
    {
		if (empty())
			return nullptr;
		else
			return static_cast<T*>(prev);
    };


    inline iterator begin(void)
    {
        return iterator(this, next);
    };


    inline iterator end(void)
    {
        return iterator(this, this);
    };


    inline const_iterator begin(void) const
    {
        return const_iterator(this, next);
    };


    inline const_iterator end(void) const
    {
        return const_iterator(this, this);
    };


    inline void push_front(iterator it)
    {
        push_front(it.node());
    };


    inline void push_front(CListNode<T, tag>* node)
    {
        insert(iterator(this, next), node);
    };


    inline void push_back(iterator it)
    {
        push_back(it.node());
    };


    inline void push_back(CListNode<T, tag>* node)
    {
        insert(iterator(this, prev->next), node);
    };


    inline iterator insert(iterator pos, CListNode<T, tag>* node)
    {
        CListNode<T, tag>* pos_node = pos.node();
        iterator result(this, node);

        node->prev = pos_node->prev;
        node->next = pos_node;

        pos_node->prev->next = node;
        pos_node->prev = node;

        return result;
    };


    inline void pop_front(void)
    {
        erase(next);
    };


    inline void pop_back(void)
    {
        erase(prev);
    };


    inline iterator erase(CListNode<T, tag>* node)
    {
        return erase(iterator(this, node));
    };


    inline iterator erase(iterator it)
    {
        if (empty() || !it)
            return it;

        CListNode<T, tag>* node = it.node();
        iterator result = ++it;

        node->prev->next = node->next;
        node->next->prev = node->prev;

        node->prev = node->next = nullptr;

        return result;
    };


    inline void merge(CList<T, tag>* list)
    {
        if (list->empty())
            return;

        prev->next = list->next;
        list->next->prev = prev;

        list->prev->next = this;
        prev = list->prev;

        list->clear();
    };


    inline void swap(CList<T, tag>* list)
    {
        if (empty() || list->empty())
            return;

        next->prev = list;
        prev->next = list;

        list->next->prev = this;
        list->prev->next = this;

        CList<T, tag> temp = *list;
        *list = *this;
        *this = temp;
    };


    inline bool contains(const CListNode<T, tag>* node) const
    {
		return std::any_of(begin(), end(), [&](CListNode<T, tag>& n) { return (node == &n); });    
    };


    inline pointer search(int32 no)
    {
        ASSERT(no >= 0 && no < std::distance(begin(), end()));
        
        iterator it = begin();
        std::advance(it, no);
		return &(*it);
    };
};