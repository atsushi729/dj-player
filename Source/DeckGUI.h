#pragma once
#include <JuceHeader.h>
#include "WaveformDisplay.h"

class DeckGUI : public juce::Component,
                public juce::Button::Listener,
                public juce::Slider::Listener,
                public juce::Timer
{
public:
    DeckGUI(int _id,
            juce::AudioFormatManager& formatManagerToUse,
            juce::AudioThumbnailCache& cacheToUse);
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
    float& getVolume() { return volume; }
    double getPosition() const { return transportSource.getCurrentPosition(); }

    void updatePlayhead();
    void setTransportPosition(double positionInSeconds);

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

    // Custom LookAndFeel for sliders
    class SliderLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
                            float sliderPos, float minSliderPos, float maxSliderPos,
                            const juce::Slider::SliderStyle, juce::Slider&) override;
    };
    
    SliderLookAndFeel sliderLookAndFeel;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
