#include <levenshteinsgate/trie.h>

#include <pybind11/pybind11.h>

#include <cstdint>
#include <stdexcept>

namespace py = pybind11;

class Trie {
public:
    Trie() = default;

    inline explicit Trie(py::iterable words);

    void insert(const std::string& word) {
        trie_.Insert(word.c_str());
    }

    int min_distance(const std::string& word) const {
        return trie_.GetDistance(word);
    }

private:
    levenshteinsgate::Breathalyzer trie_;
};

Trie::Trie(py::iterable words) {
    for (auto word : words) {
        if (PyUnicode_Check(word.ptr())) {
            word = py::reinterpret_steal<py::object>(PyUnicode_AsUTF8String(word.ptr()));
            if (!word) {
                throw std::runtime_error("failed to decode UTF-8");
            }
        }
        char* buffer;
        ssize_t length;
        if (PYBIND11_BYTES_AS_STRING_AND_SIZE(word.ptr(), &buffer, &length) != 0) {
            throw std::runtime_error("failed to extract bytes contents");
        }
        trie_.Insert(std::string(buffer, length).c_str());
    }
}

PYBIND11_MODULE(levenshteinsgate, m) {
    py::class_<Trie>(m, "Trie")
        .def(py::init<>())
        .def(py::init<py::iterable>(), py::arg("words"))
        .def("insert", &Trie::insert, py::arg("word"))
        .def("min_distance", &Trie::min_distance, py::arg("word"));
}
