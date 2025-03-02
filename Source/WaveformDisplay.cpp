#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager & formatManagerToUse,
                                 juce::AudioThumbnailCache & cacheToUse)
    : audioThumb(1000, formatManagerToUse, cacheToUse),
      fileLoaded(false)
{
    audioThumb.addChangeListener(this);
    startTimer(50);  // Update playhead every 50ms (20 times per second)
}

WaveformDisplay::~WaveformDisplay()
{
    stopTimer();  // Stop the timer when the component is destroyed
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // Clear the background

    g.setColour(juce::Colours::lightgreen);
    g.drawRect(getLocalBounds(), 1);   // Draw an outline around the component
    
    if (fileLoaded)
    {
        audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f);
        
        // Draw the playhead
        if (audioThumb.getTotalLength() > 0)
        {
            float playheadX = (playheadPosition / audioThumb.getTotalLength()) * getWidth();
            g.setColour(juce::Colours::red);  // Playhead color
            g.drawVerticalLine(static_cast<int>(playheadX), 0, static_cast<float>(getHeight()));
        }
    }
    else
    {
        g.setColour(juce::Colours::lightgreen);
        g.setFont(juce::FontOptions(20.0f));
        g.drawText("File not loaded ...", getLocalBounds(),
                   juce::Justification::centred, true);   // Draw placeholder text
    }
}

void WaveformDisplay::resized()
{
    // No child components to resize in this case
}

void WaveformDisplay::loadURL(juce::URL audioURL)
{
    DBG("WaveformDisplay::loadURL: Loading file");
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));
    
    if (fileLoaded)
    {
        DBG("WaveformDisplay::loadURL: Loaded successfully");
        playheadPosition = 0.0;  // Reset playhead position when loading a new file
        repaint();
    }
    else
    {
        DBG("WaveformDisplay::loadURL: Failed to load");
    }
}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    DBG("WaveformDisplay::changeListenerCallback: Audio thumbnail changed");
    repaint();
}

void WaveformDisplay::setPosition(double positionInSeconds)
{
    playheadPosition = positionInSeconds;
    repaint();  // Repaint to update the playhead position
}

void WaveformDisplay::timerCallback()
{
    // Optional: If you want to update the playhead automatically based on playback
    // This requires integration with the transport source, which we'll handle later
    repaint();
}
