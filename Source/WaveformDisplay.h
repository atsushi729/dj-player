#pragma once
#include <JuceHeader.h>

//==============================================================================
class WaveformDisplay  : public juce::Component,
                        public juce::ChangeListener,
                        public juce::Timer  // Add Timer for updating playhead position
{
public:
    WaveformDisplay(juce::AudioFormatManager & formatManagerToUse,
                    juce::AudioThumbnailCache & cacheToUse);
    ~WaveformDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    
    void loadURL(juce::URL audioURL);
    
    // New method to update playhead position
    void setPosition(double positionInSeconds);

private:
    juce::AudioThumbnail audioThumb;
    bool fileLoaded;
    double playheadPosition = 0.0;  // Position of the playhead in seconds
    
    void timerCallback() override;  // For periodic updates
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
