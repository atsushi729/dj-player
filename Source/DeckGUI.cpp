/*
  ==============================================================================

    This file contains the implementation of the DeckGUI class for a JUCE application,
    managing audio playback, UI controls, and turntable visualization for a deck.

  ==============================================================================
*/

#include "DeckGUI.h"

// Custom slider appearance
void DeckGUI::SliderLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPos, float minSliderPos, float maxSliderPos,
                                                const juce::Slider::SliderStyle, juce::Slider& slider)
{
    auto trackBounds = juce::Rectangle<float>(x, y + height * 0.4f, width, height * 0.2f);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(trackBounds, 2.0f);

    g.setColour(juce::Colours::grey.darker(0.5f));
    for (int i = 0; i <= 10; ++i)
    {
        float tickX = x + (width * i / 10.0f);
        g.drawVerticalLine(static_cast<int>(tickX), trackBounds.getY() - 5, trackBounds.getBottom() + 5);
    }

    auto thumbWidth = 12.0f;
    auto thumbHeight = 20.0f;
    auto thumbBounds = juce::Rectangle<float>(sliderPos - thumbWidth / 2, y + (height - thumbHeight) / 2, thumbWidth, thumbHeight);
    g.setColour(juce::Colours::orange);
    g.fillRect(thumbBounds);
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.drawRect(thumbBounds, 1.0f);
}

// Constructor: Set up UI and audio components
DeckGUI::DeckGUI(int _id,
                 juce::AudioFormatManager& formatManagerToUse,
                 juce::AudioThumbnailCache& cacheToUse)
    : id(_id), waveformDisplay(formatManagerToUse, cacheToUse)
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(volumeLabel);
    addAndMakeVisible(speedLabel);

    playButton.addListener(this);
    volumeSlider.addListener(this);
    speedSlider.addListener(this);

    volumeSlider.setRange(0.0, 1.0);
    speedSlider.setRange(0.5, 1.5);
    volumeSlider.setValue(1.0);
    speedSlider.setValue(1.0);

    volumeSlider.setLookAndFeel(&sliderLookAndFeel);
    speedSlider.setLookAndFeel(&sliderLookAndFeel);

    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    volumeLabel.setText("Volume", juce::dontSendNotification);
    speedLabel.setText("Speed", juce::dontSendNotification);

    volumeLabel.setFont(juce::FontOptions(14.0f));
    speedLabel.setFont(juce::FontOptions(14.0f));

    volumeLabel.setJustificationType(juce::Justification::centred);
    speedLabel.setJustificationType(juce::Justification::centred);

    formatManager.registerBasicFormats();

    waveformDisplay.onPositionClicked = [this](double position) {
        setTransportPosition(position); // Link waveform click to transport
    };

    startTimer(16); // 60 FPS for turntable animation
}

DeckGUI::~DeckGUI()
{
    volumeSlider.setLookAndFeel(nullptr);
    speedSlider.setLookAndFeel(nullptr);
    stopTimer();
    transportSource.stop();
    transportSource.releaseResources();
    transportSource.setSource(nullptr);
    readerSource.reset();
}

