#pragma once
#include <stats.hpp>
#include <bitset>
#include <fstream>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/container/flat_map.hpp>


std::pair<uint32_t,uint32_t> random_graph(const long double probability,
	const uint64_t size);
