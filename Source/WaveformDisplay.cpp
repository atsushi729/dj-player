#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager & formatManagerToUse,
                                 juce::AudioThumbnailCache & cacheToUse) : audioThumb(1000, formatManagerToUse,cacheToUse),fileLoaded(false)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::lightgreen);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
    if (fileLoaded)
    {
        audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f);
    }
    else
    {
        g.setColour (juce::Colours::lightgreen);
        g.setFont (juce::FontOptions (20.0f));
        g.drawText ("File not loaded ...", getLocalBounds(),
                    juce::Justification::centred, true);   // draw some placeholder text
    }
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void WaveformDisplay::loadURL(juce::URL audioURL)
{
    DBG("DeckGUI::loadFile: Successfully loaded file for Deck !!!!!");
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));
    
    if (fileLoaded)
    {
        DBG("loaded");
        repaint();
    }
    else{
        DBG("Not loaded");
    }
}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    DBG("DeckGUI::loadFile: Successfully loaded file for Deck !!!!!");
    repaint();
}
