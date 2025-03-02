#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(deck1);
    addAndMakeVisible(deck2);
    addAndMakeVisible(musicLib);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(addButton);
    addAndMakeVisible(loadToDeck1Button);
    addAndMakeVisible(loadToDeck2Button);
    
    loadButton.onClick = [this] { loadButtonClicked(); };
    addButton.onClick = [this] { addButtonClicked(); };
    loadToDeck1Button.onClick = [this] { loadToDeck1Clicked(); };
    loadToDeck2Button.onClick = [this] { loadToDeck2Clicked(); };
    
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
    auto area = getLocalBounds();
    deck1.setBounds(area.removeFromLeft(getWidth() / 3));
    deck2.setBounds(area.removeFromLeft(getWidth() / 3));
    musicLib.setBounds(area);
    loadButton.setBounds(10, getHeight() - 50, 100, 40);
    addButton.setBounds(120, getHeight() - 50, 100, 40);
    loadToDeck1Button.setBounds(230, getHeight() - 50, 100, 40);
    loadToDeck2Button.setBounds(340, getHeight() - 50, 100, 40);
}

void MainComponent::loadButtonClicked()
{
    juce::File track = musicLib.getSelectedTrack();
    if (track.exists())
    {
        deck1.loadFile(track);
    }
}

void MainComponent::addButtonClicked()
{
    auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;
    fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
    {
        juce::File chosenFile = chooser.getResult();
        if (chosenFile != juce::File())
        {
            DBG("Adding file: " << chosenFile.getFullPathName());
            musicLib.addTrack(chosenFile);
        }
    });
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
