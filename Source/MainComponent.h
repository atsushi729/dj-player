#pragma once
#include <JuceHeader.h>
#include "DeckGUI.h"
#include "MusicLibrary.h"

class MainComponent  : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    DeckGUI deck1{1};
    DeckGUI deck2{2};
    MusicLibrary musicLib;
    juce::TextButton loadButton{"Load Track"};
    juce::TextButton addButton{"Add Track"};
    juce::TextButton loadToDeck1Button{"Load to Deck 1"};
    juce::TextButton loadToDeck2Button{"Load to Deck 2"};

    juce::FileChooser fChooser{"Choose an audio file",
                              juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
                              "*.mp3;*.wav;*.aiff"};

    void loadButtonClicked();
    void addButtonClicked();
    void loadToDeck1Clicked();
    void loadToDeck2Clicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
