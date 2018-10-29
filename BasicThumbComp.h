//
//  BasicThumbComp.h
//  Emblemotionsv1-1 - App
//
//  Created by Chad Riston Denaux on 10/13/18.
//

#ifndef BasicThumbComp_h
#define BasicThumbComp_h
#include "../JuceLibraryCode/JuceHeader.h"

class BasicThumbComp : public Component,
                       private ChangeListener
{
public:
    BasicThumbComp (int sourceSamplesPerThumbnailSample,
                              AudioFormatManager& formatManager,
                              AudioThumbnailCache& cache)
    : thumbnail (sourceSamplesPerThumbnailSample, formatManager, cache)
    {
        thumbnail.addChangeListener (this);
    }
    
    void setFile (const File& file)
    {
        thumbnail.setSource (new FileInputSource (file));
    }
    
    void paint (Graphics& g) override
    {
        if (thumbnail.getNumChannels() == 0)
            paintIfNoFileLoaded (g);
        else
            paintIfFileLoaded (g);
    }
    
    void paintIfNoFileLoaded (Graphics& g)
    {
        g.fillAll (Colours::black);
        g.setColour (Colours::darkblue);
        g.drawFittedText ("No File Loaded", getLocalBounds().reduced(0, 10), Justification::centred, 2.0f);
    }
    
    void paintIfFileLoaded (Graphics& g)
    {
        g.fillAll (Colours::black);
        g.setColour (Colours::darkred);
        thumbnail.drawChannels (g, getLocalBounds().reduced(0, 10), 0.0, thumbnail.getTotalLength(), 0.2f);
    }
    
    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            thumbnailChanged();
    }
    
    
private:
    void thumbnailChanged()
    {
        repaint();
    }
    
    AudioThumbnail thumbnail;
    
   
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicThumbComp)
};

#endif /* BasicThumbComp_h */
