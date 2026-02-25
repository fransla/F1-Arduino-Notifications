#ifndef GETIMAGE_H
#define GETIMAGE_H

#include "track_assets.h"
#include <Arduino.h>
#include <string.h>

struct TrackAsset {
  const unsigned char *data;
  unsigned int length;
};

inline TrackAsset getTrackAssetForRace(const char *raceName) {
  if (strcmp(raceName, "Bahrain") == 0)
    return {images_320x240_bahrain_png, images_320x240_bahrain_png_len};
  if (strcmp(raceName, "Saudi Arabian") == 0)
    return {images_320x240_saudi_arabia_png,
            images_320x240_saudi_arabia_png_len};
  if (strcmp(raceName, "Australian") == 0)
    return {images_320x240_australia_png, images_320x240_australia_png_len};
  if (strcmp(raceName, "Azerbaijan") == 0)
    return {images_320x240_azerbaijan_png, images_320x240_azerbaijan_png_len};
  if (strcmp(raceName, "Miami") == 0)
    return {images_320x240_miami_png, images_320x240_miami_png_len};
  if (strcmp(raceName, "Emilia Romagna Grand Prix") == 0 ||
      strcmp(raceName, "Imola") == 0)
    return {images_320x240_imola_png, images_320x240_imola_png_len};
  if (strcmp(raceName, "Monaco") == 0)
    return {images_320x240_monaco_png, images_320x240_monaco_png_len};
  if (strcmp(raceName, "Barcelona-Catalunya") == 0)
    return {images_320x240_spain_png, images_320x240_spain_png_len};
  if (strcmp(raceName, "Canadian") == 0)
    return {images_320x240_canada_png, images_320x240_canada_png_len};
  if (strcmp(raceName, "Austrian") == 0)
    return {images_320x240_austria_png, images_320x240_austria_png_len};
  if (strcmp(raceName, "British") == 0)
    return {images_320x240_british_png, images_320x240_british_png_len};
  if (strcmp(raceName, "Hungarian") == 0)
    return {images_320x240_hungary_png, images_320x240_hungary_png_len};
  if (strcmp(raceName, "Belgian") == 0)
    return {images_320x240_belgian_png, images_320x240_belgian_png_len};
  if (strcmp(raceName, "Dutch") == 0)
    return {images_320x240_dutch_png, images_320x240_dutch_png_len};
  if (strcmp(raceName, "Italian") == 0)
    return {images_320x240_italian_png, images_320x240_italian_png_len};
  if (strcmp(raceName, "Singapore") == 0)
    return {images_320x240_singapore_png, images_320x240_singapore_png_len};
  if (strcmp(raceName, "Japanese") == 0)
    return {images_320x240_japan_png, images_320x240_japan_png_len};
  if (strcmp(raceName, "Qatar") == 0)
    return {images_320x240_qatar_png, images_320x240_qatar_png_len};
  if (strcmp(raceName, "United States") == 0)
    return {images_320x240_usa_png, images_320x240_usa_png_len};
  if (strcmp(raceName, "Mexican") == 0)
    return {images_320x240_mexico_png, images_320x240_mexico_png_len};
  if (strcmp(raceName, "Brazilian") == 0)
    return {images_320x240_brazil_png, images_320x240_brazil_png_len};
  if (strcmp(raceName, "Las Vegas") == 0)
    return {images_320x240_vegas_png, images_320x240_vegas_png_len};
  if (strcmp(raceName, "Abu Dhabi") == 0)
    return {images_320x240_abudhabi_png, images_320x240_abudhabi_png_len};
  if (strcmp(raceName, "Chinese") == 0)
    return {images_320x240_china_png, images_320x240_china_png_len};
  if (strcmp(raceName, "Spanish") == 0)
    return {images_320x240_madrid_png, images_320x240_madrid_png_len};

  return {images_320x240_imagenotfound_png,
          images_320x240_imagenotfound_png_len};
}

inline int getImage(const char *raceName) { return 1; }

#endif
