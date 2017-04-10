//
// hmap.c 
// Applies Homography transformation by mapping pixels from a source
// image to a destination image.
// 11/9/2011..........W. Cochran wcochran@vancouver.wsu.edu  
//
// Usage: ./hmap <image.ppm> <W> <H>
//  Reads 3x3 homography matrix from stdin.
//  image.ppm: source image
//  W x H output image written to stdout.
//
// Input and output image are in PPM format.
//  
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
  int w, h;
  unsigned char *pixels;
} Image;

Image *createImage(int w, int h) {
  Image *image = (Image *) malloc(sizeof(Image));
  image->w = w;
  image->h = h;
  image->pixels = (unsigned char *) malloc(3*w*h);
  return image;
}

void destroyImage(Image *image) {
  free(image->pixels);
  free(image);
}

//
// Read pixel at (x,y) from image via bilinear interpolation.
// If (x,y) out of bounds, a black pixel is returned.
//
void lerp(Image *image, float x, float y, float pix[3]) {
  if (x < 0 || y < 0 || x >= image->w-1 || y >= image->h-1) {
    pix[0] = pix[1] = pix[2] = 0.0;
    return;
  }

  const int r0 = (int) y;
  const int c0 = (int) x;
  const int r1 = r0 + 1;
  const int c1 = c0 + 1;

  const unsigned char *p00 = &image->pixels[3*(r0*image->w + c0)];
  const unsigned char *p10 = &image->pixels[3*(r0*image->w + c1)];
  const unsigned char *p01 = &image->pixels[3*(r1*image->w + c0)];
  const unsigned char *p11 = &image->pixels[3*(r1*image->w + c1)];

  const float s = x - c0;
  float top[3], bot[3];
  for (int k = 0; k < 3; k++) {
    top[k] = (1-s)*p00[k] + s*p10[k];
    bot[k] = (1-s)*p01[k] + s*p11[k];
  }
  
  const float t = y - r0;
  for (int k = 0; k < 3; k++)
    pix[k] = (1-t)*top[k] + t*bot[k];
}

//
// Read raw or ascii PPM image file.
//
Image *readImage(const char *fname) {
  FILE *f;
  if ((f = fopen(fname, "rb")) == NULL) {
    perror(fname);
    exit(-1);
  }
  static char buf[200];
  if (fgets(buf, sizeof(buf), f) == NULL ||
      (strncmp(buf, "P3", 2) != 0 && strncmp(buf, "P6", 2) != 0)) {
    fprintf(stderr, "%s: not a PPM image!\n", fname);
    exit(-4);
  }
    
  const bool raw = buf[1] == '6';

  while (fgets(buf, sizeof(buf), f) != NULL && buf[0] == '#')
    ;
  int w, h;
  if (sscanf(buf, "%d %d", &w, &h) != 2) {
    fprintf(stderr, "%s: bogus image size!\n", fname);
    exit(-2);
  }
    
  while (fgets(buf, sizeof(buf), f) != NULL && buf[0] == '#')
    ;
  int maxval;
  if (sscanf(buf, "%d", &maxval) != 1 || maxval < 1) {
    fprintf(stderr, "%s: max channel value is bogus!\n", fname);
    exit(-2);
  }

  Image *image = createImage(w,h);
  
  if (raw) {
    if (maxval != 255) {
      fprintf(stderr, "%s: max pixel value should be 255!\n", fname);
      exit(-2);
    }
    const int numPixels = 3*w*h;
    if (fread(image->pixels, 1, numPixels, f) != numPixels) {
      fprintf(stderr, "%s: error reading raw pixel values!\n", fname);
      exit(-2);
    }
  } else {
    const float s = 255.0/maxval;
    for (int r = 0; r < h; r++)
      for (int c = 0; c < w; c++)
	for (int k = 0; k < 3; k++) {
	  int val;
	  if (fscanf(f, "%d", &val) != 1) {
	    fprintf(stderr, "%s: unexpected EOF\n", fname);
	    exit(-2);
	  }
	  if (val < 0 || val > maxval) {
	    fprintf(stderr, "%s: pixel value of out range\n", fname);
	    exit(-2);
	  }
	  image->pixels[3*(r*w + c) + k] = (unsigned char) (s*val + 0.5);
	}
  }

  fclose(f);

  return image;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "usage: %s <image.ppm> <W> <H>\n", argv[0]);
    exit(1);
  }

  const char *fname = argv[1];
  Image *imageIn = readImage(fname);
  const int W = atoi(argv[2]);
  const int H = atoi(argv[3]);
  if (W <= 0 || H <= 0) {
    fprintf(stderr, "bogus output image size!\n");
    exit(-2);
  }

  double M[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (scanf("%lf", &M[i][j]) != 1) {
	fprintf(stderr, "unable to read H matrix!\n");
	exit(-3);
      }

  printf("P3\n");
  printf("%d %d\n", W, H);
  printf("255\n");
  
  for (int r = 0; r < H; r++) {
    for (int c = 0; c < W; c++) {
      const float u = M[0][0]*c + M[0][1]*r + M[0][2];
      const float v = M[1][0]*c + M[1][1]*r + M[1][2];
      const float w = M[2][0]*c + M[2][1]*r + M[2][2];
      const float x = u/w;
      const float y = v/w;
      float pix[3];
      lerp(imageIn, x, y, pix);
      for (int k = 0; k < 3; k++) {
	const int val = (int) (pix[k] + 0.5);
	printf("%d ", val);
      }
      printf("\n");
    }
  }

  destroyImage(imageIn);

  return 0;
}
