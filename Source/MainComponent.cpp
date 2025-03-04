#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(deck1);
    addAndMakeVisible(deck2);
    addAndMakeVisible(musicLib);
    
    // Add this line to set the deck pointers
    musicLib.setDecks(&deck1, &deck2);
    
    setSize(800, 600);
    setAudioChannels(0, 2);
    
    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    deck1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    deck2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

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

void MainComponent::releaseResources()
{
    deck1.releaseResources();
    deck2.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    juce::ColourGradient bgGradient(juce::Colours::grey.darker(0.5f), 0, 0,
                                   juce::Colours::grey.brighter(0.3f), getWidth(), getHeight(), false);
    g.setGradientFill(bgGradient);
    g.fillAll();
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10); // Padding around edges
    
    // Main content area: Deck1 (left), MusicLibrary (center), Deck2 (right)
    auto contentArea = area;
    int totalWidth = contentArea.getWidth();
    
    // Allocate 30% for each deck, 40% for music library
    int deckWidth = totalWidth * 0.3f;
    int libraryWidth = totalWidth * 0.4f;
    
    // Calculate starting x-coordinate for MusicLibrary to center it
    int libraryX = (totalWidth - libraryWidth) / 2 + contentArea.getX();
    
    deck1.setBounds(contentArea.getX(), contentArea.getY(), deckWidth, contentArea.getHeight());
    musicLib.setBounds(libraryX, contentArea.getY(), libraryWidth, contentArea.getHeight());
    deck2.setBounds(contentArea.getX() + totalWidth - deckWidth, contentArea.getY(), deckWidth, contentArea.getHeight());
}
