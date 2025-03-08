/*
  ==============================================================================

    This file defines the MusicLibrary class for a JUCE application,
    managing a track list, search functionality, and crossfader controls.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DeckGUI;  // Forward declaration

// MusicLibrary: Manages track list and crossfader
class MusicLibrary : public juce::Component,
                     public juce::TextEditor::Listener,
                     public juce::ListBoxModel,
                     public juce::Slider::Listener
{
//==============================================================================
public:
    MusicLibrary();
    ~MusicLibrary() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void textEditorTextChanged(juce::TextEditor&) override;
    void sliderValueChanged(juce::Slider* slider) override;

    // ListBoxModel: Manage track list display
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;

    juce::File getSelectedTrack();
    void addTrack(const juce::File& file);
    
    void setDecks(DeckGUI* deck1, DeckGUI* deck2); // Link to decks for loading tracks
    
//==============================================================================
private:
    juce::TextEditor searchBox;
    juce::ListBox trackList;
    juce::Array<juce::File> tracks;
    juce::File libraryFile;
    
    juce::TextButton leftArrowButton{"<"};
    juce::TextButton addButton{"Add Track"};
    juce::TextButton deleteButton{"Delete"};
    juce::TextButton rightArrowButton{">"};
    
    juce::Slider crossfaderSlider;
    juce::Label crossfaderLabel;
    
    DeckGUI* deck1Ptr{nullptr};
    DeckGUI* deck2Ptr{nullptr};
    
    juce::FileChooser fChooser{"Choose an audio file",
                              juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
                              "*.mp3;*.wav;*.aiff"};
    
    class CrossfaderLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
                            float sliderPos, float minSliderPos, float maxSliderPos,
                            const juce::Slider::SliderStyle, juce::Slider&) override;
    };
    
    CrossfaderLookAndFeel crossfaderLookAndFeel;

    void loadLibrary(); // Load tracks from XML file
    void saveLibrary(); // Save tracks to XML file
    
    void leftArrowClicked();  // Load track to Deck 1
    void rightArrowClicked(); // Load track to Deck 2
    void addButtonClicked();  // Open file chooser to add track
    void deleteButtonClicked(); // Remove selected track

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicLibrary)
};
