#pragma once
#include <JuceHeader.h>
#include "WaveformDisplay.h"

class DeckGUI : public juce::Component,
                public juce::Button::Listener,
                public juce::Slider::Listener
{
public:
    DeckGUI(int _id,
            juce::AudioFormatManager & formatManagerToUse,
            juce::AudioThumbnailCache & cacheToUse);
    ~DeckGUI() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void loadFile(const juce::File& file);
    bool isPlaying() { return playing; }
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    float getVolume() const { return volume; }
    double getPosition() const { return transportSource.getCurrentPosition(); }

    // New method to update playhead on the message thread
    void updatePlayhead();

private:
    int id;
    bool playing = false;
    float volume = 1.0f;
    float speed = 1.0f;

    juce::TextButton playButton{"Play"};
    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampleSource{&transportSource, false, 2};
    WaveformDisplay waveformDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
