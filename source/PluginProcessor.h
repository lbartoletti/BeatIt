#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#if (MSVC)
    #include "ipps.h"
#endif

/**
 * @file PluginProcessor.h
 * @brief Core processor implementation for the BeatIt metronome plugin
 * @author Lituus (Loïc Bartoletti)
 * @version 0.0.1
 */

/**
 * @brief Calculator for tap tempo functionality
 * 
 * This class handles the tap tempo mechanism by:
 * - Recording intervals between taps
 * - Computing average BPM from these intervals
 * - Handling edge cases (too fast/slow taps)
 * - Auto-resetting after timeout
 */
class TapTempoCalculator
{
public:
    /**
     * @brief Process a new tap event
     * 
     * Records the time of the tap and calculates the interval from the last tap.
     * Implements the following rules:
     * - Ignores intervals shorter than 120ms (> 500 BPM)
     * - Ignores intervals longer than 2000ms (< 30 BPM)
     * - Maintains a history of up to 4 intervals
     * - Auto-resets if no tap received within 2 seconds
     */
    void tap()
    {
        auto currentTime = juce::Time::getMillisecondCounterHiRes();

        if (!lastTapTime.has_value())
        {
            lastTapTime = currentTime;
            return;
        }

        // Calculate interval since last tap
        auto interval = currentTime - *lastTapTime;
        lastTapTime = currentTime;

        // Ignore taps that would result in BPM outside valid range (30-500 BPM)
        if (interval < 120.0 || interval > 2000.0)
        {
            reset();
            return;
        }

        // Add interval to history
        intervals.push_back (interval);

        // Keep only the last 4 intervals for average calculation
        if (intervals.size() > 4)
            intervals.erase (intervals.begin());

        // Reset if no tap received within 2 seconds
        if (currentTime - *lastTapTime > 2000.0)
            reset();
    }

    /**
     * @brief Calculate the current BPM based on recorded tap intervals
     * 
     * Computes the average interval from the recorded tap history and
     * converts it to BPM (beats per minute).
     * 
     * @return double The calculated BPM, defaults to 120.0 if no intervals recorded
     */
    double calculateBPM() const
    {
        if (intervals.empty())
            return 120.0;

        // Calculate average interval
        double averageInterval = 0.0;
        for (const auto& interval : intervals)
            averageInterval += interval;

        averageInterval /= intervals.size();

        // Convert average interval to BPM (60000ms = 1 minute)
        return 60000.0 / averageInterval;
    }

    /**
     * @brief Reset the tap tempo calculator
     * 
     * Clears all recorded intervals and last tap time.
     * Called when:
     * - A tap is too fast or too slow
     * - No tap received within timeout period
     * - Manual reset is needed
     */
    void reset()
    {
        intervals.clear();
        lastTapTime.reset();
    }

private:
    std::vector<double> intervals; ///< Storage for inter-tap intervals
    std::optional<double> lastTapTime; ///< Timestamp of the last tap
};

/**
 * @class MetronomeAudioProcessor
 * @brief Main processor class for the BeatIt metronome plugin
 * 
 * The MetronomeAudioProcessor class handles all core functionality:
 * - Tempo control with tap tempo support (1-500 BPM)
 * - Time signature handling and beat processing
 * - Sound generation and playback management
 * - Beat muting capabilities
 * - State persistence and configuration management
 */
