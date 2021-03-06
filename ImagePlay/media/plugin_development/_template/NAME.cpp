#include "%CLASSNAME%.h"

void %CLASSNAME%::init()
{
    // init
    _result     = NULL;

    // basic settings
    setClassName("%CLASSNAME%");
    setTitle("%TITLE%");
    setCategory(IPLProcess::%CATEGORY%);
    setOpenCVSupport(IPLOpenCVSupport::%OPENCV%);
    setDescription("%DESCRIPTION%");
    setKeywords("%KEYWORDS%");

    // inputs and outputs
    addInput("Image", IPL_IMAGE_COLOR);
    addOutput("Binary Image", IPL_IMAGE_BW);

    // properties
    addProcessPropertyDouble("threshold", "Threshold", "0.0 < threshold < 1.0", 0.5, IPL_WIDGET_SLIDER, 0.0, 1.0);
}

void %CLASSNAME%::destroy()
{
    delete _result;
}

bool %CLASSNAME%::processInputData(IPLImage *image, int, bool useOpenCV)
{
    // delete previous result
    delete _result;
    _result = NULL;

    int width = image->width();
    int height = image->height();
    if( image->type() == IPL_IMAGE_GRAYSCALE )
        _result = new IPLImage( IPL_IMAGE_BW, width, height );
    else
        _result = new IPLImage( image->type(), width, height );

    // get properties
    float threshold = getProcessPropertyDouble("threshold");

    int progress = 0;
    int maxProgress = image->height() * image->getNumberOfPlanes();
    int nrOfPlanes = image->getNumberOfPlanes();

#pragma omp parallel for
    for( int planeNr=0; planeNr < nrOfPlanes; planeNr++ )
    {
        IPLImagePlane* plane = image->plane( planeNr );
        IPLImagePlane* newplane = _result->plane( planeNr );
        for(int y=0; y<height; y++)
        {
            // progress
            notifyProgressEventHandler(100*progress++/maxProgress);
            for(int x=0; x<width; x++)
            {
                newplane->p(x,y) = (plane->p(x,y) < threshold) ? 0.0f : 1.0f;
            }
        }
    }
    return true;
}

IPLData *%CLASSNAME%::getResultData(int)
{
    return _result;
}
