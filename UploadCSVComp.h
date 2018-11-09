//
//  UploadCSVComp.h
//  Emblemotionsv1-1 - App
//
//  Created by Chad Riston Denaux on 11/2/18.
//

#ifndef UploadCSVComp_h
#define UploadCSVComp_h
#include "../JuceLibraryCode/JuceHeader.h"

class UploadCSVComp : private Timer
{
public:
    UploadCSVComp();
    ~UploadCSVComp();
    
    //Set up data output stream
    void timerCallback() override;
    void accessUpdateValues(int i, String s);
    
    HashMap<int, String> totalValuesUpdater;
    String header = "N | FFT | Alpha | Beta | Delta | Gamma | Theta | Joy | Anger | Disgust | Engagement | Attention | Valence | Pitch";
    
};


#endif /* UploadCSVComp_h */
