#include "DeckGUI.h"

DeckGUI::DeckGUI(int _id,
                 juce::AudioFormatManager & formatManagerToUse,
                 juce::AudioThumbnailCache & cacheToUse): id(_id), waveformDisplay(formatManagerToUse, cacheToUse)
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(waveformDisplay);

    playButton.addListener(this);
    volumeSlider.addListener(this);
    speedSlider.addListener(this);

    volumeSlider.setRange(0.0, 1.0);
    speedSlider.setRange(0.5, 1.5);
    volumeSlider.setValue(1.0);
    speedSlider.setValue(1.0);

    formatManager.registerBasicFormats();
}

DeckGUI::~DeckGUI()
{
    transportSource.stop();
    transportSource.releaseResources();
    transportSource.setSource(nullptr);
    readerSource.reset();
}

void DeckGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    auto bounds = getLocalBounds().reduced(10);

    // Draw turntable
    auto turntableBounds = bounds.removeFromTop(bounds.getHeight() - 100).withSizeKeepingCentre(200, 200);
    g.setColour(juce::Colours::black);  // Vinyl color
    g.fillEllipse(turntableBounds.toFloat());
    g.setColour(juce::Colours::red);  // Rim color
    g.drawEllipse(turntableBounds.toFloat(), 5.0f);
    g.setColour(juce::Colours::white);  // Center label
    auto center = turntableBounds.getCentre();
    g.fillEllipse(center.getX() - 20, center.getY() - 20, 40, 40);
}

void DeckGUI::resized()
{
    auto area = getLocalBounds().reduced(10);
    playButton.setBounds(area.removeFromTop(40).reduced(5));
    volumeSlider.setBounds(area.removeFromTop(40).reduced(5));
    speedSlider.setBounds(area.removeFromTop(40).reduced(5));
    waveformDisplay.setBounds(area.removeFromTop(80).reduced(5));
}

void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        if (playing)
        {
            transportSource.stop();
        }
        else if (readerSource != nullptr)
        {
            transportSource.start();
        }
        playing = !playing;
        playButton.setButtonText(playing ? "Stop" : "Play");
    }
}

void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        volume = static_cast<float>(slider->getValue());
    }
    else if (slider == &speedSlider)
    {
        speed = static_cast<float>(slider->getValue());
        resampleSource.setResamplingRatio(speed);
    }
}

void DeckGUI::loadFile(const juce::File& file)
{
    if (!file.existsAsFile())
    {
        DBG("DeckGUI::loadFile: File does not exist for Deck " << id << ": " << file.getFullPathName());
        return;
    }

    DBG("DeckGUI::loadFile: Loading file for Deck " << id << ": " << file.getFullPathName());

    // Stop playback and release resources before loading a new file
    if (playing)
    {
        transportSource.stop();
        playing = false;
        playButton.setButtonText("Play");
    }

    // Clear the current source and release resources
    transportSource.setSource(nullptr);
    readerSource.reset();

    // Create a new reader and source
    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(readerSource.get());
        DBG("DeckGUI::loadFile: Successfully loaded file for Deck " << id);
        juce::URL fileURL(file);
        waveformDisplay.loadURL(fileURL);
    }
    else
    {
        DBG("DeckGUI::loadFile: Failed to create reader for file for Deck " << id);
    }
}

void DeckGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DeckGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (playing && readerSource != nullptr)
    {
        resampleSource.getNextAudioBlock(bufferToFill);
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void DeckGUI::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}
