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
    // Gradient background
    juce::ColourGradient bgGradient(juce::Colours::darkgrey.darker(0.5f), 0, 0,
                                   juce::Colours::darkgrey.brighter(0.2f), getWidth(), getHeight(), false);
    g.setGradientFill(bgGradient);
    g.fillAll();

    auto bounds = getLocalBounds().reduced(10);
    auto turntableHeight = bounds.getHeight() / 2;
    auto turntableBounds = bounds.removeFromBottom(turntableHeight).withSizeKeepingCentre(200, 200);

    // Add shadow under turntable
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillEllipse(turntableBounds.toFloat().translated(5.0f, 5.0f)); // Shadow offset

    // Vinyl with gradient
    juce::ColourGradient vinylGradient(juce::Colours::black, turntableBounds.getCentreX(), turntableBounds.getCentreY(),
                                      juce::Colours::grey.darker(0.5f), turntableBounds.getRight(), turntableBounds.getBottom(), true);
    g.setGradientFill(vinylGradient);
    g.fillEllipse(turntableBounds.toFloat());

    // Rim with metallic effect
    g.setColour(juce::Colours::red.brighter(0.2f));
    g.drawEllipse(turntableBounds.toFloat(), 5.0f);

    // Center label with rotation animation when playing
    g.setColour(juce::Colours::white);
    auto center = turntableBounds.getCentre();
    g.saveState();
    if (playing) {
        float angle = static_cast<float>(juce::Time::getMillisecondCounterHiRes() * 0.001 * speed) * 360.0f;
        g.addTransform(juce::AffineTransform::rotation(juce::degreesToRadians(angle), center.x, center.y));
    }
    g.fillEllipse(center.x - 20, center.y - 20, 40, 40);
    g.restoreState();
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
        // Post a message to update the playhead on the GUI thread
        juce::MessageManager::callAsync([this]() { updatePlayhead(); });
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void DeckGUI::updatePlayhead()
{
    // This runs on the message thread, so it's safe to call setPosition and repaint
    waveformDisplay.setPosition(transportSource.getCurrentPosition());
}

void DeckGUI::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}
