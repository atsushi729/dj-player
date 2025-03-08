/*
  ==============================================================================

    This file defines the DeckGUI class for a JUCE application,
    handling audio playback controls and UI for a single deck.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WaveformDisplay.h"

// DeckGUI: Controls audio playback and UI for a single deck
class DeckGUI : public juce::Component,
                public juce::Button::Listener,
                public juce::Slider::Listener,
                public juce::Timer
{
//==============================================================================
public:
    DeckGUI(int _id,
            juce::AudioFormatManager& formatManagerToUse,
            juce::AudioThumbnailCache& cacheToUse);
    ~DeckGUI() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void loadFile(const juce::File& file); // Load audio file into deck
    bool isPlaying() { return playing; }
    
    // Audio processing methods
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    float getVolume() const { return volume; }
    float& getVolume() { return volume; }
    double getPosition() const { return transportSource.getCurrentPosition(); }

    void updatePlayhead(); // Sync waveform playhead with transport
    void setTransportPosition(double positionInSeconds); // Set playback position

//==============================================================================
private:
    int id;
    bool playing = false;
    float volume = 1.0f;
    float speed = 1.0f;
    float currentAngle = 0.0f;

    juce::TextButton playButton{"Play"};
    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Label volumeLabel;
    juce::Label speedLabel;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampleSource{&transportSource, false, 2};
    WaveformDisplay waveformDisplay;

    class SliderLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
                            float sliderPos, float minSliderPos, float maxSliderPos,
                            const juce::Slider::SliderStyle, juce::Slider&) override;
    };
    
    SliderLookAndFeel sliderLookAndFeel;

    void timerCallback() override; // Update turntable animation

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
