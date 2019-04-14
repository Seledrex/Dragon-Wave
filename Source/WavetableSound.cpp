/*
  ==============================================================================

	WavetableSound.cpp
	Created: 22 Mar 2019 12:14:37pm
	Author:  Eric

  ==============================================================================
*/

#include "WavetableSound.h"

WavetableSound::WavetableSound()
{
}

WavetableSound::WavetableSound(const WavetableSound& other)
{
	initialized = other.initialized;
	type = other.type;

	// Copy wavetables
	wavetables.makeCopyOf(other.wavetables);

	// Copy wavetable map
	std::copy(other.wavetableMap.begin(),
		other.wavetableMap.end(),
		std::inserter(wavetableMap, wavetableMap.begin()));

	// Copy wavetable frequencies
	std::copy(other.wavetableFrequencies.begin(),
		other.wavetableFrequencies.end(),
		std::back_inserter(wavetableFrequencies));
}

//==============================================================================
bool WavetableSound::appliesToNote(int)
{
	return true;
}

bool WavetableSound::appliesToChannel(int)
{
	return true;
}

//==============================================================================
void WavetableSound::makeArbitrary(std::vector<float> inputWavetable)
{
	// If already initialized or wrong size, do nothing
	if (initialized || inputWavetable.size() != wavetableSize)
		return;

	// Calculate constants
	int fftSize = (int)std::pow(2, fftOrder);
	int nyquist = samplingRate / 2;
	float baseFrequency = (float)fftSize / (float)wavetableSize;

	// Duplicate table
	inputWavetable = Util::duplicateTable(3, inputWavetable);

	// Create FFT object
	dsp::FFT fftObject(fftOrder);

	// Convert the input wavetable into the frequency domain
	std::vector<float> inputWavetableFFT(fftObject.getSize() * 2, 0);
	inputWavetableFFT.insert(inputWavetableFFT.begin(), inputWavetable.begin(), inputWavetable.end());
	fftObject.performRealOnlyForwardTransform(&inputWavetableFFT[0]);

	// Calculate partials needed for each table frequency
	auto partials = Functional::map(
		wavetableFrequencies,
		[nyquist](float wavetableFrequency) -> float {
			return (float)nyquist / wavetableFrequency - 1;
		},
		std::vector<float>());

	// Calculate cutoff frequencies for each table frequency
	auto cutoffFrequencies = Functional::map(
		partials,
		[baseFrequency](float partial) -> int {
			return roundToInt(partial * baseFrequency);
		},
		std::vector<int>());

	// For each cutoff frequency create its corresponding frequency domain
	auto outputWavetableFFTs = Functional::map(
		cutoffFrequencies,
		[inputWavetableFFT](int cutoffFrequency) -> std::vector<float> {
			std::vector<float> outputWavetableFFT(inputWavetableFFT.size(), 0.0);
			for (int i = 0; i < cutoffFrequency; i++)
			{
				outputWavetableFFT[i * 2] = inputWavetableFFT[i * 2];
				outputWavetableFFT[i * 2 + 1] = inputWavetableFFT[i * 2 + 1];
			}
			return outputWavetableFFT;
		},
		std::vector<std::vector<float>>());
	
	// Revert back to the time domain for each frequency domain
	auto outputWavetables = Functional::map(
		outputWavetableFFTs,
		[this, &fftObject](std::vector<float> outputWavetableFFT) -> std::vector<float> {
			fftObject.performRealOnlyInverseTransform(&outputWavetableFFT[0]);
			std::vector<float> outTable(wavetableSize);
			for (int i = 0; i < wavetableSize; i++)
				outTable[i] = outputWavetableFFT[i + wavetableSize];
			return outTable;
		},
		std::vector<std::vector<float>>());

	// Set buffer size
	wavetables.setSize((int)wavetableFrequencies.size() + 2, wavetableSize + 1);

	// Copy inputWavetable to the first channel of the buffer
	auto* samples = wavetables.getWritePointer(0);

	for (int i = 0; i < wavetableSize; i++)
		samples[i] = inputWavetable[i];

	samples[wavetableSize] = samples[0];

	// Each subsequent channel is another wavetable
	for (int i = 0; i < outputWavetables.size(); i++)
	{
		samples = wavetables.getWritePointer(i + 1);

		for (int j = 0; j < wavetableSize; j++)
			samples[j] = outputWavetables[i][j];

		samples[wavetableSize] = samples[0];
	}

	// Make the last channel a sine wave
	samples = wavetables.getWritePointer(wavetables.getNumChannels() - 1);
	float angleDelta = MathConstants<float>::twoPi / wavetableSize;
	float currentAngle = 0.0;

	for (int i = 0; i < wavetableSize; i++)
	{
		samples[i] = std::sin(currentAngle);
		currentAngle += angleDelta;
	}

	samples[wavetableSize] = samples[0];

	updateWavetableMap();
	type = Waveform::Arbitrary;
	initialized = true;
}

