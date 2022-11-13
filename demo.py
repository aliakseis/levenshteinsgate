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
