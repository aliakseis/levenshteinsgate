# levenshteinsgate

<img width="1536" height="1024" alt="Image" src="https://github.com/user-attachments/assets/e95b53f1-ec72-4a3a-b466-ef2265624846" />

Installation
------------

 - clone this repository
 - `pip install --force-reinstall ./levenshteinsgate`

Demo
----
```
from levenshteinsgate import Trie
from pathlib import Path
import time
import random

twl06 = Path('twl06.txt').read_text().split()
random.shuffle(twl06)

trie = Trie(twl06)

david_22719 = Path('david_22719.in').read_text().split()
random.shuffle(david_22719)

print('Starting...')
start_time = time.time()

sum = 0
for v in david_22719:
    sum += trie.min_distance(v)

print(sum)

print("%s seconds" % (time.time() - start_time))

start_time = time.time()

sum = 0
sumWords = 0
for v in david_22719:
    r = trie.min_distance_words(v)
    sum += r[0]
    sumWords += len(r[1])

print(sum, sumWords)

print("%s seconds" % (time.time() - start_time))

print(trie.min_distance_words(''))

print(trie.min_distance_words('o'))

print(trie.min_distance_words('ertyuiopqzwaig'))
```
Output:
```
Starting...                                                                                                                                                             
22719                                                                                                                                                                   
2.1426548957824707 seconds                                                                                                                                             
22719 369596                                                                                                                                                           
3.019028902053833 seconds                                                                                                                                               
(2, ['aa', 'ab', 'ad', 'ae', 'ag', 'ah', 'ai', 'al', 'am', 'an', 'ar', 'as', 'at', 'aw', 'ax', 'ay', 'ba', 'be', 'bi', 'bo', 'by', 'de', 'do', 'ed', 'ef', 'eh', 'el', 'em', 'en', 'er', 'es', 'et', 'ex', 'fa', 'fe', 'go', 'ha', 'he', 'hi', 'hm', 'ho', 'id', 'if', 'in', 'is', 'it', 'jo', 'ka', 'ki', 'la', 'li', 'lo', 'ma', 'me', 'mi', 'mm', 'mo', 'mu', 'my', 'na', 'ne', 'no', 'nu', 'od', 'oe', 'of', 'oh', 'oi', 'om', 'on', 'op', 'or', 'os', 'ow', 'ox', 'oy', 'pa', 'pe', 'pi', 'qi', 're', 'sh', 'si', 'so', 'ta', 'ti', 'to', 'uh', 'um', 'un', 'up', 'us', 'ut', 'we', 'wo', 'xi', 'xu', 'ya', 'ye', 'yo', 'za'])                      
(1, ['bo', 'do', 'go', 'ho', 'jo', 'lo', 'mo', 'no', 'od', 'oe', 'of', 'oh', 'oi', 'om', 'on', 'op', 'or', 'os', 'ow', 'ox', 'oy', 'so', 'to', 'wo', 'yo'])             
(8, ['eriophyid', 'erysipelas', 'exteriorizing'])
```
Comparing with https://github.com/poke1024/simtrie

```
>>> from levenshteinsgate import Trie
>>> from pathlib import Path
>>> import random
>>> twl06 = Path('twl06.txt').read_text().split()
>>> random.shuffle(twl06)
>>>
>>> trie = Trie(twl06)
>>> import timeit
>>> def search1():
...   return trie.min_distance_words('ertyuiopqzwaig')
...
>>> print(timeit.timeit(stmt=search1, number=1))
0.01364929999999731
>>> import simtrie
>>> s = simtrie.Set(twl06)
>>> def search2():
...     return list(s.similar("ertyuiopqzwaig", 8))
...
>>> print(timeit.timeit(stmt=search2, number=1))
0.019658799999888288
>>> print(search1())
(8, ['eriophyid', 'erysipelas', 'exteriorizing'])
>>> print(search2())
[('eriophyid', 8.0), ('erysipelas', 8.0), ('exteriorizing', 8.0)]
```

# Levenshtein's Gate / C++

A high-performance C++ fuzzy string matching library based on a compressed trie structure and an optimized Levenshtein distance search algorithm.

The library is designed for extremely fast dictionary lookups, spell-checking, auto-correction, and nearest-word searches while minimizing memory allocations and search overhead.

## Features

* Fast approximate string matching
* Exact word lookup support
* Levenshtein distance calculation against an entire dictionary
* Retrieval of all closest matching words
* UTF-8 and UTF-32 support
* Memory-efficient trie-based word storage
* Custom fixed-size allocator for reduced heap fragmentation
* Allocation-free search path for small inputs
* Early pruning based on word length bounds
* Optimized bit-parallel distance computation

---

## Overview

Instead of comparing a query string against every word in a dictionary, the library stores all words in a specialized trie structure and traverses only potentially relevant branches.

During traversal, an optimized dynamic-programming state is propagated through the trie. Branches that cannot produce a better result are discarded immediately.

This dramatically reduces the amount of work required when searching large dictionaries.

