# pydatatamer

View [data_tamer](https://github.com/PickNikRobotics/data_tamer/tree/main) mcap files as numpy array

## Supported Platforms
- Linux (for now)

## Quick Start (from source)

1. Clone pydata_tamer

```bash
git clone https://github.com/raghavauppuluri13/pydatatamer.git
cd pydata_tamer
```

1. Install pydata_tamer (also builds pybind11)
```bash
pip3 install -e .
```

3. Run example

```bash
cd examples
python3 read_data.py
```

## Add new snapshot types

1. Update `include/mcap_types.hpp` with your new snapshot type

```cpp
struct Point {
    double x;
    double y;

    Point() {}

    inline void update(std::string name,
                       const DataTamerParser::VarNumber &data) {
        if (name == "x")) {
            position = std::get<double>(data);
        } else if (name == "y") {
            velocity = std::get<double>(data);
        } else {
            std::cerr << "unknown series name: " << name << std::endl;
            throw std::runtime_error("Unknown series!");
        }
    }
};
```

2. Update mcap_reader.cpp's `PYBIND11_MODULE->McapReader` with your new snapshot type template

```cpp
py::class_<Point>(m, "Point")
    .def(py::init<>())
    .def_readwrite("x", &Point::x)
    .def_readwrite("y", &Point::x);
PYBIND11_NUMPY_DTYPE(Point, x, y);
```
3. pip install again to build
4. Read in python
```python
from pydatatamer.mcap_tamer import McapTamer
import numpy as np
mcap_tamer = McapTamer()
mcap_tamer.init("points.mcap")
# structured array
points_data = mcap_tamer.parse()
print(points)
```

## Credits
- MCAP reader code from [mcap-cpp](https://github.com/foxglove/mcap/tree/main/cpp/mcap/include/mcap)
- mcap_reader.cpp and data_tamer_parser.hpp  [data_tamer](https://github.com/PickNikRobotics/data_tamer/tree/main)