// Draw deck UI with animated turntable
void DeckGUI::paint(juce::Graphics& g)
{
    juce::ColourGradient bgGradient(
        juce::Colours::black.brighter(0.1f), 0, 0,
        juce::Colours::darkgrey.darker(0.3f), 0, getHeight(),
        false);
    bgGradient.addColour(0.3, juce::Colours::grey.darker(0.2f));
    g.setGradientFill(bgGradient);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.0f);

    g.setColour(juce::Colours::grey.withAlpha(0.05f));
    for (int i = 0; i < 100; ++i)
    {
        float x = juce::Random::getSystemRandom().nextFloat() * getWidth();
        float y = juce::Random::getSystemRandom().nextFloat() * getHeight();
        g.fillRect(x, y, 1.0f, 1.0f);
    }

    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.drawRoundedRectangle(getLocalBounds().reduced(2).toFloat(), 8.0f, 2.0f);

    auto bounds = getLocalBounds().reduced(10);
    auto turntableHeight = bounds.getHeight() / 2;
    auto turntableBounds = bounds.removeFromBottom(turntableHeight).withSizeKeepingCentre(200, 200);

    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillEllipse(turntableBounds.toFloat().translated(5.0f, 5.0f));

    juce::ColourGradient vinylGradient(juce::Colours::black, turntableBounds.getCentreX(), turntableBounds.getCentreY(),
                                      juce::Colours::grey.darker(0.5f), turntableBounds.getRight(), turntableBounds.getBottom(), true);
    g.setGradientFill(vinylGradient);
    g.fillEllipse(turntableBounds.toFloat());

    g.setColour(juce::Colours::red.brighter(0.2f));
    g.drawEllipse(turntableBounds.toFloat(), 5.0f);

    auto center = turntableBounds.getCentre();
    g.saveState();
    if (transportSource.getTotalLength() > 0)
    {
        g.addTransform(juce::AffineTransform::rotation(currentAngle, center.x, center.y));
    }

    g.setColour(juce::Colours::red);
    g.drawLine(center.x, center.y, center.x + 80.0f, center.y, 2.0f);

    g.restoreState();
}

// Arrange deck UI components (play button, volume/speed controls, waveform) vertically
void DeckGUI::resized()
{
    auto area = getLocalBounds().reduced(10);
    
    auto playArea = area.removeFromTop(50);
    playButton.setBounds(playArea.reduced(5));

    auto volumeArea = area.removeFromTop(60);
    volumeLabel.setBounds(volumeArea.removeFromTop(20).reduced(5));
    volumeSlider.setBounds(volumeArea.reduced(5));

    auto speedArea = area.removeFromTop(60);
    speedLabel.setBounds(speedArea.removeFromTop(20).reduced(5));
    speedSlider.setBounds(speedArea.reduced(5));

    waveformDisplay.setBounds(area.removeFromTop(80).reduced(5));
}

// Toggle play/stop state
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

// Update volume or speed based on slider
void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        volume = static_cast<float>(slider->getValue());
    }
    else if (slider == &speedSlider)
    {
        speed = static_cast<float>(slider->getValue());
        resampleSource.setResamplingRatio(speed); // Adjust playback speed
    }
}

// Load audio file into transport and waveform
void DeckGUI::loadFile(const juce::File& file)
{
    if (!file.existsAsFile())
    {
        return;
    }

    if (playing)
    {
        transportSource.stop();
        playing = false;
        playButton.setButtonText("Play");
    }

    transportSource.setSource(nullptr);
    readerSource.reset();
    currentAngle = 0.0f;

    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(readerSource.get());
        juce::URL fileURL(file);
        waveformDisplay.loadURL(fileURL);
    }
}

void DeckGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// Fill the audio buffer with the next block of samples from the resampler if playing
void DeckGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (playing && readerSource != nullptr)
    {
        resampleSource.getNextAudioBlock(bufferToFill);
        juce::MessageManager::callAsync([this]() { updatePlayhead(); }); // Async update of playhead
    }
    else
    {
        bufferToFill.clearActiveBufferRegion(); // Clear buffer if not playing
    }
}

// Sync the waveform display’s playhead with the current transport position
void DeckGUI::updatePlayhead()
{
    waveformDisplay.setPosition(transportSource.getCurrentPosition());
}

// Release audio resources held by the transport and resampler
void DeckGUI::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

// Set the playback position in the transport and update the waveform display
void DeckGUI::setTransportPosition(double positionInSeconds)
{
    if (readerSource != nullptr)
    {
        transportSource.setPosition(positionInSeconds);
        waveformDisplay.setPosition(positionInSeconds); // Keep waveform in sync
    }
}

// Animate turntable rotation
void DeckGUI::timerCallback()
{
    if (playing && transportSource.getTotalLength() > 0)
    {
        float rotationSpeed = 0.5f * juce::MathConstants<float>::pi;
        currentAngle += rotationSpeed * speed * (16.0f / 1000.0f);
        repaint();
    }
}
