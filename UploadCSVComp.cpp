//
//  UploadCSVComp.cpp
//  Emblemotionsv1-1 - App
//
//  Created by Chad Riston Denaux on 11/2/18.
//
#include "UploadCSVComp.h"


UploadCSVComp::UploadCSVComp()
{
    startTimer(10000);
    
}

UploadCSVComp::~UploadCSVComp()
{
    
}

void UploadCSVComp::timerCallback()
{
    File f("~/packet.csv");
    if(f.exists()) f.deleteFile();
    FileOutputStream fs(f);
    fs.writeString(header + "\n");
    std::cout << header << std::endl;
    HashMap<int, String>::Iterator i (totalValuesUpdater);
    while (i.next())
    {
        String key = String(i.getKey());
        String values = String(i.getValue());
        fs.writeString(key + " | " + values + "\n");
        std::cout << key + " | " + values << std::endl;
    }
    
    
}

void UploadCSVComp::accessUpdateValues(int i, String s)
{
    totalValuesUpdater.set(i, s);
}
