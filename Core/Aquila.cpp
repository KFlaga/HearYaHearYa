
#include "AquilaTest.hpp"

#include <complex>
#include <cmath>
#include <aquila/aquila.h>

#include <iostream>

namespace eave
{
	void drawDtwPath(const Aquila::Dtw& dtw, std::size_t xSize, std::size_t ySize)
	{
		std::vector<std::vector<char>> data(ySize);
		for (auto it = data.rbegin(); it != data.rend(); ++it)
		{
			std::fill_n(std::back_inserter(*it), xSize, '.');
		}

		auto path = dtw.getPath();
		for (auto it = path.begin(); it != path.end(); ++it)
		{
			data[it->x][it->y] = 'o';
		}

		for (auto it = data.rbegin(); it != data.rend(); ++it)
		{
			std::copy(
				std::begin(*it),
				std::end(*it),
				std::ostream_iterator<char>(std::cout, " ")
			);
			std::cout << "\n";
		}
	}

	void testLoadWav()
	{
		std::string path = "nuta.wav";

		Aquila::WaveFile wavFile{ path };

		std::cout << "samples " << wavFile.getSamplesCount() << "\n";
		std::cout << "length " << wavFile.getSampleFrequency() << "\n";

		//Aquila::TextPlot plot("Sound");

		//plot.setSize(5, 10);


		//std::size_t fftSize = 64;

		//auto fftComp = Aquila::FftFactory::getFft(fftSize);

		//auto* data = wavFile.toArray();
		//std::vector<double> signal{ data + 1000, data + 1000 + fftSize };

		//auto signalFft = fftComp->fft(signal.data());

		//std::vector<double> fftMod(fftSize);
		//std::transform(std::begin(signalFft), std::end(signalFft), std::begin(fftMod), [](std::complex<double> x)
		//{
		//	return std::abs(x);
		//});

		//plot.plot(fftMod);

	/*	std::size_t SIZE = 1024;
		Aquila::FrequencyType sampleFrequency = 1024;

		Aquila::SineGenerator input(sampleFrequency);
		input.setAmplitude(5).setFrequency(64).generate(SIZE);

		Aquila::Mfcc mfcc2(input.getSamplesCount());
		auto mfccValues = mfcc2.calculate(input);*/
		
		// TODO: below is broken when used with MFCC
		//Aquila::FramesCollection frames = Aquila::FramesCollection::createFromDuration(wavFile, 20.0, 0.5);

		/*Aquila::FramesCollection frames{ wavFile, unsigned int(SIZE), 0 };
		std::size_t frameSize = frames.getSamplesPerFrame();

		Aquila::Mfcc mfcc{ frameSize };

		std::size_t featuresCount = 12;

		std::vector<std::vector<double>> features{};
		features.reserve(frames.count());
		for (auto& fr : frames)
		{
			auto frameFeatures = mfcc.calculate(fr);
			features.emplace_back(std::move(frameFeatures));
		}

		std::cout << "FEATURES:\n";
		for (double f : features[frames.count() / 2])
		{
			std::cout << f << "\n";
		}

		const std::size_t X_SIZE = 24, Y_SIZE = 24;
		Aquila::DtwDataType from, to;

		std::generate_n(std::back_inserter(from), Y_SIZE, []() {
			std::vector<double> features;
			features.push_back(Aquila::randomDouble());
			features.push_back(Aquila::randomDouble());
			features.push_back(Aquila::randomDouble());
			return features;
		});

		std::generate_n(std::back_inserter(to), X_SIZE, []() {
			std::vector<double> features;
			features.push_back(Aquila::randomDouble());
			features.push_back(Aquila::randomDouble());
			features.push_back(Aquila::randomDouble());
			return features;
		});

		Aquila::Dtw dtw;
		double distance = dtw.getDistance(from, to);
		std::cout << "DTW distance (neighbors): " << distance << "\n";
		drawDtwPath(dtw, X_SIZE, Y_SIZE);

		Aquila::Dtw dtwDiag(Aquila::euclideanDistance, Aquila::Dtw::Diagonals);
		distance = dtwDiag.getDistance(from, to);
		std::cout << "DTW distance (diagonals): " << distance << "\n";
		drawDtwPath(dtwDiag, X_SIZE, Y_SIZE);*/
	}
}