The implementation combines:

* Trie-based indexing
* Length-based pruning
* Bit-parallel Levenshtein computation
* Custom memory allocation

to achieve significantly better performance than naïve dictionary scanning.

---

## Architecture

### Trie Nodes (`Tnode`)

Each node stores:

* Character value (`splitchar`)
* End-of-word flag
* Minimum descendant word length
* Maximum descendant word length
* Child pointer (`eqkid`)
* Sibling pointer (`sibling`)

The additional length metadata allows aggressive pruning during search.

```text
root
 ├── c
 │    └── a
 │         └── t
 └── d
      └── o
           └── g
```

---

### Custom Allocator

The project includes a lightweight allocator inspired by MFC's `CFixedAlloc`.

Components:

* `Plex`

  * Block allocation management
* `FixedAlloc`

  * Fixed-size node allocator
* Free-list recycling

Benefits:

* Reduced allocation overhead
* Improved cache locality
* Faster trie construction
* Minimal memory fragmentation

---

### UTF Support

The library supports:

* UTF-32 input directly
* UTF-8 input through internal decoding

The `Get()` overloads transparently read either:

```cpp
const char*
```

or

```cpp
const char32_t*
```

streams and produce Unicode code points.

This allows fuzzy matching of international text without external conversion.

---

## Core Algorithms

### Exact Search

Before performing expensive distance calculations, the library attempts a direct lookup.

```cpp
find0(...)
```

Complexity:

```text
O(length)
```

for exact matches.

---

### Approximate Search

Approximate matching is performed by traversing the trie while propagating a compact distance state.

The algorithm computes edit distance incrementally and avoids visiting irrelevant branches.

Key optimizations:

* Branch pruning
* Length bounds checking
* Bit-parallel state representation
* Early exit on threshold matches

---

### Distance Threshold Search

For common spelling-correction scenarios, the search first attempts to find matches within small edit distances.

```cpp
distance = 1
distance = 2
```

If successful, the search terminates immediately.

This makes typical typo correction extremely fast.

Examples:

```text
helo  -> hello
teh   -> the
appel -> apple
```

---

### Best-Match Search

When no close match exists, the algorithm computes the true minimum distance.

```cpp
GetDistance(...)
```

returns:

```cpp
int
```

representing the smallest edit distance between the query and any dictionary word.

---

### Closest Word Retrieval

```cpp
GetDistanceWords(...)
```

returns:

```cpp
std::tuple<
    int,
    std::vector<std::u32string>
>
```

containing:

* Minimum edit distance
* All words achieving that distance

Example:

```text
Input:
    "helo"

Output:
    distance = 1

    hello
    help
```

---

## Public API

### Insert Word

```cpp
template <typename T>
void Insert(const T* s);
```

Adds a UTF-8 or UTF-32 word to the dictionary.

Example:

```cpp
Breathalyzer dict;

dict.Insert("hello");
dict.Insert("world");
```

---

### Minimum Distance

```cpp
int GetDistance(
    const std::u32string& s
) const;
```

Returns the minimum edit distance between the query and all stored words.

Example:

```cpp
int d = dict.GetDistance(U"helo");
```

---

### Distance + Matches

```cpp
std::tuple<
    int,
    std::vector<std::u32string>
>
GetDistanceWords(
    const std::u32string& s
) const;
```

Returns both:

* Best distance
* Closest matching words

Example:

```cpp
auto [distance, words] =
    dict.GetDistanceWords(U"helo");
```

---

## Performance Characteristics

### Dictionary Construction

```text
O(total characters)
```

### Exact Lookup

```text
O(word length)
```

### Approximate Search

Typical performance is significantly better than:

```text
O(dictionary_size × word_length²)
```

because large portions of the trie are pruned before evaluation.

Performance improvements become more significant as dictionary size increases.

---

## Typical Applications

### Spell Checkers

```text
recieve -> receive
```

### Search Suggestions

```text
iphon -> iphone
```

### Command Correction

```text
git comit -> git commit
```

### OCR Post-Processing

Correct recognition errors by finding the nearest dictionary word.

### Natural Language Processing

Candidate generation for fuzzy token matching.

### Autocomplete Systems

Tolerance for typing mistakes during lookup.

---

## Design Goals

* High speed
* Low memory overhead
* Unicode support
* Minimal heap allocations
* Fast nearest-word retrieval
* Large dictionary scalability
* Header-only integration

---

## Example

```cpp
#include "levenshteinsgate.h"

using namespace levenshteinsgate;

int main()
{
    Breathalyzer dict;

    dict.Insert("hello");
    dict.Insert("help");
    dict.Insert("world");

    auto [distance, matches] =
        dict.GetDistanceWords(U"helo");

    return 0;
}
```

---
This code is particularly interesting because it is not a conventional dynamic-programming Levenshtein implementation. It combines a ternary/trie-like dictionary structure, bit-parallel distance propagation, and length-based pruning, making it closer to a specialized spell-checking engine than a generic edit-distance library.
