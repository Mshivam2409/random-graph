#include <omp.h>
#include <taskflow.hpp>
#include <stats.hpp>
#include <indicators/block_progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <CSVWriter.h>
#include <iostream>
#include <filesystem>
#include <random>
#include "random_graph.h"

typedef long double float80;
typedef std::tuple<float80, float80, float80> triplet;


template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}

namespace fs = std::filesystem;

#ifndef RANGE
#define RANGE 100
#endif
#ifndef ITERATIONS
#define ITERATIONS 100
#endif
#ifndef GRAPH_SIZE
#define GRAPH_SIZE 10000000
#endif

int main()
{
	const std::string storage_directory = "result";
	indicators::show_console_cursor(false);
	std::map<float80, triplet> storage;
	indicators::BlockProgressBar bar{
		indicators::option::BarWidth{80},
		indicators::option::Start{"["},
		indicators::option::End{"]"},
		indicators::option::ForegroundColor{indicators::Color::white},
		indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}} };


	if (!fs::is_directory(storage_directory) || !fs::exists(storage_directory)) { // Check if src folder exists
		fs::create_directory(storage_directory); // create src folder
	}


	bar.print_progress();
	tf::Taskflow taskflow;
	tf::Executor executor;
	constexpr double begin = 0.0;
	constexpr double end = ((double)2.0) / GRAPH_SIZE;
	constexpr double difference = end / RANGE;
	std::vector<double> probabilities;
	probabilities.push_back(begin);
	for (uint8_t i = 1; i <= RANGE; i++)
		probabilities.push_back(begin + (i * difference));
	tf::Task compute = taskflow.for_each(probabilities.begin() + 1, probabilities.end(), [&bar, &storage,&difference,&storage_directory](auto& i)
		{ 

			CSVWriter csv;
			const uint8_t id = i / difference;
			float80 avg = 0;
			uint32_t _min = GRAPH_SIZE + 1, _max = 0;
			csv.newRow() << "iteration" << "edges" << "cardinality";
			for (uint16_t j = 0; j < ITERATIONS; j++)
			{
				auto cardinality = random_graph(i, GRAPH_SIZE);
				_min = std::min(cardinality.second, _min);
				_max = std::max(cardinality.second, _max);
				avg += cardinality.second;
				csv.newRow() << j << cardinality.first << cardinality.second;
			}
			storage[i] = triplet(float80(_min), avg / ITERATIONS, float80(_max));
			csv.writeToFile(storage_directory + "/" + std::to_string(id) + ".csv");
			bar.tick();
		});

	executor.run(taskflow).get();
	bar.mark_as_completed();
	std::vector<triplet> plot_data;
	std::transform(probabilities.begin(), probabilities.end(), std::back_inserter(plot_data), [&storage](long double p) -> triplet
		{ return storage[p]; });
	indicators::show_console_cursor(true);
	CSVWriter csv;
	csv.newRow() << "probability" << "min cardinality" << "avg cardinality" << "max cardinality";
	for (uint16_t i = 0; i < probabilities.size(); i++)
	{
		csv.newRow() << to_string_with_precision(probabilities[i] * GRAPH_SIZE)
			<< to_string_with_precision(std::get<0>(storage[probabilities[i]]) / GRAPH_SIZE)
			<< to_string_with_precision(std::get<1>(storage[probabilities[i]]) / GRAPH_SIZE)
			<< to_string_with_precision(std::get<2>(storage[probabilities[i]]) / GRAPH_SIZE);
	}
	csv.writeToFile("final.csv");
}
