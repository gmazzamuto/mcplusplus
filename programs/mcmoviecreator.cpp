#include "mcmoviecreator.h"

#include <QApplication>
#include <QString>

MCMovieCreator::MCMovieCreator(QString fileName, QWidget *parent) :
    GLWidget(parent)
{
    file.setXMLParserEnabled(true);
    file.openFile(fileName.toStdString().c_str());

    setDisplayedOriginPos(0,0,-600);
    setDisplayedAxisLength(500);
    memset(dataTimes,0,4*sizeof(MCfloat *));
    memset(dataPoints,0,4*sizeof(MCfloat *));
    points = NULL;
}

void MCMovieCreator::createMovie() {
    unsigned int wFlags = FLAG_TRANSMITTED | FLAG_BALLISTIC;
    const u_int64_t *photonCounters = file.photonCounters();
    //allocate memory and load data from file
    for (uint type = 0; type < 4; type++) {
        if(wFlags & walkerTypeToFlag(type)) {
            dataTimes[type] = (MCfloat*)malloc(sizeof(MCfloat)*photonCounters[type]);
            if(!file.loadData(DATA_TIMES,(walkerType)type, dataTimes[type])) {
                free(dataTimes[type]);
                dataTimes[type] = NULL;
            }
        }
    }

    for (uint type = 0; type < 4; type++) {
        if(wFlags & walkerTypeToFlag(type)) {
            dataPoints[type] = (MCfloat*)malloc(sizeof(MCfloat)*photonCounters[type]*2);
            if(!file.loadData(DATA_POINTS,(walkerType)type, dataPoints[type])) {
                free(dataPoints[type]);
                dataPoints[type] = NULL;
            }
        }
    }

    MCfloat binSize = 5e-2;

    MCfloat minVal=1./0., maxVal=-1./0.;

    //find min and max values
    for (uint type = 0; type < 4; ++type) {
        if(dataTimes[type] == NULL)
            continue;
        for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
            if(dataTimes[type][i] > maxVal)
                maxVal = dataTimes[type][i];
            if(dataTimes[type][i] < minVal)
                minVal = dataTimes[type][i];
        }
    }

    unsigned int nBins = ceil((maxVal - minVal)/binSize);
    if(nBins == 0)
        nBins = 1;
    vector<vector<MCfloat> > containers;
    for (uint i = 0; i < nBins; ++i) {
        containers.push_back(vector<MCfloat>());
    }

    size_t c = 0;
    MCfloat z = file.simulation()->sample()->totalThickness();
    for (uint type = 0; type < 4; ++type) {
        if(dataTimes[type] == NULL)
            continue;
        c = 0;
        for (u_int64_t i = 0; i < photonCounters[type]; ++i) {
            unsigned int index = (dataTimes[type][i]-minVal)/binSize;
            containers[index].push_back(dataPoints[type][c++]);
            containers[index].push_back(dataPoints[type][c++]);
            containers[index].push_back(z);
        }
    }

    for (uint type = 0; type < 4; ++type) {
        if(dataTimes[type] != NULL)
            free(dataTimes[type]);
        if(dataPoints[type] != NULL)
            free(dataPoints[type]);
    }

    for (uint i = 0; i < nBins; ++i) {
        currentText = QString("%1 ps").arg(binSize*(i+0.5));
        stringstream ss;
        ss << "grab-";
        ss << setw(5) << setfill('0') << i;
        ss << ".png";

        setPoints(containers[i].data(),containers[i].size());
        setGrabFileName(ss.str().c_str());
        updateGL();
        qApp->processEvents();
        grab();

        containers[i].clear();
    }

    containers.clear();
    setPoints(NULL,0);
}

void MCMovieCreator::paint_GL_impl()
{
    if(points != NULL) {
        glBegin(GL_POINTS);
        glColor3f( 1,1,1 );
        for (int i = 0; i < nPoints; i+=3) {
            glVertex3f(points[i],points[i+1],points[i+2]);
        }
        glEnd();
    }

    QFont font;
    font.setPointSize(18);
    glColor3f( 1,1,1 );
    renderText(120,150,10,currentText,font);
}

void MCMovieCreator::setPoints(const MCfloat *points, size_t n)
{
    this->points = points;
    nPoints = n;
}

