#include "DeckGUI.h"

DeckGUI::DeckGUI(int _id) : id(_id)
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(speedSlider);

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
}

void DeckGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    
    auto bounds = getLocalBounds().reduced(10);
    g.setColour(juce::Colours::red);
    g.drawRoundedRectangle(bounds.toFloat(), 10.0f, 2.0f);
    
    g.setColour(juce::Colours::yellowgreen);
    g.fillRect(bounds.removeFromTop(200));
}

void DeckGUI::resized()
{
    auto area = getLocalBounds().reduced(10);
    playButton.setBounds(area.removeFromTop(40).reduced(5));
    volumeSlider.setBounds(area.removeFromTop(40).reduced(5));
    speedSlider.setBounds(area.removeFromTop(40).reduced(5));
}

void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        playing ? transportSource.stop() : transportSource.start();
        playing = !playing;
        playButton.setButtonText(playing ? "Stop" : "Play");
    }
}

void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        volume = (float)slider->getValue();
    if (slider == &speedSlider)
        resampleSource.setResamplingRatio(slider->getValue());
    
}

void DeckGUI::loadFile(const juce::File& file)
{
    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(readerSource.get());
    }
}

void DeckGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DeckGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (playing)
        resampleSource.getNextAudioBlock(bufferToFill);
    else
        bufferToFill.clearActiveBufferRegion();
}

void DeckGUI::releaseResources()
{
    transportSource.releaseResources();
}
