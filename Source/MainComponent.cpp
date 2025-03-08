/*
  ==============================================================================

    This file contains the implementation of the MainComponent class for a JUCE application,
    serving as the top-level component managing audio decks and music library.

  ==============================================================================
*/

#include "MainComponent.h"

// Constructor: Set up UI components and audio channels
MainComponent::MainComponent()
{
    addAndMakeVisible(deck1);
    addAndMakeVisible(deck2);
    addAndMakeVisible(musicLib);
    
    musicLib.setDecks(&deck1, &deck2); // Link music library to decks
    
    setSize(800, 600);
    setAudioChannels(0, 2); // Stereo output
    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

// Prepare audio processing for decks
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    deck1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    deck2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// Mix audio from both decks into output buffer
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    juce::AudioBuffer<float> tempBuffer(2, bufferToFill.numSamples);
    tempBuffer.clear();

    deck1.getNextAudioBlock(bufferToFill);
    for (int ch = 0; ch < 2; ++ch)
        tempBuffer.addFrom(ch, 0, *bufferToFill.buffer, ch, 0, bufferToFill.numSamples, deck1.getVolume());

    deck2.getNextAudioBlock(bufferToFill);
    for (int ch = 0; ch < 2; ++ch)
        tempBuffer.addFrom(ch, 0, *bufferToFill.buffer, ch, 0, bufferToFill.numSamples, deck2.getVolume());

    bufferToFill.buffer->copyFrom(0, 0, tempBuffer.getReadPointer(0), bufferToFill.numSamples);
    bufferToFill.buffer->copyFrom(1, 0, tempBuffer.getReadPointer(1), bufferToFill.numSamples);
}

// Free up audio resources for both decks
void MainComponent::releaseResources()
{
    deck1.releaseResources();
    deck2.releaseResources();
}

// Draw radial gradient background
void MainComponent::paint(juce::Graphics& g)
{
    juce::ColourGradient bgGradient(
        juce::Colours::darkgrey.darker(0.8f), getWidth() / 2.0f, getHeight() / 2.0f,
        juce::Colours::darkgrey.brighter(0.2f), 0, 0,
        true);
    bgGradient.addColour(0.5, juce::Colours::darkgrey);
    g.setGradientFill(bgGradient);
    g.fillAll();

    g.setColour(juce::Colours::grey.darker(0.5f).withAlpha(0.5f));
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
}

// Layout components: Decks on sides, music library in center
void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto contentArea = area;
    int totalWidth = contentArea.getWidth();
    
    int deckWidth = totalWidth * 0.3f;
    int libraryWidth = totalWidth * 0.4f;
    
    int libraryX = (totalWidth - libraryWidth) / 2 + contentArea.getX();
    
    deck1.setBounds(contentArea.getX(), contentArea.getY(), deckWidth, contentArea.getHeight());
    musicLib.setBounds(libraryX, contentArea.getY(), libraryWidth, contentArea.getHeight());
    deck2.setBounds(contentArea.getX() + totalWidth - deckWidth, contentArea.getY(), deckWidth, contentArea.getHeight());
}
