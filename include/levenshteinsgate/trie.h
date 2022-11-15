#pragma once

#include <algorithm>
#include <cassert>
#include <string>
#include <tuple>
#include <vector>
#include <utility>

#ifdef _MSC_VER
#define __inline__ __forceinline
#endif


namespace levenshteinsgate {


inline int Log(unsigned int v) // 32-bit word to find the log of
{
    static const signed char LogTable256[256] =
    {
    #define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
        - 1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
        LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
        LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
    };

    unsigned int t, tt; // temporaries

    if (tt = v >> 16)
    {
      return (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
    }
      
    return (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
}


struct Plex     // Similar to MFC CPlex
// warning variable length structure
{
    Plex* pNext;
    int dwReserved[1];    // align on 8 byte boundary

    void* data() { return this+1; }

    // like 'calloc' but no zero fill
    // may throw memory exceptions
    static Plex* Create(Plex*& pHead, size_t nMax, size_t cbElement)
    {
        assert(nMax > 0 && cbElement > 0);
        Plex* p = (Plex*) operator new(sizeof(Plex) + nMax * cbElement);
                // may throw exception
        p->pNext = pHead;
        pHead = p;  // change head (adds in reverse order for simplicity)
        return p;
    }

    void FreeDataChain()       // free this one and links
    {
        Plex* p = this;
        while (p != 0)
        {
            void* bytes = p;
            p = p->pNext;
            operator delete(bytes);
        }
    }
};

typedef unsigned int UINT;

class FixedAlloc    // Similar to MFC CFixedAlloc
{
// Constructors
public:
    FixedAlloc(UINT nAllocSize, UINT nBlockSize = 64);

// Attributes
    UINT GetAllocSize() { return m_nAllocSize; }

// Operations
public:
    void* Alloc();  // return a chunk of memory of nAllocSize
    void Free(void* p); // free chunk of memory returned from Alloc
    void FreeAll(); // free everything allocated from this allocator

// Implementation
public:
    ~FixedAlloc();

protected:
    struct CNode
    {
        CNode* pNext;	// only valid when in free list
    };

    UINT m_nAllocSize;	// size of each block from Alloc
    UINT m_nBlockSize;	// number of blocks to get at a time
    Plex* m_pBlocks;	// linked list of blocks (is nBlocks*nAllocSize)
    CNode* m_pNodeFree;	// first free node (0 if no free nodes)
};

FixedAlloc::FixedAlloc(UINT nAllocSize, UINT nBlockSize)
{
    assert(nAllocSize >= sizeof(CNode));
    assert(nBlockSize > 1);

    if (nAllocSize < sizeof(CNode))
        nAllocSize = sizeof(CNode);
    if (nBlockSize <= 1)
        nBlockSize = 64;

    m_nAllocSize = nAllocSize;
    m_nBlockSize = nBlockSize;
    m_pNodeFree = 0;
    m_pBlocks = 0;
}

FixedAlloc::~FixedAlloc()
{
    FreeAll();
}

void FixedAlloc::FreeAll()
{
    m_pBlocks->FreeDataChain();
    m_pBlocks = 0;
    m_pNodeFree = 0;
}

void* FixedAlloc::Alloc()
{
    if (m_pNodeFree == 0)
    {
        // add another block
        Plex* pNewBlock = Plex::Create(m_pBlocks, m_nBlockSize, m_nAllocSize);

        // chain them into free list
        // free in reverse order to make it easier to debug
        char* pData = (char*) pNewBlock->data() + m_nAllocSize * (m_nBlockSize - 1);
        for (int i = m_nBlockSize; i > 0; i--, pData -= m_nAllocSize)
        {
            CNode* pNode = (CNode*) pData;
            pNode->pNext = m_pNodeFree;
            m_pNodeFree = pNode;
        }
    }
    assert(m_pNodeFree != 0);  // we must have something

    // remove the first available node from the free list
    void* pNode = m_pNodeFree;
    m_pNodeFree = m_pNodeFree->pNext;
    return pNode;
}

void FixedAlloc::Free(void* p)
{
    if (p != 0)
    {
        // simply return the node to the free list
        CNode* pNode = (CNode*)p;
        pNode->pNext = m_pNodeFree;
        m_pNodeFree = pNode;
    }
}

inline char32_t Get(const char32_t*& s)
{
    return *s++;
}

inline char32_t Get(const char*& source)
{
    static const char trailingBytesForUTF8[256] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
    };

    static const char32_t offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL,
                 0x03C82080UL, 0xFA082080UL, 0x82082080UL };

    char32_t ch = 0;
    unsigned short extraBytesToRead = trailingBytesForUTF8[*source];

    switch (extraBytesToRead) {
    case 5: ch += *source++; ch <<= 6;
    case 4: ch += *source++; ch <<= 6;
    case 3: ch += *source++; ch <<= 6;
    case 2: ch += *source++; ch <<= 6;
    case 1: ch += *source++; ch <<= 6;
    case 0: ch += *source++;
    }
    ch -= offsetsFromUTF8[extraBytesToRead];

    return ch;
}


struct Tnode
{
    Tnode *eqkid, *sibling;