void WavetableSound::makeSine()
{
	wavetables.setSize((int)wavetableFrequencies.size() + 2, wavetableSize + 1);

	float angleDelta = MathConstants<float>::twoPi / wavetableSize;
	float currentAngle = 0.0;

	for (int i = 0; i < wavetableSize; i++)
	{
		float sample = std::sin(currentAngle);

		for (int channel = 0; channel < wavetables.getNumChannels(); channel++)
		{
			auto* samples = wavetables.getWritePointer(channel);
			samples[i] = sample;
		}

		currentAngle += angleDelta;
	}

	for (int channel = 0; channel < wavetables.getNumChannels(); channel++)
	{
		auto* samples = wavetables.getWritePointer(channel);
		samples[wavetableSize] = samples[0];
	}

	updateWavetableMap();
	type = Waveform::Sine;
	initialized = true;
}

void WavetableSound::makeTriangle()
{
	auto triangle = Util::linspace(0.0f, 1.0f, wavetableSize / 4);
	auto mid = Util::linspace(1.0f, -1.0f, wavetableSize / 2);
	auto end = Util::linspace(-1.0f, 0.0f, wavetableSize / 4);
	triangle.insert(triangle.end(), mid.begin(), mid.end());
	triangle.insert(triangle.end(), end.begin(), end.end());
	makeArbitrary(triangle);
	type = Waveform::Triangle;
	initialized = true;
}

void WavetableSound::makeSawtooth()
{
	auto sawtooth = Util::linspace(0.0f, 1.0f, wavetableSize / 2);
	auto sawSecondHalf = Util::linspace(-1.0f, 0.0f, wavetableSize / 2);
	sawtooth.insert(sawtooth.end(), sawSecondHalf.begin(), sawSecondHalf.end());
	makeArbitrary(sawtooth);
	type = Waveform::Sawtooth;
	initialized = true;
}

void WavetableSound::makeSquare()
{
	auto square = std::vector<float>(wavetableSize, -1.0);

	for (int i = 0; i < wavetableSize / 2; i++)
		square[i] = 1.0;

	makeArbitrary(square);
	type = Waveform::Square;
	initialized = true;
}

void WavetableSound::makeNoise()
{
	wavetables.setSize((int)wavetableFrequencies.size() + 2, wavetableSize + 1);

	Random random;

	for (int i = 0; i <= wavetableSize; i++)
	{
		for (int channel = 0; channel < wavetables.getNumChannels(); channel++)
		{
			auto* samples = wavetables.getWritePointer(channel);
			samples[i] = random.nextFloat() * 2.0f - 1.0f;
		}
	}

	updateWavetableMap();
	type = Waveform::Noise;
	initialized = true;
}

//==============================================================================
AudioSampleBuffer& WavetableSound::getWavetables()
{
	return wavetables;
}

int WavetableSound::getWavetableSize()
{
	return wavetableSize;
}

WavetableSound::Waveform WavetableSound::getWaveformType()
{
	return type;
}

bool WavetableSound::isInitialized()
{
	return initialized;
}

//==============================================================================
std::pair<float, float> WavetableSound::getBoundingFrequencies(float target)
{
	int n = (int)wavetableFrequencies.size();

	if (target <= wavetableFrequencies[0])
		return std::pair<float, float>(
			wavetableFrequencies[0],
			wavetableFrequencies[0]);

	if (target >= wavetableFrequencies[n - 1])
		return std::pair<float, float>(
			wavetableFrequencies[n - 1],
			wavetableFrequencies[n - 1]);

	int low = 0, high = n, mid = 0;

	while (low < high)
	{
		mid = (low + high) / 2;

		if (wavetableFrequencies[mid] == target)
			return std::pair<float, float>(
				wavetableFrequencies[mid],
				wavetableFrequencies[mid]);

		if (target < wavetableFrequencies[mid])
		{
			if (mid > 0 && target > wavetableFrequencies[mid - 1])
				return std::pair<float, float>(
					wavetableFrequencies[mid - 1],
					wavetableFrequencies[mid]);
			high = mid;
		}
		else
		{
			if (mid < n - 1 && target < wavetableFrequencies[mid + 1])
				return std::pair<float, float>(
					wavetableFrequencies[mid],
					wavetableFrequencies[mid + 1]);
			low = mid + 1;
		}
	}

	return std::pair<float, float>(
		wavetableFrequencies[mid],
		wavetableFrequencies[mid]);
}

std::pair<int, int> WavetableSound::getBoundingIndexes(std::pair<float, float> targetFrequencies)
{
	auto lo = wavetableMap.find(targetFrequencies.first);
	auto hi = wavetableMap.find(targetFrequencies.second);
	return std::pair<int, int>(lo->second, hi->second);
}

//==============================================================================
void WavetableSound::updateWavetableMap()
{
	// Put all the indexes into the table map with their corresponding frequency
	for (int i = 0; i < wavetableFrequencies.size(); i++)
		wavetableMap.insert(std::pair<float, int>(wavetableFrequencies[i], i + 1));

	// Set the input wavetable to be the lowest wavetable
	wavetableMap.insert(std::pair<float, int>(lowestFrequency, 0));
	wavetableFrequencies.insert(wavetableFrequencies.begin(), lowestFrequency);

	// Set highest wavetable to be a sine wave
	wavetableMap.insert(std::pair<float, int>(highestFrequency, wavetables.getNumChannels() - 1));
	wavetableFrequencies.push_back(highestFrequency);
}
