#include <Muse/Muse.h>
#include "MuseObject.h"

@implementation MuseObject{
    int _number;
}

//initialize instance to activate methods
MuseObject *muse = [[MuseObject alloc] init];


//C++ method encapsulating OBj-C method
int ShowTheNumber(int i)
{
  return  [(id) muse showNum:i];
}

//corresponding Obj-C method
- (int) showNum:(int) i
{
    _number= i;
    return _number;
}

@end
