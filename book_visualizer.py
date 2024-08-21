import matplotlib.pyplot as plt
import numpy as np
import sys
import time

book_path = sys.argv[1]

buy_buckets_list = []
sell_buckets_list = []

def generate_multiples_and_values(data):
    if len(data) == 0:
        return [0, 1], [0]
    
    min_key = min(data.keys())
    max_key = max(data.keys())
    multiples = list(range(min_key, max_key + 100, 100))
    values = [data.get(key, 0) for key in multiples]
    multiples += [max_key + 100]
    return multiples, values

with open(book_path) as file:
   
    buy_buckets = dict()
    sell_buckets = dict()
    while line := file.readline():
        if "Side" in line:
            buy_buckets_list.append(buy_buckets)
            sell_buckets_list.append(sell_buckets)
            buy_buckets = dict()
            sell_buckets = dict()
        else:
            split_line = line.split(",")
            
            side = split_line[0]
            price = int(split_line[2])
            volume = int(split_line[3])
            
            price_bucket = price // 1000
            if side == "B":
                if price_bucket not in buy_buckets:
                    buy_buckets[price_bucket] = 0
                buy_buckets[price_bucket] += volume
            if side == "S":
                if price_bucket not in sell_buckets:
                    sell_buckets[price_bucket] = 0
                sell_buckets[price_bucket] += volume

print(buy_buckets_list)
print(sell_buckets_list)


plt.ion()
figure, ax = plt.subplots(figsize=(10, 8))

buy_x, buy_y = generate_multiples_and_values(buy_buckets_list[0])
sell_x, sell_y = generate_multiples_and_values(sell_buckets_list[0])

buy_plot = ax.stairs(buy_y, buy_x, fill=True)
sell_plot = ax.stairs(sell_y, sell_x, fill=True)


plt.show()

for i in range(600, len(buy_buckets_list), 10):
    time.sleep(0.1)
    
    buy_x, buy_y = generate_multiples_and_values(buy_buckets_list[i])
    sell_x, sell_y = generate_multiples_and_values(sell_buckets_list[i])
    
    print(i)
    
    buy_plot.set_data(values=buy_y, edges=buy_x, baseline=None)
    sell_plot.set_data(values=sell_y, edges=sell_x, baseline=None)
    
    figure.canvas.draw()

    figure.canvas.flush_events()



                


