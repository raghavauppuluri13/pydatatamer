#include "data_tamer_parser.hpp"
#include "mcap_types.hpp"
#include "reader.hpp"
#include <pybind11/attr.h>
#include <pybind11/buffer_info.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

template <typename MsgStruct> class McapTamer {

  public:
    McapTamer(){};
    void init(std::string filepath) {
        // open the file
        {
            auto const res = reader_.open(filepath);
            if (!res.ok()) {
                throw std::runtime_error("Can't open MCAP file");
            }
        }
        // must call this, before accessing the schemas
        auto summary =
            reader_.readSummary(mcap::ReadSummaryMethod::NoFallbackScan);
        for (const auto &[schema_id, mcap_schema] : reader_.schemas()) {
            std::string schema_text(
                reinterpret_cast<const char *>(mcap_schema->data.data()),
                mcap_schema->data.size());

            auto dt_schema = DataTamerParser::BuilSchemaFromText(schema_text);
            schema_id_to_hash_[mcap_schema->id] = dt_schema.hash;
            hash_to_schema_[dt_schema.hash] = dt_schema;
        }
    }

    py::array_t<MsgStruct> parse() {
        // this application will do nothing with the actual data. We will simple
        // count the number of messages per time series
        using MessageCount = std::map<std::string, size_t>;
        std::map<std::string, MessageCount> message_counts_per_channel;

        auto IncrementCounter = [&](const std::string &series_name,
                                    MessageCount &message_counts) {
            auto it = message_counts.find(series_name);
            if (it == message_counts.end()) {
                message_counts[series_name] = 1;
            } else {
                it->second++;
            }
        };

        std::vector<MsgStruct> msgs_array;

        // parse all messages
        for (const auto &msg : reader_.readMessages()) {
            // start updating the fields of SnapshotView
            DataTamerParser::SnapshotView snapshot;
            snapshot.timestamp = msg.message.logTime;
            snapshot.schema_hash = schema_id_to_hash_.at(msg.schema->id);
            const auto &dt_schema = hash_to_schema_.at(snapshot.schema_hash);

            // msg_buffer contains both active_mask and payload, serialized
            // one after the other.
            DataTamerParser::BufferSpan msg_buffer = {
                reinterpret_cast<const uint8_t *>(msg.message.data),
                msg.message.dataSize};

            const uint32_t mask_size =
                DataTamerParser::Deserialize<uint32_t>(msg_buffer);
            snapshot.active_mask.data = msg_buffer.data;
            snapshot.active_mask.size = mask_size;
            msg_buffer.trimFront(mask_size);

            const uint32_t payload_size =
                DataTamerParser::Deserialize<uint32_t>(msg_buffer);
            snapshot.payload.data = msg_buffer.data;
            snapshot.payload.size = payload_size;

            // prepare the callback to be invoked by ParseSnapshot.
            // Wrap IncrementCounter to add the channel_name
            const std::string &channel_name = msg.channel->topic;
            auto &message_counts = message_counts_per_channel[channel_name];
            MsgStruct msg_struct;
            auto callback_number = [&](const std::string &series_name,
                                       const DataTamerParser::VarNumber &data) {
                msg_struct.update(series_name, data);
                IncrementCounter(series_name, message_counts);
            };

            DataTamerParser::ParseSnapshot(dt_schema, snapshot,
                                           callback_number);
            msgs_array_.push_back(msg_struct);
        }

        // display the counted data samples
        for (const auto &[channel_name, msg_counts] :
             message_counts_per_channel) {
            std::cout << channel_name << ":" << std::endl;
            for (const auto &[name, count] : msg_counts) {
                std::cout << "   " << name << ":" << count << std::endl;
            }
        }
        return py::array_t<MsgStruct>(msgs_array_.size(), msgs_array_.data());
    }

  private:
    mcap::McapReader reader_;
    // start reading all the schemas and parsing them
    std::unordered_map<mcap::SchemaId, size_t> schema_id_to_hash_;
    std::unordered_map<size_t, DataTamerParser::Schema> hash_to_schema_;
    std::vector<MsgStruct> msgs_array_;
};

PYBIND11_MODULE(mcap_tamer, m) {
    m.doc() = "Python bindings for data_tamer";

    py::class_<Proprioception>(m, "Proprioception")
        .def(py::init<>())
        .def_readwrite("qpos", &Proprioception::qpos)
        .def_readwrite("qvel", &Proprioception::qvel)
        .def_readwrite("qpos_d", &Proprioception::qpos_d)
        .def_readwrite("qeffort", &Proprioception::qeffort)
        .def_readwrite("torq_bias", &Proprioception::torq_bias);
    PYBIND11_NUMPY_DTYPE(Proprioception, qpos, qvel, qpos_d, qeffort,
                         torq_bias);

    py::class_<McapTamer<Proprioception>>(m, "McapTamer")
        .def(py::init<>())
        .def("init", &McapTamer<Proprioception>::init)
        .def("parse", &McapTamer<Proprioception>::parse);
}
