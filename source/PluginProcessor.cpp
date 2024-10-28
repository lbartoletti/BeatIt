#include "PluginProcessor.h"
#include "NotationManager.h"
#include "PluginEditor.h"
#include <map>

namespace
{
    // Audio Constants
    constexpr double DEFAULT_SAMPLE_RATE = 44100.0;
    constexpr double MIN_BPM = 1.0f;
    constexpr double MAX_BPM = 500.0f;
    constexpr double DEFAULT_BPM = 120.0f;

    // Click Sound Parameters
    namespace ClickParams
    {
        // High Click
        constexpr float HIGH_FREQUENCY = 1500.0f;
        constexpr float HIGH_DURATION_MS = 30.0f;

        // Low Click
        constexpr float LOW_FREQUENCY = 800.0f;
        constexpr float LOW_DURATION_MS = 20.0f;

        // Mute
        constexpr float ATTACK_TIME_MS = 1.0f;
        constexpr float DEFAULT_AMPLITUDE = 0.5f;

        // Rest sound
        constexpr float REST_SOUND_FREQUENCY = 200.0f;
        constexpr float REST_SOUND_DURATION_MS = 15.0f;
    }
}

//==============================================================================
// Constructor and Destructor
//==============================================================================
MetronomeAudioProcessor::MetronomeAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    initializeParameters();
    initializeAudioState();
    initializeSoundMaps();
    mutedBeats.resize (static_cast<size_t> (getBeatsPerBar()), false);
}

MetronomeAudioProcessor::~MetronomeAudioProcessor()
{
    state->removeParameterListener ("firstBeatSound", this);
    state->removeParameterListener ("otherBeatsSound", this);
}

