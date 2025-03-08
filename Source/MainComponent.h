/*
  ==============================================================================

    This file defines the MainComponent class for a JUCE application,
    acting as the main container for decks and music library components.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DeckGUI.h"
#include "MusicLibrary.h"

// MainComponent: Top-level component managing decks and library
class MainComponent  : public juce::AudioAppComponent
{
    
//==============================================================================
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
//==============================================================================
private:
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumCache{100};
    
    DeckGUI deck1{1, formatManager, thumCache};
    DeckGUI deck2{2, formatManager, thumCache};
    MusicLibrary musicLib;

    juce::FileChooser fChooser{"Choose an audio file",
                              juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
                              "*.mp3;*.wav;*.aiff"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
