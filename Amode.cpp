#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

float **createDataMatrix(int numElement, int numSample)
{
    // Create a 2D data matrix of size numElement and numSample
    //declaring the double pointer allocating pointer with numElements
    float** RFData = new float*[numElement];

    //using loop to allocate new float array of length numSample
    for (int i = 0; i < numElement; i++)
    {
        RFData[i] = new float[numSample];
    }
    //returning RFData
    return RFData;
}
int loadRFData(float **RFData, const char *fileName, int numElement, int numSample)
{
    // Open the text file fileName, read the data and store into RFData
    //opening file using ifstream
    ifstream input(fileName);

    //reading data into array
    if(input.is_open())
    {
        for (int i = 0; i < numElement; i++)
        {
            for (int j = 0; j < numSample; j++)
            {
                char s[20];
                //convert from string to float
                input.getline(s,20,'\n'); 
                RFData[i][j] = atof(s);
            }
        }
    }
    else
    {
        return -1;
    }
    return 0;
}
float *genScanlineLocation(int &numPixel)
{
    float *scanLineLocation = new float[numPixel];
    float depth;

    //prompt user for depth of scanline
    cout<<"Enter how deep the scanline should be: ";
    cin>>depth;

    //prompt user for number of pixels of scanline
    cout<<"Enter the number of pixels of the scanline: ";
    cin>>numPixel;

    //creating an array
    for (int j = 0; j < numPixel; j++)
    {
        scanLineLocation[j] = (j*depth)/(numPixel-1);
    }
    return scanLineLocation;
}

// Create an array containing the element location (in x-direction) of the ultrasound transducer
float *genElementLocation(int numElement, float PITCH)
{
    //create array with numElement elements
    float* eleLocation = new float[numElement];
    for (int n = 0; n < numElement; n++)
    {
        eleLocation[n] = (n - ((numElement-1)/2.0))*PITCH;
    }
    return eleLocation;
}

// Allocate memory to store the beamformed scanline
float *createScanline(int numPixel)
{
    //storing results of beamforming into scanline
    float* scanline = new float[numPixel];
    return scanline;
}

// Beamform the A-mode scanline
void beamform(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel, float FS, float SoS)
{
    //declaring the variables
    float tForward[numPixel];
    float tBackward[numPixel][numElement];
    float tTotal[numPixel][numElement];
    int sSample[numPixel][numElement];
    float Preal[numPixel];
    float Pimag[numPixel];

    for (int i = 0; i < numPixel; i++)
    {
        //calculate tForward
        tForward[i] = (scanlinePosition[i])/SoS;

        //setting Preal and Pimag to zero
        Preal[i] = 0.0;
        Pimag[i] = 0.0;
        for (int j = 0; j < numElement; j++)
        {
            //calculate tBackward
            tBackward[i][j] = (sqrt(pow(scanlinePosition[i], 2) + pow(elementPosition[j],2))/SoS);
            //calculate tTotal
            tTotal[i][j] = tForward[i] + tBackward[i][j];
            //finding sample number sSample
            sSample[i][j] = (floor(tTotal[i][j] * FS));
            //calculating Preal and Pimag
            Preal[i] = Preal[i] + realRFData[j][sSample[i][j]];
            Pimag[i] = Pimag[i] + imagRFData[j][sSample[i][j]];
        } 
        //magnitude of scanline using imaginary and real part of the echo signal
        scanline[i]= sqrt(pow(Preal[i],2)+pow(Pimag[i],2));
    }
}

// Write the scanline to a csv file
int outputScanline(const char *fileName, float *scanlinePosition, float *scanline, int numPixel)
{
    ofstream outputFile(fileName);
    if (!outputFile.is_open())
    {
        //returning -1 to indicate that the file was unsuccessfully created or opened
        cout<< "There was an error in opening or creating the file named: "<< fileName<<endl;
        return -1;
    }
    //iterating through each scanline location and storing them into the output file
    for (int i = 0; i < numPixel; i++)
    {
        //storing the scanlinepostion into the output file
        outputFile << scanlinePosition[i]<< "," << scanline[i]<<endl;
    }
    return 0;
}
// Destroy all the allocated memory
void destroyAllArrays(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel)
{
    //destroying the double pointers
    for (int i = 0; i < numElement;++i) 
    {
        delete realRFData[i];
        delete imagRFData[i];
    }
    //deleting the memory allocated before
    delete realRFData;
    delete imagRFData;
    delete scanline;
    delete scanlinePosition;
    delete elementPosition;
}