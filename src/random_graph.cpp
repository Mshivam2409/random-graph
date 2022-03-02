#include "random_graph.h"
using Graph = boost::compressed_sparse_row_graph<>;
using Vertex = Graph::vertex_descriptor;

template <typename V>
struct Mapper {
	using Id = uint64_t; // component id
	using Cardinality = uint64_t;

	using Map = boost::container::flat_map<Id, Cardinality>;
	using Value = Map::value_type;
	Map& storage;

	friend void put(Mapper& m, V const& /*v*/, Id id) { m.storage[id] += 1; }

	Value largest() const {
		return not storage.empty()
			? *max_element(begin(storage), end(storage),
				[](Value const& a, Value const& b) {
					return a.second < b.second;
				})
			: Value{};
	}
};

template <typename V> struct boost::property_traits<Mapper<V>> {
	using category = boost::writable_property_map_tag;
	using key_type = V;
	using value_type = int;
};

std::pair<uint32_t, uint32_t> random_graph(const long double probability,
	const uint64_t size)
{
	std::vector<std::pair<Vertex, Vertex>> edges;

	for (uint32_t i = 0; i < size; i++) {

		auto edge_list = stats::rbern<std::vector<uint8_t>>(size, 1, probability);
		std::for_each(edge_list.begin(), edge_list.end(), [=, &edges, j = 0U](const uint8_t __edge) mutable {
			if (__edge) {
				edges.push_back(std::pair<Vertex, Vertex>(i, j));
			}
			j++;
		});
		edge_list.clear();
	}


	Graph graph{ boost::edges_are_sorted, edges.begin(), edges.end(), size };

	Mapper<uint64_t>::Map result;
	Mapper<uint64_t> mapper{ result };
	uint64_t num = boost::connected_components(graph, mapper);
	auto [id, cardinality] = mapper.largest();
	return std::pair<uint32_t, uint32_t>(edges.size(),cardinality);
}