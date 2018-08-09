/* MIT License

Copyright (c) 2018 Biro Eniko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace std;

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)> SOURCEIMAGE\n"
              << "\tOptions:\n"
              << "\t-h, --help\tshow this help message and exit\n"
              << "\t-t ACTION\tIf set to 0 the program encodes the input file, if it is set to 1, it decodes it. If it is set to 2, it does both. \n"
              << "\tSOURCEIMAGE:\n"
              << "\tThe path of the source image is needed"
              << std::endl;
}

unsigned char* readPPM(string fileName, char* pSix, int& width, int& height, int& maximum) {
    // open the file to read
    FILE* fr = fopen(fileName.c_str(), "rb");

    // formatted read of header
    char line[256];
    fgets(line, 256, fr);
    sscanf(line, "%s", pSix);

    // check to see if it's a PPM image file
    if (strncmp(pSix, "P6", 2) != 0) {
        printf("The file format is not PPM\n");
    } else {
        printf("The file format is PPM, OK!\n");
    }

    // read the width, height and maximum
    fgets(line, 256, fr);
    sscanf(line, "%d %d", &width, &height);
    fgets(line, 256, fr);
    sscanf(line, "%d", &maximum);

    // check to see if they were stored properly
    printf("ENCODING\n");
    printf("PSix: %s\n", pSix);
    printf("Width: %d\n", width);
    printf("Height: %d\n", height);
    printf("maximum: %d\n\n", maximum);

    int size = width * height * 3;

    // allocate array for pixels
    unsigned char* pixels = new unsigned char[size];

    // unformatted read of binary pixel data
    fread(pixels, 1, size, fr);

    // close file
    fclose(fr);

    // return the array
    return pixels;

}

unsigned char* readEncoded(string fileName, unsigned char** occurences, int& width, int& height, int& maximum)
{
    // open the file to read
    FILE* fr = fopen(fileName.c_str(), "rb");

    // read the width, height and maximum
    fscanf(fr, "%d\n %d\n", &width, &height);
    fscanf(fr, "%d\n", &maximum);

    // check to see if they were stored properly
    printf("DECODING\n");
    printf("Width: %d\n", width);
    printf("Height: %d\n", height);
    printf("maximum: %d\n\n", maximum);

    int size = width * height * 3;
    int sizeOccurences = width * height;

    // allocate array for pixels
    unsigned char* encodedPixels = new unsigned char[size];
    *occurences = new unsigned char[sizeOccurences];

    unsigned char* readOccurence = *occurences;
    unsigned char* readEncoded = encodedPixels;

    while (fread(readOccurence++, 1, 1, fr)) 
    {
        fread(readEncoded, 1, 3, fr);
        readEncoded += 3;
    }
    fclose(fr);
    return encodedPixels;
}

void encode(std::string path)
{
    char pSix[10];		// indicates this is a PPM image
    int width = 0;		// width of the image
    int height = 0;		// heigt of the image
    int maximum = 0;	// maximum pixel value

    // read the PPM file and store its contents inside an array and return the pointer to that array to pixelArray
    unsigned char* pixelArray = readPPM(path, pSix, width, height, maximum);
    std::vector <unsigned char> encodedPixels;
    std::vector <unsigned char> occurence;

    bool same = false;
    bool first = true;

    int length = 0;

    for (int i = 0; i < width * height * 3; i+=3)
    {
        if (!first && pixelArray[i] == encodedPixels[encodedPixels.size() - 3]
                   && pixelArray[i+1] == encodedPixels[encodedPixels.size() - 2]
                   && pixelArray[i+2] == encodedPixels[encodedPixels.size() - 1]
            )
        {
            same = true;
        }
        else
        {
            first = false;
            same = false;
        }

        if (same)
        {
            if (occurence[occurence.size() - 1] == 255)
            {
                encodedPixels.push_back(pixelArray[i]);
                encodedPixels.push_back(pixelArray[i+1]);
                encodedPixels.push_back(pixelArray[i+2]);
                occurence.push_back(1);
                length++;
            }
            else
                occurence[occurence.size() - 1] += 1;
        }
        else
        {
            encodedPixels.push_back(pixelArray[i]);
            encodedPixels.push_back(pixelArray[i+1]);
            encodedPixels.push_back(pixelArray[i+2]);
            occurence.push_back(1);
            length++;
        }
    }

    string outputPath = path.substr(0, path.size()-4) + ".rle";
    FILE *fp = fopen(outputPath.c_str(), "wb");
    fprintf(fp, "%d %d\n255\n", width, height);

    for (int i = 0; i < length; i++)
    {
        fwrite(&occurence[i], 1, 1, fp);
        fwrite(&encodedPixels[i*3], 1, 3, fp);
    }
    fclose(fp);
}

void decode(std::string path)
{
    int width = 0;		// width of the image
    int height = 0;		// heigt of the image
    int maximum = 0;	// maximum pixel value

    unsigned char* occurences;
    unsigned char* encodedPixels = readEncoded(path, &occurences, width, height, maximum);

    string newPath = path.substr(0, path.size()-4) + "_decoded.ppm";
    FILE *fp = fopen(newPath.c_str(), "wb");
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    int pixelCounter = 0;
    for (int i = 0; i < width*height; )
    {
        for (int k = 0; k < occurences[pixelCounter]; k++)
        {
            fwrite(&encodedPixels[pixelCounter*3], 1, 3, fp);
            i++;        // we wrote one pixel
        }
        pixelCounter++;
    }
    fclose(fp);
}


unsigned char* readEncodedPackBits(string fileName, int& width, int& height, int& maximum, int& size)
{
    // open the file to read
    FILE* fr = fopen(fileName.c_str(), "rb");

    // read the width, height and maximum
    fread(&width, 1, sizeof(int), fr);
    fread(&height, 1, sizeof(int), fr);
    maximum = 255;

    // check to see if they were stored properly
    printf("DECODING\n");
    printf("Width: %d\n", width);
    printf("Height: %d\n", height);
    printf("maximum: %d\n\n", maximum);

    int headerSize = ftell(fr);
    fseek(fr, 0, SEEK_END);
    size = ftell(fr) - headerSize;
    fseek(fr, headerSize, SEEK_SET);

    // allocate array for pixels
    unsigned char* encodedData = new unsigned char[size];
    fread(encodedData, 1, size, fr);
    fclose(fr);
    return encodedData;
}

void encodeImagePackBits(std::string path)
{
    char pSix[10];		// indicates this is a PPM image
    int width = 0;		// width of the image
    int height = 0;		// heigt of the image
    int maximum = 0;	// maximum pixel value

    // read the PPM file and store its contents inside an array and return the pointer to that array to pixelArray
    unsigned char* pixelArray = readPPM(path, pSix, width, height, maximum);
    std::vector <unsigned char> encodedPixels;
    std::vector <char> occurence;

    bool first = true;

    for (int i = 0; i < width * height * 3; i+=3)
    {
        if (first)
        {
            encodedPixels.push_back(pixelArray[i]);
            encodedPixels.push_back(pixelArray[i+1]);
            encodedPixels.push_back(pixelArray[i+2]);
            occurence.push_back(0);
            first = false;
            continue;
        }

        bool same = pixelArray[i] == encodedPixels[encodedPixels.size() - 3]
                    && pixelArray[i+1] == encodedPixels[encodedPixels.size() - 2]
                    && pixelArray[i+2] == encodedPixels[encodedPixels.size() - 1];

        if (same)
        {
            if (occurence.back() <= 0)
            {
                if (occurence.back() == -127)
                {
                    encodedPixels.push_back(pixelArray[i]);
                    encodedPixels.push_back(pixelArray[i+1]);
                    encodedPixels.push_back(pixelArray[i+2]);
                    occurence.push_back(0);
                }
                else
                    occurence.back()--;
            }
            else
            {
                occurence.back()--;
                occurence.push_back(-1);
            }
        }
        else
        {
            encodedPixels.push_back(pixelArray[i]);
            encodedPixels.push_back(pixelArray[i+1]);
            encodedPixels.push_back(pixelArray[i+2]);

            if (occurence.back() < 0 || occurence.back() == 127)
                occurence.push_back(0);
            else
                occurence.back()++;
        }
    }
    occurence.push_back(-128);

    string outputPath = path.substr(0, path.size()-4) + "_packbits.rle";
    FILE *fp = fopen(outputPath.c_str(), "wb");
    fwrite(&width, 1, sizeof(int), fp);
    fwrite(&height, 1, sizeof(int), fp);

    int pixelCounter = 0;
    for (int i = 0; i < occurence.size(); i++)
    {
        fwrite(&occurence[i], 1, 1, fp);
        // if n is negative, we have to  repeat 1 - n times
        if (occurence[i] < 0 && occurence[i] != -128)
        {
            fwrite(&encodedPixels[pixelCounter], 1, 3, fp);
            pixelCounter+=3;
        }
        else if (occurence[i] >= 0)
        {
            for (int k = 0; k < occurence[i] + 1; k++)
            {
                fwrite(&encodedPixels[pixelCounter], 1, 3, fp);
                pixelCounter+=3;
            }
        }
        else
        {
            break;
        }
    }
    fclose(fp);
}

void decodeImagePackBits(std::string path)
{
    int width = 0;		// width of the image
    int height = 0;		// heigt of the image
    int maximum = 0;	// maximum pixel value

    int size;
    unsigned char* encodedData = readEncodedPackBits(path, width, height, maximum, size);

    string newPath = path.substr(0, path.size()-4) + "_decoded.ppm";
    FILE *fp = fopen(newPath.c_str(), "wb");
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    for (int i = 0; i < size; )
    {
        int occurence = (char)encodedData[i];
        i++;

        if (occurence < 0 && occurence != -128)
        {
            for (int k = 0; k < 1 - occurence; k++)
            {
                fwrite(&encodedData[i], 1, 3, fp);
            }
            i += 3;
        }
        else if (occurence >= 0)
        {
            for (int k = 0; k < occurence + 1; k++)
            {
                fwrite(&encodedData[i], 1, 3, fp);
                i += 3;
            }
        }
        else
        {
            break;
        }
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        show_usage(argv[0]);
        return 1;
    }
    std::vector <std::string> sources;
    std::string destination;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else if ((arg == "-t") || (arg == "--type")) {
            if (argc < 4)
            {
                show_usage(argv[0]);
                return 1;
            }
            if (i + 1 < argc) {                                     // Make sure we aren't at the end of argv!
                destination = argv[++i];
            } else {
                  std::cerr << "--type option needs one argument." << std::endl;
                return 1;
            }
        } else {
            sources.push_back(argv[i]);
        }
    }
    if (destination == "" || sources.size() != 1 || (destination != "0" && destination != "1" && destination != "2" && destination != "3"))
    {
        show_usage(argv[0]);
        return 1;
    }
    /*
    0 - encode image
    1 - decode image
    2 - encode and decode image
    3 - encode and decode image with the PackBits algorithm
    */
    if (destination == "0")
            encode(sources[0]);
    else if (destination == "1")
            decode(sources[0]);
    else if (destination == "2")
    {
        encode(sources[0]);
        decode(sources[0].substr(0, sources[0].size()-4) + ".rle");
    }
    else
    {
        encodeImagePackBits(sources[0]);
        decodeImagePackBits(sources[0].substr(0, sources[0].size()-4) + "_packbits.rle");
    }

    return 0;
}

