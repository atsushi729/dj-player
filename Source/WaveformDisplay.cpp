#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
                                 juce::AudioThumbnailCache& cacheToUse)
    : audioThumb(1000, formatManagerToUse, cacheToUse)
{
    audioThumb.addChangeListener(this);
    startTimer(50);
}

WaveformDisplay::~WaveformDisplay()
{
    stopTimer();
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    drawBackground(g);
    
    if (fileLoaded)
    {
        drawWaveform(g);
        drawPlayhead(g);
        drawHoverIndicator(g);
    }
    else
    {
        drawPlaceholderText(g);
    }
}

void WaveformDisplay::resized()
{
}

void WaveformDisplay::loadURL(juce::URL audioURL)
{
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));
    
    if (fileLoaded)
    {
        playheadPosition = 0.0;
        repaint();
    }
}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    repaint();
}

void WaveformDisplay::setPosition(double positionInSeconds)
{
    playheadPosition = juce::jlimit(0.0, audioThumb.getTotalLength(), positionInSeconds);
    repaint();
}

void WaveformDisplay::timerCallback()
{
    repaint();
}

void WaveformDisplay::mouseMove(const juce::MouseEvent& event)
{
    if (fileLoaded && audioThumb.getTotalLength() > 0)
    {
        hoverPosition = static_cast<float>(event.x) / getWidth() * audioThumb.getTotalLength();
        repaint();
    }
}

void WaveformDisplay::mouseDown(const juce::MouseEvent& event)
{
    if (fileLoaded && audioThumb.getTotalLength() > 0 && onPositionClicked)
    {
        double clickedPosition = static_cast<double>(event.x) / getWidth() * audioThumb.getTotalLength();
        clickedPosition = juce::jlimit(0.0, audioThumb.getTotalLength(), clickedPosition);
        onPositionClicked(clickedPosition);
    }
}

void WaveformDisplay::mouseExit(const juce::MouseEvent& event)
{
    hoverPosition = -1.0f;
    repaint();
}

void WaveformDisplay::drawBackground(juce::Graphics& g)
{
    juce::ColourGradient bgGradient(
        juce::Colours::black.brighter(0.1f), 0, 0,
        juce::Colours::darkgrey.darker(0.2f), 0, getHeight(),
        false);
    bgGradient.addColour(0.5, juce::Colours::grey.darker(0.1f));
    g.setGradientFill(bgGradient);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.0f);

    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.drawRoundedRectangle(getLocalBounds().reduced(2).toFloat(), 8.0f, 2.0f);
}

void WaveformDisplay::drawWaveform(juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(5);
    
    juce::ColourGradient waveGradient(
        juce::Colours::cyan.darker(0.2f), 0, bounds.getCentreY(),
        juce::Colours::lightgreen.brighter(0.3f), 0, bounds.getBottom(),
        false);
    g.setGradientFill(waveGradient);
    
    audioThumb.drawChannel(g, bounds, 0, audioThumb.getTotalLength(), 0, 1.0f);
}

void WaveformDisplay::drawPlayhead(juce::Graphics& g)
{
    if (audioThumb.getTotalLength() <= 0) return;

    float playheadX = (playheadPosition / audioThumb.getTotalLength()) * getWidth();
    
    g.setColour(juce::Colours::red.withAlpha(0.5f));
    g.drawVerticalLine(static_cast<int>(playheadX), 0, static_cast<float>(getHeight()));
    
    juce::Path playheadMarker;
    playheadMarker.addTriangle(playheadX - 5, 0, playheadX + 5, 0, playheadX, 10);
    g.setColour(juce::Colours::red.brighter(0.5f));
    g.fillPath(playheadMarker);
    
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.drawVerticalLine(static_cast<int>(playheadX) + 1, 0, static_cast<float>(getHeight()));
}

void WaveformDisplay::drawHoverIndicator(juce::Graphics& g)
{
    if (hoverPosition < 0 || audioThumb.getTotalLength() <= 0) return;

    float hoverX = (hoverPosition / audioThumb.getTotalLength()) * getWidth();
    
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawVerticalLine(static_cast<int>(hoverX), 0, static_cast<float>(getHeight()));
    
    juce::String timeText = juce::String::formatted("%.1f s", hoverPosition);
    g.setFont(juce::FontOptions(14.0f));
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.drawText(timeText, hoverX + 5, 5, 50, 20, juce::Justification::left);
}

void WaveformDisplay::drawPlaceholderText(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgreen.withAlpha(0.7f));
    g.setFont(juce::FontOptions(20.0f, juce::Font::italic));
    
    float alpha = 0.7f + 0.3f * std::sin(juce::Time::getMillisecondCounterHiRes() * 0.002f);
    g.setColour(juce::Colours::lightgreen.withAlpha(alpha));
    g.drawText("File not loaded ...", getLocalBounds(), juce::Justification::centred, true);
}
