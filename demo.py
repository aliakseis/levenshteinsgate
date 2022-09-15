from levenshteinsgate import Trie
from pathlib import Path

twl06 = Path('twl06.txt').read_text().split()
trie = Trie(twl06)

david_22719 = Path('david_22719.in').read_text().split()

sum = 0
for v in david_22719:
    sum += trie.min_distance(v)

print(sum)
