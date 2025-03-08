/*
  ==============================================================================

    This file contains the implementation of the MusicLibrary class for a JUCE application,
    handling track management, search, and crossfader functionality.

  ==============================================================================
*/

#include "MusicLibrary.h"
#include "DeckGUI.h"

// Custom crossfader appearance
void MusicLibrary::CrossfaderLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
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

// Constructor: Initialize UI components
MusicLibrary::MusicLibrary()
{
    addAndMakeVisible(searchBox);
    addAndMakeVisible(trackList);
    addAndMakeVisible(leftArrowButton);
    addAndMakeVisible(addButton);
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(rightArrowButton);
    addAndMakeVisible(crossfaderSlider);
    addAndMakeVisible(crossfaderLabel);

    searchBox.addListener(this);
    trackList.setModel(this);
    crossfaderSlider.addListener(this);
    
    searchBox.setTextToShowWhenEmpty("Search tracks...", juce::Colours::grey);
    
    leftArrowButton.onClick = [this] { leftArrowClicked(); };
    addButton.onClick = [this] { addButtonClicked(); };
    deleteButton.onClick = [this] { deleteButtonClicked(); };
    rightArrowButton.onClick = [this] { rightArrowClicked(); };
    
    crossfaderSlider.setRange(0.0, 1.0);
    crossfaderSlider.setValue(0.5);
    crossfaderSlider.setLookAndFeel(&crossfaderLookAndFeel);
    crossfaderSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    crossfaderSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    crossfaderLabel.setText("Crossfader", juce::dontSendNotification);
    crossfaderLabel.setFont(juce::FontOptions(14.0f));
    crossfaderLabel.setJustificationType(juce::Justification::centred);

    libraryFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("dj_library.xml");
    loadLibrary();
}

MusicLibrary::~MusicLibrary()
{
    crossfaderSlider.setLookAndFeel(nullptr);
    saveLibrary(); // Persist library on shutdown
}

// Draw the music library UI with a gradient background and rounded borders
void MusicLibrary::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(
        juce::Colours::lightgrey.brighter(0.2f), 0, 0,
        juce::Colours::lightgrey.darker(0.1f), 0, getHeight(),
        false);
    gradient.addColour(0.7, juce::Colours::lightgrey);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 12.0f);

    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawRoundedRectangle(getLocalBounds().reduced(1).toFloat(), 12.0f, 3.0f);

    g.setColour(juce::Colours::grey.darker(0.7f).withAlpha(0.8f));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 12.0f, 1.5f);
}

// Arrange UI components (search box, buttons, crossfader, track list) within the library
void MusicLibrary::resized()
{
    auto area = getLocalBounds().reduced(5);
    searchBox.setBounds(area.removeFromTop(30));
    
    auto controlArea = area.removeFromBottom(80);
    auto buttonArea = controlArea.removeFromTop(30);
    leftArrowButton.setBounds(buttonArea.removeFromLeft(30).reduced(2));
    addButton.setBounds(buttonArea.removeFromLeft(120).reduced(2));
    deleteButton.setBounds(buttonArea.removeFromLeft(120).reduced(2));
    rightArrowButton.setBounds(buttonArea.removeFromRight(30).reduced(2));
    
    auto crossfaderArea = controlArea;
    crossfaderLabel.setBounds(crossfaderArea.removeFromTop(20).reduced(5));
    crossfaderSlider.setBounds(crossfaderArea.reduced(5, 2));
    
    trackList.setBounds(area);
}

// Update track list display when search text changes
void MusicLibrary::textEditorTextChanged(juce::TextEditor&)
{
    trackList.updateContent(); // Refresh list on search input
}

// Adjust deck volumes based on crossfader position
void MusicLibrary::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &crossfaderSlider && deck1Ptr != nullptr && deck2Ptr != nullptr)
    {
        float value = static_cast<float>(crossfaderSlider.getValue());
        deck1Ptr->getVolume() = 1.0f - value;
        deck2Ptr->getVolume() = value;
    }
}

// Count visible rows based on search filter
int MusicLibrary::getNumRows()
{
    auto searchText = searchBox.getText().toLowerCase();
    if (searchText.isEmpty())
        return tracks.size();
    
    int count = 0;
    for (const auto& track : tracks)
        if (track.getFileName().toLowerCase().contains(searchText))
            count++;
    return count;
}

