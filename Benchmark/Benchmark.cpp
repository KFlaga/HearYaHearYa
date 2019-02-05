#include <chrono>
#include <iostream>
#include <algorithm>
#include <string>
#include <aquila/aquila.h>
#include <Features.hpp>
#include <Dba.hpp>


namespace eave
{
	template<typename Func>
	std::chrono::nanoseconds timeIt(Func f)
	{
		auto start = std::chrono::high_resolution_clock::now();
		f();
		auto end = std::chrono::high_resolution_clock::now();
		return end - start;
	}

	struct  BenchmarkResult
	{
		int iterations;
		std::chrono::nanoseconds totalTime;
		std::chrono::nanoseconds averageTime;
	};

	template<typename Func>
	BenchmarkResult benchmark(Func f, std::chrono::seconds minTime = std::chrono::seconds{ 1 })
	{
		// warm-up
		auto time_1 = timeIt(f);

		// do warm-up for 10% of minTime
		int warmUpIterations = (int)std::ceil(minTime * 0.1 / time_1);

		for (int i = 0; i < warmUpIterations; ++i)
		{
			f();
		}

		auto start = std::chrono::high_resolution_clock::now();
		auto end = start;
		int iterations = 0;
		while (end - start < minTime)
		{
			f();
			iterations++;
			end = std::chrono::high_resolution_clock::now();
		}
		std::chrono::nanoseconds totalTime = (end - start);
		std::chrono::nanoseconds averageTime = std::chrono::duration_cast<std::chrono::nanoseconds>(totalTime / double(iterations));
		return BenchmarkResult{ iterations, totalTime, averageTime };
	}

	enum class TimeUnit
	{
		Auto = 0,
		Nano,
		Micro,
		Mili,
		Sec
	};

