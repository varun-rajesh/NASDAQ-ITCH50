import matplotlib.pyplot as plt
import sys

prices = []
with open(sys.argv[1]) as f:
    while line := f.readline():
        price = int(line) / 10000.0
        prices.append(price)

plt.plot(prices)
plt.show()