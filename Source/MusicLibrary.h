#pragma once
#include <JuceHeader.h>

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
    void addTrack(const juce::File& file);  // New method to add tracks

private:
    juce::TextEditor searchBox;
    juce::ListBox trackList;
    juce::Array<juce::File> tracks;
    juce::File libraryFile;
    
    void loadLibrary();
    void saveLibrary();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicLibrary)
};