    char32_t splitchar;
    /*bool*/ char terminating;
    unsigned char maxLength, minLength;

    Tnode(char32_t ch)
        : splitchar(ch)
        , terminating(false)
        , maxLength(0)
        , minLength(255)
        , eqkid(0), sibling(0)
    {
    }

    void* operator new(size_t size, FixedAlloc& s_alloc)
    {
        assert(size == s_alloc.GetAllocSize());
        return s_alloc.Alloc();
    }
    void* operator new(size_t, void* p) { return p; }

    void operator delete(void* p, FixedAlloc& s_alloc) { s_alloc.Free(p); }
};

template<typename T>
Tnode* insert_new(const T *stream, char32_t ch, unsigned int& length, FixedAlloc& s_alloc)
{
    assert(ch & ~31);

    Tnode *p = new(s_alloc) Tnode(ch);

    ch = Get(stream);
    if (0 == ch)
        p->terminating = true;
    else
    {
        ++length;
        p->eqkid = insert_new(stream, ch, length, s_alloc);
    }

    p->maxLength = length;
    p->minLength = length;

    return p;
}

template<typename T>
Tnode* insert(Tnode *p, const T *stream, char32_t ch, unsigned int& length, FixedAlloc& s_alloc)
{
    assert(ch & ~31);

    if (p && ch < p->splitchar) // reverse order
    {
        p->sibling = insert(p->sibling, stream, ch, length, s_alloc);
    }
    else
    {
        if (p == 0) {
            p = new(s_alloc) Tnode(ch);
        }
        if (ch == p->splitchar) {
            ch = Get(stream);
            if (0 == ch)
                p->terminating = true;
            else
            {
                ++length;
                p->eqkid = insert(p->eqkid, stream, ch, length, s_alloc);
            }
        }
        else
        {
            Tnode* temp = insert_new(stream, ch, length, s_alloc);
            assert(0 == temp->sibling);
            temp->sibling = p;
            temp->minLength = p->minLength;
            temp->maxLength = p->maxLength;
            p = temp;
        }
    }

    if (p->maxLength < length)
        p->maxLength = length;

    if (p->minLength > length)
        p->minLength = length;

    assert(p->minLength <= p->maxLength);

    return p;
}


const unsigned int ZERO_DISTANCE = 0xFFFFFFFF;//(unsigned int)-1;

class Breathalyzer
{
private:
    enum { DISTANCE_THRESHOLD = 3 };

    struct SearchData {
        std::vector<std::u32string> words;
        char32_t buffer[512];
    };

    Tnode* wordList = nullptr;

    FixedAlloc s_alloc{ sizeof(Tnode), 64 };

