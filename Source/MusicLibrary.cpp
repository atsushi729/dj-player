#include "MusicLibrary.h"
#include "DeckGUI.h"

MusicLibrary::MusicLibrary()
{
    addAndMakeVisible(searchBox);
    addAndMakeVisible(trackList);
    addAndMakeVisible(leftArrowButton);
    addAndMakeVisible(addButton);
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(rightArrowButton);
    
    searchBox.addListener(this);
    trackList.setModel(this);
    
    searchBox.setTextToShowWhenEmpty("Search tracks...", juce::Colours::grey);
    leftArrowButton.onClick = [this] { leftArrowClicked(); };
    addButton.onClick = [this] { addButtonClicked(); };
    deleteButton.onClick = [this] { deleteButtonClicked(); };
    rightArrowButton.onClick = [this] { rightArrowClicked(); };
    
    libraryFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("dj_library.xml");
    loadLibrary();
}

MusicLibrary::~MusicLibrary()
{
    saveLibrary();
}

void MusicLibrary::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightgrey);
}

void MusicLibrary::resized()
{
    auto area = getLocalBounds().reduced(5);
    searchBox.setBounds(area.removeFromTop(30));
    
    // Button area at bottom
    auto buttonArea = area.removeFromBottom(30);
    leftArrowButton.setBounds(buttonArea.removeFromLeft(30).reduced(2));
    addButton.setBounds(buttonArea.removeFromLeft(120).reduced(2));
    deleteButton.setBounds(buttonArea.removeFromLeft(120).reduced(2));
    rightArrowButton.setBounds(buttonArea.removeFromRight(30).reduced(2));
    
    trackList.setBounds(area);
}

void MusicLibrary::textEditorTextChanged(juce::TextEditor&)
{
    trackList.updateContent();
}

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
                g.fillAll(rowIsSelected ? juce::Colours::lightblue : juce::Colours::white);
                g.setColour(juce::Colours::black);
                g.drawText(tracks[i].getFileName(), 5, 0, width - 10, height, juce::Justification::centredLeft);
                break;
            }
            visibleRow++;
        }
    }
}

void MusicLibrary::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    trackList.selectRow(row);
}

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
                return tracks[i];
            visibleRow++;
        }
    }
    return juce::File();
}

void MusicLibrary::addTrack(const juce::File& file)
{
    if (file.existsAsFile() && !tracks.contains(file))
    {
        tracks.add(file);
        trackList.updateContent();
    }
}

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
                        tracks.add(trackFile);
                }
            }
        }
    }
    trackList.updateContent();
}

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

void MusicLibrary::setDecks(DeckGUI* deck1, DeckGUI* deck2)
{
    deck1Ptr = deck1;
    deck2Ptr = deck2;
}

void MusicLibrary::leftArrowClicked()
{
    juce::File selectedTrack = getSelectedTrack();
    if (selectedTrack.exists() && deck1Ptr != nullptr)
    {
        deck1Ptr->loadFile(selectedTrack);
    }
}

void MusicLibrary::rightArrowClicked()
{
    juce::File selectedTrack = getSelectedTrack();
    if (selectedTrack.exists() && deck2Ptr != nullptr)
    {
        deck2Ptr->loadFile(selectedTrack);
    }
}

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