	std::string stringifyTime(std::chrono::nanoseconds t, TimeUnit unit)
	{
		if (unit == TimeUnit::Auto)
		{
			if (t < std::chrono::microseconds{ 100 })
			{
				return std::to_string(t.count()) + "ns";
			}
			else if (t < std::chrono::milliseconds{ 100 })
			{
				return std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(t).count()) + "us";
			}
			else if (t < std::chrono::seconds{ 100 })
			{
				return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t).count()) + "ms";
			}
			else
			{
				return std::to_string(std::chrono::duration_cast<std::chrono::seconds>(t).count()) + "s";
			}
		}
		else if (unit == TimeUnit::Nano)
		{
			return std::to_string(t.count()) + "ns";
		}
		else if (unit == TimeUnit::Micro)
		{
			return std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(t).count()) + "us";
		}
		else if (unit == TimeUnit::Mili)
		{
			return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t).count()) + "ms";
		}
		else if (unit == TimeUnit::Sec)
		{
			return std::to_string(std::chrono::duration_cast<std::chrono::seconds>(t).count()) + "s";
		}
	}

	template<typename Func>
	void executeBenchmark(Func f, const std::string& name, std::chrono::seconds minTime = std::chrono::seconds{ 1 }, TimeUnit unit = TimeUnit::Micro)
	{
		auto result = benchmark(f, minTime);
		std::cout << name << " : " << stringifyTime(result.averageTime, unit) << " (i " << result.iterations << ")\n";
	}

	auto generateRandomFeatures(int count)
	{
		return [count]() {
			Aquila::WhiteNoiseGenerator x{ 100.0 /* parameter is dummy */ };
			x.setAmplitude(1.0).generate(count);
			return std::vector<double>(std::begin(x), std::end(x));
		};
	}

	struct BenchmarkAquilaDtwData
	{
		Aquila::DtwDataType first;
		Aquila::DtwDataType second;
	};

	BenchmarkAquilaDtwData prepareBenchmarkAquilaDtwData(int lengthFirst, int lengthSecond, int lengthFeatures)
	{
		BenchmarkAquilaDtwData result;

		std::generate_n(std::back_inserter(result.first), lengthFirst, generateRandomFeatures(lengthFeatures));
		std::generate_n(std::back_inserter(result.second), lengthSecond, generateRandomFeatures(lengthFeatures));

		return result;
	}

	void benchmarkAquilaDtw(int lengthFirst, int lengthSecond, int lengthFeatures, const std::string& name)
	{
		auto data = prepareBenchmarkAquilaDtwData(lengthFirst, lengthSecond, lengthFeatures);
		executeBenchmark([&]() { Aquila::Dtw{ euclideanDistanceSquared }.getDistance(data.first, data.second); }, name);
	}

	struct BenchmarkDbaData
	{
		std::vector<FeatureVector<MFCC>> sequences;
		FeatureVector<MFCC> initialAverage;
	};

	BenchmarkDbaData prepareBenchmarkDbaData(int sequencesCount, int framesInSample, int lengthFeatures)
	{
		BenchmarkDbaData result;
		result.sequences.resize(sequencesCount);

		for (auto& s : result.sequences)
		{
			std::generate_n(std::back_inserter(s), framesInSample, generateRandomFeatures(lengthFeatures));
		}

		result.initialAverage = result.sequences[0];

		return result;
	}

	void benchmarkDba(int sequencesCount, int framesInSample, int lengthFeatures, const std::string& name)
	{
		auto data = prepareBenchmarkDbaData(sequencesCount, framesInSample, lengthFeatures);
		executeBenchmark([&]() { computeAverageSequenceWithDBA(data.sequences, data.initialAverage); }, name);
	}

	void executeBenchmarks()
	{
		std::cout << "Executing benchmarks:\n";

		std::cout << "\n";
		benchmarkAquilaDtw(10, 10, 10, "Aquila::Dtw(rand, N=10, M=10, F=10)");
		benchmarkAquilaDtw(20, 20, 10, "Aquila::Dtw(rand, N=20, M=20, F=10)");
		benchmarkAquilaDtw(40, 40, 10, "Aquila::Dtw(rand, N=40, M=40, F=10)");
		benchmarkAquilaDtw(80, 80, 10, "Aquila::Dtw(rand, N=80, M=80, F=10)");
		benchmarkAquilaDtw(160, 160, 10, "Aquila::Dtw(rand, N=160, M=160, F=10)");
		benchmarkAquilaDtw(320, 320, 10, "Aquila::Dtw(rand, N=320, M=320, F=10)");

		std::cout << "\n";
		benchmarkAquilaDtw(80, 80, 10, "Aquila::Dtw(rand, N=80, M=80, F=10)");
		benchmarkAquilaDtw(80, 80, 20, "Aquila::Dtw(rand, N=80, M=80, F=20)");
		benchmarkAquilaDtw(80, 80, 40, "Aquila::Dtw(rand, N=80, M=80, F=40)");
		benchmarkAquilaDtw(80, 80, 80, "Aquila::Dtw(rand, N=80, M=80, F=80)");

		std::cout << "\n";
		benchmarkDba(5, 10, 10, "DBA(rand, S=5, N=10, F=10)");
		benchmarkDba(5, 20, 10, "DBA(rand, S=5, N=20, F=10)");
		benchmarkDba(5, 40, 10, "DBA(rand, S=5, N=40, F=10)");
		benchmarkDba(5, 80, 10, "DBA(rand, S=5, N=80, F=10)");

		std::cout << "\n";
		benchmarkDba(10, 10, 10, "DBA(rand, S=10, N=10, F=10)");
		benchmarkDba(10, 20, 10, "DBA(rand, S=10, N=20, F=10)");
		benchmarkDba(10, 40, 10, "DBA(rand, S=10, N=40, F=10)");
		benchmarkDba(10, 80, 10, "DBA(rand, S=10, N=80, F=10)");

		std::cout << "\n";
		benchmarkDba(20, 10, 10, "DBA(rand, S=20, N=10, F=10)");
		benchmarkDba(20, 20, 10, "DBA(rand, S=20, N=20, F=10)");
		benchmarkDba(20, 40, 10, "DBA(rand, S=20, N=40, F=10)");
		benchmarkDba(20, 80, 10, "DBA(rand, S=20, N=80, F=10)");
	}
}

int main()
{
	eave::executeBenchmarks();
}
