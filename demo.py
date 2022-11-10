from levenshteinsgate import Trie
from pathlib import Path
import time

twl06 = Path('twl06.txt').read_text().split()
trie = Trie(twl06)

david_22719 = Path('david_22719.in').read_text().split()

print('Starting...')
start_time = time.time()

sum = 0
for v in david_22719:
    sum += trie.min_distance(v)

print(sum)

print("%s seconds" % (time.time() - start_time))

start_time = time.time()

sum = 0
for v in david_22719:
    sum += trie.min_distance_words(v)[0]

print(sum)

print("%s seconds" % (time.time() - start_time))

print(trie.min_distance_words('ertyuiopqzwaig'))