class MetronomeAudioProcessor : public juce::AudioProcessor,
                                public juce::AudioProcessorValueTreeState::Listener
{
public:
    /**
     * @enum ClickType
     * @brief Available types of metronome clicks
     */
    enum class ClickType {
        High, /**< High-pitched click (1500 Hz, 30ms) */
        Low, /**< Low-pitched click (800 Hz, 20ms) */
        Mute /**< Silent click (no sound output) */
    };

    /**
     * @enum Subdivision
     * @brief Defines available rhythmic subdivision patterns
     * 
     * Represents different ways a beat can be subdivided, matching common musical notation
     */
    enum class Subdivision {
        Quarter, /**< Single quarter note per beat */
        TwoEighths, /**< Two eighth notes per beat */
        ThreeEighths, /**< Triplet feel - three eighth notes per beat */
        FourSixteenths, /**< Four sixteenth notes per beat */
        EighthTwoSixteenths, /**< One eighth followed by two sixteenth notes */
        TwoSixteenthsEighth /**< Two sixteenth notes followed by one eighth note */
    };

    //==============================================================================
    /** @name Construction and Destruction */
    ///@{

    /**
     * @brief Initializes the processor with default settings
     */
    MetronomeAudioProcessor();

    /**
     * @brief Cleanup and resource release
     */
    ~MetronomeAudioProcessor() override;
    ///@}

    //==============================================================================
    /** @name Audio Processing */
    ///@{

    /**
     * @brief Prepares the processor for playback
     * @param sampleRate The target sample rate
     * @param samplesPerBlock Maximum expected block size
     */
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;

    /**
     * @brief Releases resources when playback stops
     */
    void releaseResources() override;

    /**
     * @brief Processes an incoming audio block
     * @param buffer Audio buffer to process
     * @param midiMessages MIDI messages to process (unused)
     */
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    ///@}

    //==============================================================================
    /** @name Editor Management */
    ///@{

    /**
     * @brief Creates the plugin's editor instance
     * @return Pointer to the new editor component
     */
    juce::AudioProcessorEditor* createEditor() override;

    /**
     * @brief Checks editor availability
     * @return true if the plugin has an editor component
     */
    bool hasEditor() const override;
    ///@}

    //==============================================================================
    /** @name Plugin Information */
    ///@{

    /**
     * @brief Gets the plugin name
     * @return Plugin name as a string
     */
    const juce::String getName() const override;

    /**
     * @brief Checks MIDI input capability
     * @return true if the plugin accepts MIDI input
     */
    bool acceptsMidi() const override;

    /**
     * @brief Checks MIDI output capability
     * @return true if the plugin produces MIDI output
     */
    bool producesMidi() const override;

    /**
     * @brief Checks if plugin is MIDI effect
     * @return true if plugin is MIDI effect only
     */
    bool isMidiEffect() const override;

    /**
     * @brief Gets plugin's tail length
     * @return Tail length in seconds
     */
    double getTailLengthSeconds() const override;

    /**
     * @brief Gets total number of programs
     * @return Number of available programs
     */
    int getNumPrograms() override;

    /**
     * @brief Gets current program index
     * @return Current program index
     */
    int getCurrentProgram() override;

    /**
     * @brief Sets current program
     * @param index Program index to set
     */
    void setCurrentProgram (int index) override;

    /**
     * @brief Gets program name
     * @param index Program index
     * @return Program name as string
     */
    const juce::String getProgramName (int index) override;

    /**
     * @brief Changes program name
     * @param index Program index
     * @param newName New name to set
     */
    void changeProgramName (int index, const juce::String& newName) override;
    ///@}

    //==============================================================================
    /** @name State Management */
    ///@{

    /**
     * @brief Gets current play state
     * @return true if metronome is playing
     */
    bool getPlayState() const;

    /**
     * @brief Toggles play/stop state
     */
    void togglePlayState();

    /**
     * @brief Gets current tempo rounded to integer
     * @return Current tempo in BPM
     */
    int getRoundedTempo() const { return static_cast<int> (std::round (bpmParameter->load())); }

    /**
     * @brief Gets beats per bar
     * @return Number of beats per bar
     */
    int getBeatsPerBar() const;

    /**
     * @brief Gets beat denominator
     * @return Beat denominator value
     */
    int getBeatDenominator() const;

    /**
     * @brief Gets current beat position
     * @return Current beat index
     */
    int getCurrentBeat() const { return currentBeat; }

    /**
     * @brief Saves plugin state
     * @param destData Memory block for state data
     */
    void getStateInformation (juce::MemoryBlock& destData) override;

    /**
     * @brief Restores plugin state
     * @param data Pointer to state data
     * @param sizeInBytes Size of state data
     */
    void setStateInformation (const void* data, int sizeInBytes) override;

    /**
     * @brief Updates timing calculations
     */
    void updateTimingInfo();

    /**
     * @brief Gets parameter state
     * @return Reference to parameter state
     */
    juce::AudioProcessorValueTreeState& getState() { return *state; }
    ///@}

    //==============================================================================
    /** @name Parameter Handling */
    ///@{

    /**
     * @brief Handles parameter changes
     * @param parameterID ID of changed parameter
     * @param newValue New parameter value
     */
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    ///@}

    //==============================================================================
    /** @name Beat Muting */
    ///@{

    /**
     * @brief Checks if beat is muted
     * @param beatIndex Beat to check
     * @return true if beat is muted
     */
    bool isBeatMuted (int beatIndex) const;

    /**
     * @brief Toggles beat mute state
     * @param beatIndex Beat to toggle
     */
    void toggleBeatMute (int beatIndex);

    /**
     * @brief Gets muted beats pattern
     * @return Reference to muted beats vector
     */
    const std::vector<bool>& getMutedBeats() const { return mutedBeats; }

    /**
     * @brief Sets new muted beats pattern
     * @param newMutedBeats New mute pattern to set
     */
    void setMutedBeats (const std::vector<bool>& newMutedBeats);

    /**
     * @brief Updates muted beats vector size
     */
    void updateMutedBeatsSize();
    ///@}

    //==============================================================================
    /** @name Subdivision */
    ///@{
    /**
     * @brief Gets the number of subdivisions for the current pattern
     * @return Number of clicks per beat in current subdivision
     */
    int getSubdivisionCount() const;

    /**
     * @brief Gets the timing positions for current subdivision pattern
     * @return Vector of normalized positions (0.0 to 1.0) for subdivisions within a beat
     */
    std::vector<float> getSubdivisionTimings() const;
    ///@}

    //==============================================================================
    /** @name Tap Tempo */
    /**
     * @brief Process a tap tempo event
     * 
     * Handles a new tap tempo event by:
     * 1. Recording the tap
     * 2. Calculating new BPM
     * 3. Updating the BPM parameter if valid
     */
    void processTapTempo();
    ///@}

private:
    //==============================================================================
    /** @name Initialization Methods */
    ///@{
    void initializeParameters();
    void initializeAudioState();
    void initializeSoundMaps();
    void initializeSounds();
    void initializeMutedBeats();
    ///@}

    /** @name Audio Processing Methods */
    ///@{
    void processSample (juce::AudioBuffer<float>& buffer, int sample, int totalNumOutputChannels);
    void generateClickSound (juce::AudioBuffer<float>& buffer, ClickType type);
    void generateClickWaveform (juce::AudioBuffer<float>& buffer, float frequency, double sampleRate, float durationMs);
    const juce::AudioBuffer<float>& getSoundBufferForClickType (ClickType type) const;
    ///@}

    //==============================================================================
    /** @name Parameter State */
    ///@{
    std::unique_ptr<juce::AudioProcessorValueTreeState> state;
    std::atomic<float>* bpmParameter = nullptr;
    std::atomic<float>* playParameter = nullptr;
    std::atomic<float>* beatsPerBarParameter = nullptr;
    std::atomic<float>* beatDenominatorParameter = nullptr;
    std::atomic<float>* firstBeatSoundParameter = nullptr;
    std::atomic<float>* otherBeatsSoundParameter = nullptr;
    ///@}

    //==============================================================================
    /** @name Audio Buffers */
    ///@{
    juce::AudioBuffer<float> highClickBuffer;
    juce::AudioBuffer<float> lowClickBuffer;
    juce::AudioBuffer<float> muteBuffer;
    std::map<juce::String, ClickType> soundTypeMap;
    ///@}

    //==============================================================================
    /** @name Playback State */
    ///@{
    int currentBeat = 0;
    int samplesPerBeat = 0;
    double currentSampleRate = 44100.0;
    /** @brief Position in the current beat */
    int soundPosition = 0;
    /** @brief Position in the current click sound */
    int clickPosition = 0;
    ///@}

    //==============================================================================
    /** @name Beat Management */
    ///@{
    std::vector<bool> mutedBeats;
    ///@}

    //==============================================================================
    /** @name Subdivision Management */
    ///@{
    /** @brief Parameter handling subdivision pattern selection */
    std::atomic<float>* subdivisionParameter = nullptr;

    /** @brief Cached timings for current subdivision pattern */
    std::vector<float> currentSubdivisionTimings;

    /**
     * @brief Updates internal timing calculations for subdivisions
     * 
     * Calculates precise timing positions for each click within a beat
     * based on the current subdivision pattern.
     */
    void updateSubdivisionTimings();
    ///@}

    //==============================================================================
    /** @name Tap Tempo */
    TapTempoCalculator tapTempoCalculator; ///< Calculator for tap tempo functionality
    ///@}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MetronomeAudioProcessor)
};