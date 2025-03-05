#pragma once
#include <JuceHeader.h>

class WaveformDisplay : public juce::Component,
                        public juce::ChangeListener,
                        public juce::Timer
{
public:
    WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
                    juce::AudioThumbnailCache& cacheToUse);
    ~WaveformDisplay() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void loadURL(juce::URL audioURL);
    void setPosition(double positionInSeconds);

    void mouseMove(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    std::function<void(double)> onPositionClicked;

private:
    juce::AudioThumbnail audioThumb;
    bool fileLoaded{false};
    double playheadPosition{0.0};
    float hoverPosition{-1.0f};

    void timerCallback() override;

    void drawBackground(juce::Graphics& g);
    void drawWaveform(juce::Graphics& g);
    void drawPlayhead(juce::Graphics& g);
    void drawHoverIndicator(juce::Graphics& g);
    void drawPlaceholderText(juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
