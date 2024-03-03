from pydatatamer.mcap_tamer import McapTamer
import numpy as np

mcap_tamer = McapTamer()
mcap_tamer.init("proprio_example.mcap")

# structured array
data = mcap_tamer.parse()

print("data shape", data.shape)
print("data type", data.dtype)
