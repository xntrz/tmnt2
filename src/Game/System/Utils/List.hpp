#pragma once


class CListNodeDefaultTag { };


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
    using value_type        = T;
    using reference         = T&;
    using const_reference   = const T&;
    using pointer           = T*;
    using const_pointer     = const T*;

public:
#if (__cplusplus < 201703)
    template<class Ty>
    class iterator_base : public std::iterator<std::bidirectional_iterator_tag, T>
#else
    template<class Ty>
    class iterator_base
#endif        
    {
    public:
#if (__cplusplus < 201703)
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
#else 
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
#endif  
        
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

    
    using iterator = iterator_base<CListNode<T, tag>>;
    using const_iterator = iterator_base<const CListNode<T, tag>>;


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
        this->next = other.next;
        this->prev = other.prev;
        this->data = other.data;
        return *this;
    };


    inline void clear(void)
    {
		this->data = nullptr;
		this->next = this->prev = this;
    };


    inline bool empty(void) const
    {
		return (this->prev == this);
    };


    inline pointer front(void)
    {
		if (empty())
			return nullptr;
		else
			return static_cast<T*>(this->next);
    };


    inline pointer front(void) const
    {
		if (empty())
			return nullptr;
		else
			return static_cast<T*>(this->next);
    };


    inline pointer back(void)
    {
		if (empty())
			return nullptr;
		else
			return static_cast<T*>(this->prev);
    };


    inline pointer back(void) const
    {
		if (empty())
			return nullptr;
		else
			return static_cast<T*>(this->prev);
    };


    inline iterator begin(void)
    {
		return iterator(this, this->next);
    };


    inline iterator end(void)
    {
        return iterator(this, this);
    };


    inline const_iterator begin(void) const
    {
		return const_iterator(this, this->next);
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
		insert(iterator(this, this->next), node);
    };


    inline void push_back(iterator it)
    {
        push_back(it.node());
    };


    inline void push_back(CListNode<T, tag>* node)
    {
		insert(iterator(this, this->prev->next), node);
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
		erase(this->next);
    };


    inline void pop_back(void)
    {
		erase(this->prev);
    };


    inline iterator erase(CListNode<T, tag>* node)
    {
        return erase(iterator(this, node));
    };


    inline iterator erase(iterator it)
    {
        ASSERT(!empty());
        ASSERT(!!it);

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

		this->prev->next = list->next;
		list->next->prev = this->prev;

        list->prev->next = this;
		this->prev = list->prev;

        list->clear();
    };


    inline void swap(CList<T, tag>* list)
    {
        if (empty() || list->empty())
            return;

		this->next->prev = list;
		this->prev->next = list;

        list->next->prev = this;
        list->prev->next = this;

        CList<T, tag> temp = *list;
        *list = *this;
        *this = temp;
    };


    inline bool contains(const CListNode<T, tag>* node) const
    {
        return std::any_of(begin(), end(), [&](CListNode<T, tag>& n) {
            return (node == &n);
        });
    };


    inline pointer search(int32 no)
    {
#ifdef _DEBUG
        int32 numNodes = static_cast<int32>(std::distance(begin(), end()));

        ASSERT(no >= 0);
        ASSERT(no < numNodes);
#endif /* _DEBUG */

        iterator it = begin();
        std::advance(it, no);
		return &(*it);
    };
};