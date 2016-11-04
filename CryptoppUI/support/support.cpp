#include "support.h"


Support::Support()
{
    settings = NULL;
}

Support::~Support()
{
    if(settings != NULL)
        delete settings;
}




