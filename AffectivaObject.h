//
//  AffectivaObject.h
//  Emblemotionsv1-1
//
//  Created by Chad Riston Denaux on 10/9/18.
//

#ifndef AffectivaObject_h
#define AffectivaObject_h
#import <AVFoundation/AVCaptureSession.h>
#import "ObjCInterface.h"
#import <Affdex/Affdex.h>
#import <Cocoa/Cocoa.h>


@interface AffObject : AVCaptureSession <AFDXDetectorDelegate>

//initialize this object
@property (strong) AFDXDetector *detector;
@property (strong) IBOutlet NSImageView *imageView;

-(float) getJoy;
-(float) getAnger;
-(float) getDisgust;
-(float) getEngage;
-(float) getAtt;
-(float) getValence;


@end


#endif /* AffectivaObject_h */