//==============================================================================
// Initialization Methods
//==============================================================================
void MetronomeAudioProcessor::initializeParameters()
{
    // Create parameter layout
    state = std::make_unique<juce::AudioProcessorValueTreeState> (*this, nullptr, "Parameters", juce::AudioProcessorValueTreeState::ParameterLayout {
                                                                                                    std::make_unique<juce::AudioParameterInt> ("bpm", "BPM", static_cast<int> (MIN_BPM), static_cast<int> (MAX_BPM), static_cast<int> (DEFAULT_BPM)),

                                                                                                    std::make_unique<juce::AudioParameterBool> ("play", "Play", false),

                                                                                                    std::make_unique<juce::AudioParameterChoice> ("beatsPerBar", "Beats Per Bar", juce::StringArray { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" }, 3),

                                                                                                    std::make_unique<juce::AudioParameterChoice> ("beatDenominator", "Beat Denominator", juce::StringArray { "1", "2", "4", "8" }, 2),

                                                                                                    std::make_unique<juce::AudioParameterChoice> ("firstBeatSound", "First Beat Sound", juce::StringArray { "High Click", "Low Click", "Mute" }, 0),

                                                                                                    std::make_unique<juce::AudioParameterChoice> ("otherBeatsSound", "Other Beats Sound", juce::StringArray { "High Click", "Low Click", "Mute" }, 1),

                                                                                                    std::make_unique<juce::AudioParameterChoice> ("restSound", "Rest Sound", juce::StringArray { "Same as Beat", "Rest Sound", "Mute" }, 2),

                                                                                                    std::make_unique<juce::AudioParameterChoice> ("subdivision", "Beat Subdivision",
                                                                                                        juce::StringArray {
                                                                                                            "No Subdivision", // 0
                                                                                                            "Half", // 1
                                                                                                            "Half + Rest", // 2
                                                                                                            "Rest + Half", // 3
                                                                                                            "Triplet", // 4
                                                                                                            "Rest + Half + Half Triplet", // 5
                                                                                                            "Half + Rest + Half Triplet", // 6
                                                                                                            "Half + Half + Rest Triplet", // 7
                                                                                                            "Rest + Half + Rest Triplet", // 8
                                                                                                            "Quarter", // 9
                                                                                                            "Rest + Eighth Pattern", // 10
                                                                                                            "Eighth + Eighth + Quarter", // 11
                                                                                                            "Quarter + Eighth + Eighth", // 12
                                                                                                            "Eighth + Quarter + Eighth" // 13
                                                                                                        },
                                                                                                        0),
        std::make_unique<juce::AudioParameterChoice>("restSound", "Rest Sound",
                juce::StringArray{"Same as Beat", "Debug Sound", "Mute"},
                0),
                                                                                                });

    // Get parameter pointers
    bpmParameter = state->getRawParameterValue ("bpm");
    playParameter = state->getRawParameterValue ("play");
    beatsPerBarParameter = state->getRawParameterValue ("beatsPerBar");
    beatDenominatorParameter = state->getRawParameterValue ("beatDenominator");
    firstBeatSoundParameter = state->getRawParameterValue ("firstBeatSound");
    otherBeatsSoundParameter = state->getRawParameterValue ("otherBeatsSound");
    restSoundParameter = state->getRawParameterValue ("restSound");
    subdivisionParameter = state->getRawParameterValue ("subdivision");

    // Add parameter listeners
    state->addParameterListener ("firstBeatSound", this);
    state->addParameterListener ("otherBeatsSound", this);
}

void MetronomeAudioProcessor::initializeAudioState()
{
    currentSampleRate = DEFAULT_SAMPLE_RATE;
    currentBeat = 0;
    soundPosition = 0;
    samplesPerBeat = 0;
}

void MetronomeAudioProcessor::initializeSoundMaps()
{
    // Initialize click type mapping
    soundTypeMap["High Click"] = ClickType::High;
    soundTypeMap["Low Click"] = ClickType::Low;
    soundTypeMap["Mute"] = ClickType::Mute;

    // Generate initial sounds
    initializeSounds();
}

//==============================================================================
// Audio Processing
//==============================================================================
void MetronomeAudioProcessor::prepareToPlay (double sampleRate, [[maybe_unused]] int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    initializeSounds();
    updateTimingInfo();
}

void MetronomeAudioProcessor::releaseResources()
{
    // Nothing to release
}

void MetronomeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
    [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear output buffers
    for (auto i = 0; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    static float lastBpm = bpmParameter->load();
    float currentBpm = bpmParameter->load();

    if (std::abs (lastBpm - currentBpm) > 0.01f && getPlayState())
    {
        // Stop sound
        clickPosition = -1; // Stop current click
        soundPosition = 0; // Reset position
        currentBeat = 0; // Reset beat

        updateTimingInfo();
        lastBpm = currentBpm;

        return;
    }

    if (getPlayState())
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            processSample (buffer, sample, totalNumOutputChannels);
        }
    }
}

void MetronomeAudioProcessor::processSample (juce::AudioBuffer<float>& buffer,
    int sample,
    int totalNumOutputChannels)
{
    bool startClick = false;
    bool isRest = false;

    if (soundPosition == 0)
    {
        startClick = true;
    }
    else if (getPlayState())
    {
        const auto subdivType = static_cast<Subdivision> (static_cast<int> (subdivisionParameter->load()));
        startClick = processSubdivisionClick (subdivType, soundPosition, isRest);
    }

    // Generate click if needed and not muted
    if (currentBeat >= 0 && currentBeat < getBeatsPerBar() && static_cast<size_t> (currentBeat) < mutedBeats.size() && !mutedBeats[currentBeat])
    {
        if (startClick)
        {
            clickPosition = 0;
            currentClickIsRest = isRest;
        }

        if (clickPosition >= 0)
        {
            const auto restType = static_cast<RestSoundType> (
                static_cast<int> (restSoundParameter->load()));

            const auto& soundBuffer = [this, restType]() -> const juce::AudioBuffer<float>& {
                if (currentClickIsRest)
                {
                    switch (restType)
                    {
                        case RestSoundType::SameAsBeat:
                            return getSoundBufferForClickType (soundTypeMap[(currentBeat == 0) ? state->getParameter ("firstBeatSound")->getCurrentValueAsText() : state->getParameter ("otherBeatsSound")->getCurrentValueAsText()]);

                        case RestSoundType::RestSound:
                            return restSoundBuffer;

                        case RestSoundType::Mute:
                            return muteBuffer;

                        default:
                            return muteBuffer;
                    }
                }
                else
                {
                    return getSoundBufferForClickType (soundTypeMap[(currentBeat == 0) ? state->getParameter ("firstBeatSound")->getCurrentValueAsText() : state->getParameter ("otherBeatsSound")->getCurrentValueAsText()]);
                }
            }();

            if (clickPosition < soundBuffer.getNumSamples())
            {
                float sampleValue = soundBuffer.getSample (0, clickPosition);
                for (int channel = 0; channel < totalNumOutputChannels; ++channel)
                {
                    buffer.setSample (channel, sample, sampleValue);
                }
                clickPosition++;
            }
            else
            {
                clickPosition = -1;
            }
        }
    }

    soundPosition++;
    if (soundPosition >= samplesPerBeat)
    {
        soundPosition = 0;
        currentBeat = (currentBeat + 1) % getBeatsPerBar();
    }
}

//==============================================================================
// Sound Generation
//==============================================================================
void MetronomeAudioProcessor::generateClickSound (juce::AudioBuffer<float>& buffer, ClickType type)
{
    const double sampleRate = getSampleRate();

    // Set parameters based on click type
    float frequency = 0.0f;
    float durationMs = 0.0f;

    switch (type)
    {
        case ClickType::High:
            frequency = ClickParams::HIGH_FREQUENCY;
            durationMs = ClickParams::HIGH_DURATION_MS;
            break;
        case ClickType::Low:
            frequency = ClickParams::LOW_FREQUENCY;
            durationMs = ClickParams::LOW_DURATION_MS;
            break;
        case ClickType::Mute:
            durationMs = 1.0f; // Minimal buffer for mute
            frequency = 0.0f;
            break;
    }

    const int numSamples = static_cast<int> ((durationMs / 1000.0f) * sampleRate);
    buffer.setSize (1, numSamples);
    buffer.clear();

    if (type != ClickType::Mute)
    {
        generateClickWaveform (buffer, frequency, sampleRate, durationMs);
    }
}

void MetronomeAudioProcessor::generateClickWaveform (juce::AudioBuffer<float>& buffer,
    float frequency,
    double sampleRate,
    float durationMs)
{
    const auto attackTime = static_cast<float> (ClickParams::ATTACK_TIME_MS / 1000.0);
    const auto decayTime = static_cast<float> ((durationMs / 1000.0) - attackTime);
    const auto attackSamples = static_cast<float> (attackTime * sampleRate);
    const auto decaySamples = static_cast<float> (decayTime * sampleRate);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const auto time = static_cast<float> (sample) / static_cast<float> (sampleRate);
        auto signalValue = std::sin (2.0f * juce::MathConstants<float>::pi * frequency * time);

        float envelope;
        if (static_cast<float> (sample) < attackSamples)
            envelope = static_cast<float> (sample) / attackSamples;
        else
            envelope = 1.0f - ((static_cast<float> (sample) - attackSamples) / decaySamples);

        envelope = std::max (0.0f, std::min (1.0f, envelope));
        buffer.setSample (0, sample, signalValue * envelope * ClickParams::DEFAULT_AMPLITUDE);
    }
}

