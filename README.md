# levenshteinsgate

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
