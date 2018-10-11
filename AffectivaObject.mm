
#import "AffectivaObject.h"
#import <Affdex/Affdex.h>

@implementation AffObject{
    CGFloat _emJoy, _emAnger, _emEngage, _emDisgust, _emAtt, _emVal;
}

AffObject *affObj = [[AffObject alloc] init];

-(id)init{
    return [self initilizeDetectors];
}

   //initialize to connect with system camera
-(id)initilizeDetectors{
    self = [super init];
    self.detector = [[AFDXDetector alloc] initWithDelegate: self usingCamera:AFDX_CAMERA_FRONT maximumFaces:1];
    
    [self.detector setDetectAllEmotions:YES];
    [self.detector setDetectAllExpressions:YES];
    [self.detector start];
    
    self.detector.maxProcessRate = 8.0;
    
    return self;
}

-(void)detector:(AFDXDetector *)detector hasResults:(NSMutableDictionary *)faces forImage:(NSImage *)image atTime:(NSTimeInterval)time
{
    if (faces == nil)
    {
        self.imageView.image= image;
    }
    else
    {
        NSArray *a = [faces allValues];
        if([a count] > 0)
        {
            AFDXFace *face = [a objectAtIndex:0];
            
            _emJoy = face.emotions.joy;
            _emAnger = face.emotions.anger;
            _emEngage = face.emotions.engagement;
            _emDisgust = face.emotions.disgust;
            _emAtt = face.expressions.attention;
            _emVal = face.emotions.valence;
            
        }
        
    }
}


float affJoy()
{
    return [(id) affObj getJoy];
}

- (float ) getJoy
{
    return _emJoy;
}

float affAnger()
{
    return [(id) affObj getAnger];
}

- (float ) getAnger
{
    return _emAnger;
}

float affEngage()
{
    return [(id) affObj getEngage];
}

- (float ) getEngage
{
    return _emEngage;
}

float affDisgust()
{
    return [(id) affObj getDisgust];
}

- (float ) getDisgust
{
    return _emDisgust;
}

float affAtt()
{
    return [(id) affObj getAtt];
}

- (float ) getAtt
{
    return _emAtt;
}

float affValence()
{
    return [(id) affObj getValence];
}

- (float ) getValence
{
    return _emVal;
}
@end
