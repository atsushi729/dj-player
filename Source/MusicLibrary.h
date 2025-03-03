#pragma once
#include <JuceHeader.h>

class DeckGUI;  // Forward declaration

class MusicLibrary : public juce::Component,
                     public juce::TextEditor::Listener,
                     public juce::ListBoxModel
{
public:
    MusicLibrary();
    ~MusicLibrary() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void textEditorTextChanged(juce::TextEditor&) override;

    // ListBoxModel methods
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;

    juce::File getSelectedTrack();
    void addTrack(const juce::File& file);
    
    // New methods to set deck pointers
    void setDecks(DeckGUI* deck1, DeckGUI* deck2);

private:
    juce::TextEditor searchBox;
    juce::ListBox trackList;
    juce::Array<juce::File> tracks;
    juce::File libraryFile;
    
    // New arrow buttons
    juce::TextButton leftArrowButton{"<"};  // Load to Deck 1
    juce::TextButton rightArrowButton{">"}; // Load to Deck 2
    
    // Pointers to decks
    DeckGUI* deck1Ptr{nullptr};
    DeckGUI* deck2Ptr{nullptr};
    
    void loadLibrary();
    void saveLibrary();
    
    // New button click handlers
    void leftArrowClicked();
    void rightArrowClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicLibrary)
};
