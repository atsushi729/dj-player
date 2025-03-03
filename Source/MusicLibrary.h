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
    
    void setDecks(DeckGUI* deck1, DeckGUI* deck2);

private:
    juce::TextEditor searchBox;
    juce::ListBox trackList;
    juce::Array<juce::File> tracks;
    juce::File libraryFile;
    
    // Buttons
    juce::TextButton leftArrowButton{"<"};
    juce::TextButton addButton{"Add Track"};
    juce::TextButton deleteButton{"Delete"};
    juce::TextButton rightArrowButton{">"};
    
    // Deck pointers
    DeckGUI* deck1Ptr{nullptr};
    DeckGUI* deck2Ptr{nullptr};
    
    juce::FileChooser fChooser{"Choose an audio file",
                              juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
                              "*.mp3;*.wav;*.aiff"};
    
    void loadLibrary();
    void saveLibrary();
    
    void leftArrowClicked();
    void rightArrowClicked();
    void addButtonClicked();
    void deleteButtonClicked();  // New handler

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicLibrary)
};
