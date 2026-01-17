// elevation-zip.c
// Gets elevations at points and in regions, from a zip archive

#include <zip.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "readzip.h"
#include "tiles.h"

int getTile(char *filename, size_t len, float lat, float lng)
{
	char ns = lat >= 0 ? 'N' : 'S'; // Positive is north
	char ew = lng >= 0 ? 'E' : 'W'; // Positive is east

	int tileLat = abs((int)floor(lat));
	int tileLng = abs((int)floor(lng));

	return snprintf(filename, len, "%c%02d%c%03d.hgt", ns, tileLat, ew, tileLng);
}

int getTileZip(char *filename, size_t len, float lat, float lng)
{
	char ns = lat >= 0 ? 'N' : 'S'; // Positive is north
	char ew = lng >= 0 ? 'E' : 'W'; // Positive is east

	int tileLat = abs((int)floor(lat));
	int tileLng = abs((int)floor(lng));

	return snprintf(filename, len, "hgt_files/%c%02d%c%03d.SRTMGL3.hgt.zip", ns, tileLat, ew, tileLng);
}

// returns the 'index' of the tile that contains a certain coordinate
// quicker than getTile()
int getTileIndex(float lat, float lng)
{
	return (int)floor(lat) * 360 + (int)floor(lng);
}

int tileNumber = 0;
char tileName[100];
char tileNameZip[100];
FILE *elfile;
char *tileData = NULL;


// width and heigth are in 'pixels'
// heights must be able to accomodate width-many values
int getElevationLineTiles(float *heights, int width, int nthLine, float startLat, float startLng, float vscale, float rot, int waterDrop, int baseHeight, int stepSize, TileArray *ta)
{
	int h;
	char number[2];

	int t = getTileIndex(startLat, startLng);

	// let's go with mercator space for now (Terrain2STL style)
	int y = nthLine;
	for (int x = 0; x < width; x++)
	{
		float u = (float)y / 1200;
		float v = (float)x / 1200;

		u *= stepSize;
		v *= stepSize;

		// get the lat and lng for each point
		float lat = startLat + u * cos(rot) + v * sin(rot);
		float lng = startLng + v * cos(rot) - u * sin(rot);

		// interpolate - maybe skip for rot0=0?

		float elevations[2][2];
		for (int a = 0; a < 2; a++)
		{ // x coord for Interpolation
			for (int b = 0; b < 2; b++)
			{ //

				// interesting bit of code here.
				// floor(...)/1200 as no distortion but cannot handle tile edges
				// x+float() handles edges well but with distortion
				// lat+(float)b/1200
				float intlat = floor(lat * 1200 + b) / 1200.0 + 0.000598907; // magic number??
				float intlng = floor(lng * 1200 + a) / 1200.0;
				// floor(lng*1200+a)/1200;

				if (getTileIndex(intlat, intlng) != tileNumber || elfile == NULL)
				{
					tileNumber = getTileIndex(intlat, intlng);
					getTileZip(tileNameZip, 100, intlat, intlng);
					getTile(tileName, 100, intlat, intlng);
					if (tileData != NULL)
						free(tileData);
#warning "Handle the case where we can't open the file - return zeros for ocean elev"
					
					// check if tile is already loaded in heightFiles
					Tile *found4 = find_tile_by_name(&ta, "Edwin");
					if (found4 != NULL) {
						printf("Found Tile ID: %d, Name: %s, Height Data: %s\n", found4->id, found4->name, found4->heightData);
					} else {
						printf("Tile with name Edwin not found, need to load it.\n");
					}

					// else Open the ZIP archive and append to heightFiles
					tileData = read_file_from_zip(tileNameZip, tileName);

					if (tileData != NULL)
					{
						//printf("Content of %s:\n%s\n", tileName, tileData);
					}
					
				}

				int p = (int)(1201 * (intlng - floor(intlng)));	   // x or lng component
				p += (int)(1201 * (ceil(intlat) - intlat)) * 1201; // y or lat component

				if (tileData == NULL)
				{ // if we can't open the file, return height = 0
					fprintf(stderr, "Unable to open '%s' for reading!\n", tileName);
					h = 0;
				}
				else
				{ // otherwise read height from file buffer
					number[0] = tileData[p * 2];
					number[1] = tileData[p * 2 + 1];
					//fseek(elfile, p * 2, SEEK_SET);
					//fread(number, 1, 2, elfile);
					//fprintf(stdout, "Read from '%s' buffer position %d values %d %d\n", tileName, p*2, number[0], number[1]);

					h = number[1];
					if (h < 0)
					{
						h = h + 255;
					}
					h += number[0] << 8;
				}
				if (h == 0)
				{
					h -= waterDrop / vscale;
				}
				elevations[a][b] = (float)h;
			}
		}
		float fracLat = lat - floor(lat * 1200) / 1200;
		float fracLng = lng - floor(lng * 1200) / 1200;
		fracLat *= 1200;
		fracLng *= 1200;
		float westLng = elevations[0][0] * (1 - fracLat) + elevations[0][1] * fracLat;
		float eastLng = elevations[1][0] * (1 - fracLat) + elevations[1][1] * fracLat;

		// not sure about this...
		float intHeight = westLng * (1 - fracLng) + eastLng * fracLng;
		heights[x] = intHeight * vscale + baseHeight;
	}
	if (tileData != NULL)
		free(tileData);
		fprintf(stdout, "Freed tileData for %s\n", tileName);
	tileData = NULL;
	return 1;
}