    __inline__
    bool DoSearch(const Tnode* p, unsigned int* d, const std::u32string& s,
                  const int offset, const unsigned int maxDistance, unsigned int& distance) const
    {
        int threshold = int(s.size());
        if (p->eqkid != 0)
            threshold -= p->eqkid->maxLength - offset;

        unsigned int* upLeft = d;
        unsigned int* downRight = upLeft + s.size() + 2;

        const char32_t* i = s.data();
        const char32_t* dataEnd = i + s.size();

        const char32_t splitchar = p->splitchar;

        const unsigned int startDistance = distance;

        bool ok = (0 < threshold)? ((startDistance >> threshold) >= maxDistance) : (startDistance >= maxDistance);

        if (!ok)
        {
            const char32_t* pThreshold = i + threshold - 1;

            for (; i < pThreshold; ++i, ++upLeft, ++downRight)
            {
                if (*i == splitchar)
                    distance = *upLeft;
                else
                    distance = (*upLeft | *(upLeft + 1) | distance) >> 1;

                *downRight = distance;

                if ((distance >> (pThreshold - i)) >= maxDistance)
                {
                    goto ok_loop;
                }
            }
            for (;;)
            {
                if (*i == splitchar)
                    distance = *upLeft;
                else
                    distance = (*upLeft | *(upLeft + 1) | distance) >> 1;

                *downRight = distance;

                if (distance >= maxDistance)
                {
                    goto ok_loop;
                }

                if (++i >= dataEnd)
                    return false;

                ++upLeft;
                ++downRight;
            }

            return false;
        }
        else
        {
            for (;;)
            {
                if (*i == splitchar)
                    distance = *upLeft;
                else
                    distance = (*upLeft | *(upLeft + 1) | distance) >> 1;

                *downRight = distance;
ok_loop:
                if (++i >= dataEnd)
                    return true;

                ++upLeft;
                ++downRight;
            }

            return true;
        }
    }


    bool GetDistance(const Tnode* p, unsigned int* d, const std::u32string& s,
        int offset, int& maxDistance, SearchData* searchData) const
    {
        for (; p != 0; p = p->eqkid, ++offset, d += s.size() + 1)
        {
            if (int(s.length()) - p->maxLength > maxDistance)
                return false;

            if (p->minLength - int(s.length()) > maxDistance)
                return false;

            if (GetDistance(p->sibling, d, s, offset, maxDistance, searchData))
                return true;

            if (0 == p->eqkid && !p->terminating)
                return false;

            //int distance(offset);
            unsigned int distance(ZERO_DISTANCE >> offset);
            const unsigned int expMaxDistance(ZERO_DISTANCE >> maxDistance);

            if (!DoSearch(p, d, s, offset, expMaxDistance, distance))
                return false;

            if (searchData)
                searchData->buffer[offset - 1] = p->splitchar;

            if (p->terminating)
            {
                if (distance > expMaxDistance)
                {
                    maxDistance = 31 - Log(distance);
                    if (searchData) {
                        searchData->words.clear();
                        searchData->words.emplace_back(searchData->buffer, searchData->buffer + offset);
                    }
                    else if (maxDistance <= DISTANCE_THRESHOLD)
                        return true;
                }
                else if (searchData && distance == expMaxDistance)
                    searchData->words.emplace_back(searchData->buffer, searchData->buffer + offset);
            }
        }
        return false;
    }

    bool find0(const Tnode* p, const char32_t* s) const
    {
        for (; p != 0; p = p->eqkid)
        {
            while (p->splitchar > *s)
            {
                if (0 == p->sibling)
                {
                    return false;
                }
                p = p->sibling;
            }

            if (p->splitchar < *s)
            {
                return false;
            }

            if (0 == *(++s))
            {
                return p->terminating;
            }
        }

        return false;
    }

    bool find(const Tnode* p, unsigned int* d, const std::u32string& s,
        int offset, const int maxDistance, SearchData* searchData) const
    {
        for (; p != 0; p = p->eqkid, ++offset, d += s.size() + 1)
        {
            if (int(s.length()) - p->maxLength > maxDistance)
                return false;

            if (p->minLength - int(s.length()) > maxDistance)
                return false;

            if (find(p->sibling, d, s, offset, maxDistance, searchData))
                return true;

            if (0 == p->eqkid && !p->terminating)
                return false;

            //int distance(offset);
            unsigned int distance(ZERO_DISTANCE >> offset);
            const unsigned int expMaxDistance(ZERO_DISTANCE >> maxDistance);

            if (!DoSearch(p, d, s, offset, expMaxDistance, distance))
                return false;

            if (searchData)
                searchData->buffer[offset - 1] = p->splitchar;

            if (p->terminating)
            {
                if (distance >= expMaxDistance)
                {
                    if (searchData)
                        searchData->words.emplace_back(searchData->buffer, searchData->buffer + offset);
                    else
                        return true;
                }
            }
        }
        return false;
    }

