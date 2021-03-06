#include <cstring>

#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "drake/bindings/pydrake/common/deprecation_pybind.h"
#include "drake/bindings/pydrake/common/drake_optional_pybind.h"
#include "drake/bindings/pydrake/documentation_pybind.h"
#include "drake/bindings/pydrake/pydrake_pybind.h"
#include "drake/lcm/drake_lcm.h"
#include "drake/lcm/drake_lcm_interface.h"
#include "drake/lcm/drake_mock_lcm.h"

namespace drake {
namespace pydrake {

PYBIND11_MODULE(lcm, m) {
  // NOLINTNEXTLINE(build/namespaces): Emulate placement in namespace.
  using namespace drake::lcm;
  constexpr auto& doc = pydrake_doc.drake.lcm;

  // Use `py::bytes` as a mid-point between C++ LCM (`void* + int` /
  // `vector<uint8_t>`) and Python LCM (`str`).
  using PyHandlerFunction = std::function<void(py::bytes)>;

  {
    using Class = DrakeLcmInterface;

    py::class_<Class>(m, "DrakeLcmInterface", doc.DrakeLcmInterface.doc)
        // N.B. We do not bind `Subscribe` as multi-threading from C++ may
        // wreak havoc on the Python GIL with a callback.
        .def("Publish",
            [](Class* self, const std::string& channel, py::bytes buffer,
                optional<double> time_sec) {
              // TODO(eric.cousineau): See if there is a way to extra the raw
              // bytes from `buffer` without copying.
              std::string str = buffer;
              self->Publish(channel, str.data(), str.size(), time_sec);
            },
            py::arg("channel"), py::arg("buffer"),
            py::arg("time_sec") = py::none(), doc.DrakeLcmInterface.Publish.doc)
        .def("HandleSubscriptions", &DrakeLcmInterface::HandleSubscriptions,
            py::arg("timeout_millis"),
            doc.DrakeLcmInterface.HandleSubscriptions.doc);
  }

  {
    using Class = DrakeLcm;
    py::class_<Class, DrakeLcmInterface>(m, "DrakeLcm", doc.DrakeLcm.doc)
        .def(py::init<>(), doc.DrakeLcm.ctor.doc_0args)
        .def(py::init<std::string>(), py::arg("lcm_url"),
            doc.DrakeLcm.ctor.doc_1args)
        .def("StartReceiveThread",
            [](DrakeLcm* self) {
              WarnDeprecated(
                  "Call DrakeLcm.HandleSubscriptions periodically instead.");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
              self->StartReceiveThread();
#pragma GCC diagnostic pop
            },
            doc.DrakeLcm.StartReceiveThread.doc_deprecated)
        .def("StopReceiveThread",
            [](DrakeLcm* self) {
              WarnDeprecated(
                  "Call DrakeLcm.HandleSubscriptions periodically instead.");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
              self->StopReceiveThread();
#pragma GCC diagnostic pop
            },
            doc.DrakeLcm.StopReceiveThread.doc_deprecated);
    // TODO(eric.cousineau): Add remaining methods.
  }

  {
    using Class = DrakeMockLcm;
    py::class_<Class, DrakeLcmInterface>(
        m, "DrakeMockLcm", doc.DrakeMockLcm.doc)
        .def(py::init<>(), doc.DrakeMockLcm.ctor.doc)
        .def("Subscribe",
            [](Class* self, const std::string& channel,
                PyHandlerFunction handler) {
              auto subscription = self->Subscribe(
                  channel, [handler](const void* data, int size) {
                    handler(py::bytes(static_cast<const char*>(data), size));
                  });
              // Unsubscribe is not supported by the mock.
              DRAKE_DEMAND(subscription == nullptr);
            },
            py::arg("channel"), py::arg("handler"),
            doc.DrakeMockLcm.Subscribe.doc)
        .def("InduceSubscriberCallback",
            [](Class* self, const std::string& channel, py::bytes buffer) {
              WarnDeprecated("Use Publish + HandleSubscriptions instead.");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
              std::string str = buffer;
              self->InduceSubscriberCallback(channel, str.data(), str.size());
#pragma GCC diagnostic pop
            },
            py::arg("channel"), py::arg("buffer"),
            doc.DrakeMockLcm.InduceSubscriberCallback.doc_deprecated)
        .def("get_last_published_message",
            [](const Class* self, const std::string& channel) {
              WarnDeprecated("Use pydrake.lcm.Subscriber instead.");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
              const std::vector<uint8_t>& bytes =
                  self->get_last_published_message(channel);
              return py::bytes(
                  reinterpret_cast<const char*>(bytes.data()), bytes.size());
#pragma GCC diagnostic pop
            },
            py::arg("channel"),
            doc.DrakeMockLcm.get_last_published_message.doc_deprecated);
  }

  ExecuteExtraPythonCode(m);
}

}  // namespace pydrake
}  // namespace drake