// Draw a single track item in the list box, applying search filter and selection styling
void MusicLibrary::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    auto searchText = searchBox.getText().toLowerCase();
    int visibleRow = 0;
    
    for (int i = 0; i < tracks.size(); i++)
    {
        if (searchText.isEmpty() || tracks[i].getFileName().toLowerCase().contains(searchText))
        {
            if (visibleRow == rowNumber)
            {
                g.fillAll(rowIsSelected ? juce::Colours::lightblue : (rowNumber % 2 == 0 ? juce::Colours::white : juce::Colours::lightgrey.brighter(0.5f)));
                g.setColour(juce::Colours::black);
                g.setFont(juce::FontOptions(16.0f));
                g.drawText(tracks[i].getFileName(), 10, 0, width - 20, height, juce::Justification::centredLeft);
                break;
            }
            visibleRow++;
        }
    }
}

// Handle track selection when a list box item is clicked
void MusicLibrary::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    trackList.selectRow(row);
}

// Get file of selected track, accounting for search filter
juce::File MusicLibrary::getSelectedTrack()
{
    int row = trackList.getSelectedRow();
    if (row < 0) return juce::File();
    
    auto searchText = searchBox.getText().toLowerCase();
    int visibleRow = 0;
    
    for (int i = 0; i < tracks.size(); i++)
    {
        if (searchText.isEmpty() || tracks[i].getFileName().toLowerCase().contains(searchText))
        {
            if (visibleRow == row)
            {
                return tracks[i];
            }
            visibleRow++;
        }
    }
    return juce::File();
}

// Add a new track to the library if it exists and isn’t already present
void MusicLibrary::addTrack(const juce::File& file)
{
    if (file.existsAsFile() && !tracks.contains(file))
    {
        tracks.add(file);
        trackList.updateContent(); // Refresh the track list display
    }
}

// Load track list from XML file
void MusicLibrary::loadLibrary()
{
    tracks.clear();
    
    if (libraryFile.existsAsFile())
    {
        std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(libraryFile);
        if (xml && xml->hasTagName("MusicLibrary"))
        {
            for (auto* element : xml->getChildIterator())
            {
                if (element->hasTagName("Track"))
                {
                    juce::String path = element->getStringAttribute("path");
                    juce::File trackFile(path);
                    if (trackFile.existsAsFile())
                    {
                        tracks.add(trackFile);
                    }
                }
            }
        }
    }
    trackList.updateContent();
}

// Save track list to XML file
void MusicLibrary::saveLibrary()
{
    juce::XmlElement xml("MusicLibrary");
    
    for (const auto& track : tracks)
    {
        auto* trackElement = xml.createNewChildElement("Track");
        trackElement->setAttribute("path", track.getFullPathName());
    }
    
    if (!xml.writeTo(libraryFile))
    {
        DBG("Failed to save music library to " << libraryFile.getFullPathName());
    }
}

// Link the music library to two decks and initialize their volumes
void MusicLibrary::setDecks(DeckGUI* deck1, DeckGUI* deck2)
{
    deck1Ptr = deck1;
    deck2Ptr = deck2;
    if (deck1Ptr && deck2Ptr)
    {
        deck1Ptr->getVolume() = 0.5f;
        deck2Ptr->getVolume() = 0.5f; // Balance volumes initially
    }
}

// Load the selected track into Deck 1 when the left arrow is clicked
void MusicLibrary::leftArrowClicked()
{
    juce::File selectedTrack = getSelectedTrack();
    if (selectedTrack.exists() && deck1Ptr != nullptr)
    {
        deck1Ptr->loadFile(selectedTrack);
    }
}

// Load the selected track into Deck 2 when the right arrow is clicked
void MusicLibrary::rightArrowClicked()
{
    juce::File selectedTrack = getSelectedTrack();
    if (selectedTrack.exists() && deck2Ptr != nullptr)
    {
        deck2Ptr->loadFile(selectedTrack);
    }
}

// Open file chooser to add new track
void MusicLibrary::addButtonClicked()
{
    auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;
    fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
    {
        juce::File chosenFile = chooser.getResult();
        if (chosenFile != juce::File())
        {
            DBG("Adding file: " << chosenFile.getFullPathName());
            addTrack(chosenFile);
        }
    });
}

// Remove selected track from list
void MusicLibrary::deleteButtonClicked()
{
    int selectedRow = trackList.getSelectedRow();
    if (selectedRow >= 0)
    {
        auto searchText = searchBox.getText().toLowerCase();
        int visibleRow = 0;
        
        for (int i = 0; i < tracks.size(); i++)
        {
            if (searchText.isEmpty() || tracks[i].getFileName().toLowerCase().contains(searchText))
            {
                if (visibleRow == selectedRow)
                {
                    DBG("Deleting track: " << tracks[i].getFullPathName());
                    tracks.remove(i);
                    trackList.updateContent();
                    trackList.deselectAllRows();
                    break;
                }
                visibleRow++;
            }
        }
    }
}