    void GetWords(const Tnode* p, int offset, const int length, SearchData& searchData) const
    {
        for (; p != 0; p = p->eqkid, ++offset)
        {
            if (p->minLength > length)
                return;

            if (offset > length)
                return;

            GetWords(p->sibling, offset, length, searchData);

            searchData.buffer[offset - 1] = p->splitchar;

            if (offset == length && p->terminating)
                searchData.words.emplace_back(searchData.buffer, searchData.buffer + offset);
        }
    }

public:
    template <typename T>
    void Insert(const T* s)
    {
        if (auto ch = Get(s))
        {
            unsigned int length(1);
            wordList = insert(wordList, s, ch, length, s_alloc);
        }
    }

    int GetDistance(const std::u32string& s) const
    {
        if (!wordList)
            return s.length();

        if (s.empty())
            return wordList->minLength;

        if (find0(wordList, s.c_str()))
            return 0;

        enum { FAST_BUFFER_SIZE = 512 };

        unsigned int sbuf[FAST_BUFFER_SIZE];

        const unsigned int wordDim = wordList->maxLength + 1;
        const size_t bufSize = wordDim * (s.size() + 1);
        unsigned int* d = (bufSize > FAST_BUFFER_SIZE)? new unsigned int[bufSize] : sbuf;

        for (unsigned int i = 0; i <= s.size(); ++i)
            d[i] = ZERO_DISTANCE >> i;
        for (unsigned int i = 1; i < wordDim; ++i)
            d[i * (s.size() + 1)] = ZERO_DISTANCE >> i;


        for (int result = 1; result < DISTANCE_THRESHOLD; ++result)
            if (find(wordList, &d[0], s, 1, result, nullptr))
            {
                if (d != sbuf)
                    delete[] d;
                return result;
            }

        int result = std::max((unsigned int) wordList->minLength, (unsigned int) s.size());
        GetDistance(wordList, &d[0], s, 1, result, nullptr);

        if (d != sbuf)
            delete[] d;
        return result;
    }

    std::tuple<int, std::vector<std::u32string>> GetDistanceWords(const std::u32string& s) const
    {
        if (!wordList)
            return { static_cast<int>(s.length()), {} };

        if (s.empty())
        {
            SearchData searchData;
            GetWords(wordList, 1, wordList->minLength, searchData);
            return { wordList->minLength, std::move(searchData.words) };
        }

        if (find0(wordList, s.c_str()))
            return { 0, { s } };

        enum { FAST_BUFFER_SIZE = 512 };

        unsigned int sbuf[FAST_BUFFER_SIZE];

        const unsigned int wordDim = wordList->maxLength + 1;
        const size_t bufSize = wordDim * (s.size() + 1);
        unsigned int* d = (bufSize > FAST_BUFFER_SIZE) ? new unsigned int[bufSize] : sbuf;

        for (unsigned int i = 0; i <= s.size(); ++i)
            d[i] = ZERO_DISTANCE >> i;
        for (unsigned int i = 1; i < wordDim; ++i)
            d[i * (s.size() + 1)] = ZERO_DISTANCE >> i;

        SearchData searchData;

        for (int result = 1; result < DISTANCE_THRESHOLD; ++result)
            if (find(wordList, &d[0], s, 1, result, &searchData), !searchData.words.empty())
            {
                if (d != sbuf)
                    delete[] d;
                return { result, std::move(searchData.words) };
            }

        int result = std::max((unsigned int)wordList->minLength, (unsigned int)s.size());
        GetDistance(wordList, &d[0], s, 1, result, &searchData);

        if (d != sbuf)
            delete[] d;
        return { result, std::move(searchData.words) };
    }
};


}  // namespace levenshteinsgate
