# pydatatamer

View [data_tamer](https://github.com/PickNikRobotics/data_tamer/tree/main) mcap files as numpy array

## Supported Platforms
- Linux (for now)

## Quick Start (from source)

1. Clone pydata_tamer

```
git clone https://github.com/raghavauppuluri13/pydatatamer.git
cd pydata_tamer
```

1. Install pydata_tamer (also builds pybind11)
```
pip3 install -e .
```

3. Run example

```
cd examples
python3 read_data.py
```

## Add new snapshot types

1. Update `include/mcap_types.hpp` with your new snapshot type

```
struct PointKinematics {
    double position;
    double velocity;

    PointKinematics() {}

    inline void update(std::string name,
                       const DataTamerParser::VarNumber &data) {
        int index = EXTRACT_INDEX(name);

        if (array_name_eq(name, "position")) {
            position = std::get<double>(data);
        } else if (name == "velocity") {
            velocity = std::get<double>(data);
        } else {
            std::cerr << "unknown series name: " << name << std::endl;
            throw std::runtime_error("Unknown series!");
        }
    }
};
```

2. Update mcap_reader.cpp's `PYBIND11_MODULE->McapReader` with your new snapshot type template

```
py::class_<PointKinematics>(m, "PointKinematics")
    .def(py::init<>())
    .def_readwrite("position", &PointKinematics::position)
    .def_readwrite("velocity", &PointKinematics::velocity);
PYBIND11_NUMPY_DTYPE(PointKinematics, position, velocity);
```
3. pip install again to build

## Credits
- MCAP reader code from [mcap-cpp](https://github.com/foxglove/mcap/tree/main/cpp/mcap/include/mcap)
- mcap_reader.cpp and data_tamer_parser.hpp  [data_tamer](https://github.com/PickNikRobotics/data_tamer/tree/main)
