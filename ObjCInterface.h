//
//  ObjCInterface.h
//  Emblemotionsv1-1
//
//  Created by Chad Riston Denaux on 10/8/18.
//
//  This header will expose the methods that will be made available to the main JUCE
//  components.

#ifndef ObjCInterface_h
#define ObjCInterface_h

//method to show interface works for passing a variable to Obj-C from C++ and back.
int ShowTheNumber(int i);

//affDex Accessors
float affJoy();
float affAnger();
float affDisgust();
float affEngage();
float affAtt();
float affValence();



void connector();

#endif /* ObjCInterface_h */