const juce::AudioBuffer<float>& MetronomeAudioProcessor::getSoundBufferForClickType (ClickType type) const
{
    switch (type)
    {
        case ClickType::High:
            return highClickBuffer;
        case ClickType::Low:
            return lowClickBuffer;
        default:
            return muteBuffer;
    }
}

void MetronomeAudioProcessor::initializeSounds()
{
    generateClickSound (highClickBuffer, ClickType::High);
    generateClickSound (lowClickBuffer, ClickType::Low);
    generateClickSound (muteBuffer, ClickType::Mute);


    // Rest Sound
    juce::AudioBuffer<float> tempBuffer;
    const double sampleRate = getSampleRate();
    const int numSamples = static_cast<int> ((ClickParams::REST_SOUND_DURATION_MS / 1000.0f) * sampleRate);
    restSoundBuffer.setSize (1, numSamples);
    restSoundBuffer.clear();

    // Low frequency sinus
    for (int sample = 0; sample < numSamples; ++sample)
    {
        const auto time = static_cast<float> (sample) / static_cast<float> (sampleRate);
        auto signalValue = std::sin (2.0f * juce::MathConstants<float>::pi * ClickParams::REST_SOUND_FREQUENCY * time);

        float envelope = 1.0f - std::abs (2.0f * sample / numSamples - 1.0f);
        restSoundBuffer.setSample (0, sample, signalValue * envelope * 0.3f);
    }
}

//==============================================================================
// Timing and Beat Management
//==============================================================================
void MetronomeAudioProcessor::updateTimingInfo()
{
    int bpm = static_cast<int> (std::round (bpmParameter->load()));
    int denominator = getBeatDenominator();

    double adjustedBpm = static_cast<double> (bpm * (4.0 / denominator));
    double beatsPerSecond = adjustedBpm / 60.0;

    if (currentSampleRate > 0 && beatsPerSecond > 0)
    {
        samplesPerBeat = static_cast<int> (currentSampleRate / beatsPerSecond);
    }
}

//==============================================================================
// Parameter Management
//==============================================================================
void MetronomeAudioProcessor::parameterChanged (const juce::String& parameterID,
    [[maybe_unused]] float newValue)
{
    if (parameterID == "bpm")
    {
        updateTimingInfo();
    }
    else if (parameterID == "beatDenominator" || parameterID == "beatsPerBar")
    {
        updateTimingInfo();
        updateMutedBeatsSize();
        currentBeat = 0;

        if (parameterID == "beatDenominator")
        {
            if (auto* editor = dynamic_cast<MetronomeAudioProcessorEditor*> (getActiveEditor()))
            {
                editor->updateSubdivisionComboBox (getBeatDenominator());
            }
        }
    }
}

