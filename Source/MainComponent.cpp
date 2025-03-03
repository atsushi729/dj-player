#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(deck1);
    addAndMakeVisible(deck2);
    addAndMakeVisible(musicLib);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(loadToDeck1Button);
    addAndMakeVisible(loadToDeck2Button);
    
    loadButton.onClick = [this] { loadButtonClicked(); };
    loadToDeck1Button.onClick = [this] { loadToDeck1Clicked(); };
    loadToDeck2Button.onClick = [this] { loadToDeck2Clicked(); };
    
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
    g.fillAll(juce::Colours::grey);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10); // Padding around edges
    
    // Top control bar for buttons
    auto buttonArea = area.removeFromTop(50);
    loadButton.setBounds(buttonArea.removeFromLeft(100).reduced(5));
    loadToDeck1Button.setBounds(buttonArea.removeFromLeft(100).reduced(5));
    loadToDeck2Button.setBounds(buttonArea.removeFromLeft(100).reduced(5));
    
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

void MainComponent::loadButtonClicked()
{
    juce::File track = musicLib.getSelectedTrack();
    if (track.exists())
    {
        deck1.loadFile(track);
    }
}

void MainComponent::loadToDeck1Clicked()
{
    auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;
    fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
    {
        juce::File chosenFile = chooser.getResult();
        if (chosenFile != juce::File())
        {
            DBG("Loading to Deck 1: " << chosenFile.getFullPathName());
            deck1.loadFile(chosenFile);
            musicLib.addTrack(chosenFile);
        }
    });
}

void MainComponent::loadToDeck2Clicked()
{
    auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;
    fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
    {
        juce::File chosenFile = chooser.getResult();
        if (chosenFile != juce::File())
        {
            DBG("Loading to Deck 2: " << chosenFile.getFullPathName());
            deck2.loadFile(chosenFile);
            musicLib.addTrack(chosenFile);
        }
    });
}
