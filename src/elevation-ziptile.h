#ifndef ELEVATION_ZIPTILE_H
#define ELEVATION_ZIPTILE_H
#include <stdio.h>
#include <math.h>
#include "tiles.h"
int getTile(char *filename, size_t len, float lat, float lng);
int getTileSRTM(char *filename, size_t len, float lat, float lng);
int getTileZip(char *filename, size_t len, float lat, float lng);

int getTileIndex(float lat, float lng);
int getElevationLineTiles(float *heights, int width, int nthLine, float startLat, float startLng, float vscale, float rot, int waterDrop, int baseHeight, int stepSize, TileArray *ta);
// vector<float> getElevations(float _lat,float _lng,int width,int height,float vscale,float rot,int,int,int);
#endif // ELEVATION_ZIPTILE_H