//==============================================================================
// State Getters
//==============================================================================
bool MetronomeAudioProcessor::getPlayState() const
{
    return playParameter->load() > 0.5f;
}

void MetronomeAudioProcessor::togglePlayState()
{
    bool newState = !getPlayState();
    state->getParameter ("play")->setValueNotifyingHost (newState ? 1.0f : 0.0f);

    if (newState)
    {
        updateTimingInfo();
        soundPosition = 0;
        currentBeat = 0;
    }
}

int MetronomeAudioProcessor::getBeatsPerBar() const
{
    return static_cast<int> (beatsPerBarParameter->load()) + 1;
}

int MetronomeAudioProcessor::getBeatDenominator() const
{
    return 1 << static_cast<int> (beatDenominatorParameter->load());
}

void MetronomeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto stateTree = state->copyState();

    juce::String mutedBeatsStr;
    for (size_t i = 0; i < mutedBeats.size(); ++i)
    {
        mutedBeatsStr += mutedBeats[i] ? "1" : "0";
        if (i < mutedBeats.size() - 1)
            mutedBeatsStr += ",";
    }

    stateTree.setProperty ("mutedBeats", mutedBeatsStr, nullptr);

    std::unique_ptr<juce::XmlElement> xml (stateTree.createXml());
    copyXmlToBinary (*xml, destData);
}

void MetronomeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        auto tree = juce::ValueTree::fromXml (*xmlState);
        if (tree.isValid())
        {
            state->replaceState (tree);

            juce::String mutedBeatsStr = tree.getProperty ("mutedBeats", "");
            if (mutedBeatsStr.isNotEmpty())
            {
                auto tokens = juce::StringArray::fromTokens (mutedBeatsStr, ",", "");
                mutedBeats.clear();
                for (const auto& token : tokens)
                {
                    mutedBeats.push_back (token == "1");
                }
                updateMutedBeatsSize();
            }
        }
    }
}

//==============================================================================
// Beats muted
//==============================================================================
void MetronomeAudioProcessor::initializeMutedBeats()
{
    mutedBeats.clear();
    mutedBeats.resize (static_cast<size_t> (getBeatsPerBar()), false);
}

void MetronomeAudioProcessor::updateMutedBeatsSize()
{
    const auto newSize = static_cast<size_t> (getBeatsPerBar());
    if (mutedBeats.size() != newSize)
    {
        std::vector<bool> oldStates = mutedBeats;
        mutedBeats.resize (newSize, false);

        for (size_t i = 0; i < std::min (oldStates.size(), mutedBeats.size()); ++i)
        {
            mutedBeats[i] = oldStates[i];
        }
    }
}

void MetronomeAudioProcessor::toggleBeatMute (int beatIndex)
{
    if (beatIndex >= 0 && static_cast<size_t> (beatIndex) < mutedBeats.size())
    {
        mutedBeats[static_cast<size_t> (beatIndex)] = !mutedBeats[static_cast<size_t> (beatIndex)];
    }
}

bool MetronomeAudioProcessor::isBeatMuted (int beatIndex) const
{
    if (beatIndex >= 0 && static_cast<size_t> (beatIndex) < mutedBeats.size())
    {
        return mutedBeats[static_cast<size_t> (beatIndex)];
    }
    return false;
}

void MetronomeAudioProcessor::setMutedBeats (const std::vector<bool>& newMutedBeats)
{
    mutedBeats = newMutedBeats;
    updateMutedBeatsSize();
}

//==============================================================================
// Plugin Information
//==============================================================================
const juce::String MetronomeAudioProcessor::getName() const { return JucePlugin_Name; }
bool MetronomeAudioProcessor::acceptsMidi() const { return false; }
bool MetronomeAudioProcessor::producesMidi() const { return false; }
bool MetronomeAudioProcessor::isMidiEffect() const { return false; }
double MetronomeAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int MetronomeAudioProcessor::getNumPrograms() { return 1; }
int MetronomeAudioProcessor::getCurrentProgram() { return 0; }
void MetronomeAudioProcessor::setCurrentProgram ([[maybe_unused]] int index) {}
const juce::String MetronomeAudioProcessor::getProgramName ([[maybe_unused]] int index) { return {}; }
void MetronomeAudioProcessor::changeProgramName ([[maybe_unused]] int index, [[maybe_unused]] const juce::String& newName) {}

//==============================================================================
// Editor
//==============================================================================
bool MetronomeAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* MetronomeAudioProcessor::createEditor()
{
    return new MetronomeAudioProcessorEditor (*this);
}

//==============================================================================
// Plugin Creation
//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MetronomeAudioProcessor();
}

//==============================================================================
// Tap Tempo
//==============================================================================
void MetronomeAudioProcessor::processTapTempo()
{
    tapTempoCalculator.tap();
    double newBpm = tapTempoCalculator.calculateBPM();

    // Round to nearest integer BPM
    newBpm = std::round (newBpm);

    // Clamp BPM to valid range
    newBpm = std::clamp (newBpm, MIN_BPM, MAX_BPM);

    // Update BPM parameter
    float normalizedBpm = static_cast<float> (state->getParameter ("bpm")->convertTo0to1 (newBpm));
    state->getParameter ("bpm")->setValueNotifyingHost (normalizedBpm);
}

/**
 * Process click timing based on current subdivision
 * @param subdivision Current subdivision type
 * @param currentPosition Position in the current beat (in samples)
 * @param isRest Output parameter indicating if current position is a rest
 * @return true if a click should be triggered at this position
 */
bool MetronomeAudioProcessor::processSubdivisionClick (Subdivision subdivision,
    int currentPosition,
    bool& isRest)
{
    isRest = false; // By default, not a rest

    switch (subdivision)
    {
        case Subdivision::NoSubdivision:
            return false;

        case Subdivision::Half: // Two equal notes
            return (currentPosition == samplesPerBeat / 2);

        case Subdivision::HalfAndRest: // Note + Rest
            if (currentPosition == samplesPerBeat / 2)
            {
                isRest = true; // This is the rest
                return true; // Always return true, sound selection handled in processSample
            }
            return false;

        case Subdivision::RestHalf: // Rest + Note
            if (currentPosition == 0)
            {
                isRest = true;
                return true;
            }
            return (currentPosition == samplesPerBeat / 2);

        case Subdivision::Triplet: // Three equal notes
        {
            int tripletInterval = samplesPerBeat / 3;
            return (currentPosition == tripletInterval || currentPosition == tripletInterval * 2);
        }

        case Subdivision::RestHalfHalfTriplet: // Rest + Two Notes (triplet)
        {
            int tripletInterval = samplesPerBeat / 3;
            if (currentPosition == 0)
            {
                isRest = true;
                return true;
            }
            return (currentPosition == tripletInterval || currentPosition == tripletInterval * 2);
        }

        case Subdivision::HalfRestHalfTriplet: // Note + Rest + Note (triplet)
        {
            int tripletInterval = samplesPerBeat / 3;
            if (currentPosition == tripletInterval)
            {
                isRest = true;
                return true;
            }
            return currentPosition == tripletInterval * 2;
        }

        case Subdivision::HalfHalfRestTriplet: // Two Notes + Rest (triplet)
        {
            int tripletInterval = samplesPerBeat / 3;
            if (currentPosition == tripletInterval * 2)
            {
                isRest = true;
                return true;
            }
            return currentPosition == tripletInterval;
        }

        case Subdivision::RestHalfRestTriplet: // Rest + Note + Rest (triplet)
        {
            int tripletInterval = samplesPerBeat / 3;
            if (currentPosition == 0 || currentPosition == tripletInterval * 2)
            {
                isRest = true;
                return true;
            }
            return currentPosition == tripletInterval;
        }

        case Subdivision::Quarter: // Four equal notes
        {
            int quarterInterval = samplesPerBeat / 4;
            return (currentPosition == quarterInterval || currentPosition == quarterInterval * 2 || currentPosition == quarterInterval * 3);
        }

        case Subdivision::RestEighthPattern: // Rest + Note + Rest + Note
        {
            int quarterInterval = samplesPerBeat / 4;
            if (currentPosition == 0 || currentPosition == quarterInterval * 2)
            {
                isRest = true;
                return true;
            }
            return (currentPosition == quarterInterval || currentPosition == quarterInterval * 3);
        }

        case Subdivision::EighthEighthQuarter: // Two short + long
        {
            int quarterInterval = samplesPerBeat / 4;
            return (currentPosition == quarterInterval);
        }

        case Subdivision::QuarterEighthEighth: // Long + two short
        {
            int quarterInterval = samplesPerBeat / 4;
            return (currentPosition == quarterInterval * 2 || currentPosition == quarterInterval * 3);
        }

        case Subdivision::EighthQuarterEighth: // Short + long + short
        {
            int quarterInterval = samplesPerBeat / 4;
            return (currentPosition == quarterInterval || currentPosition == quarterInterval * 3);
        }

        case Subdivision::Count:
        default:
            return false;
    }
}