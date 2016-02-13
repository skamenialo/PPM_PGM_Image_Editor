#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;
class Image
{
public:
	//Skladowe koloru
    struct Rgb
    {
		//czerwony, zielony, niebieski
        float r, g, b;

        Rgb() : r(0), g(0), b(0) {}
        Rgb(float c) : r(c), g(c), b(c) {}
        Rgb(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
        Rgb(Rgb &rgb):r(rgb.r),g(rgb.g),b(rgb.b) {}
    };

	//paramerty obrazka
    int width, height, depth;
    Rgb **pixels;//wskaznik na tablice 2D
    string header;

    Image() : width(0), height(0), pixels(NULL) { }
    Image(const string _header,const unsigned int _w, const unsigned int _h,const int _d, Rgb** _p) : header(_header), width(_w), height(_h),depth(_d), pixels(_p) { }
    Image(const string _header, const unsigned int &_w, const unsigned int &_h,const int &_d, ifstream &_ifs) : header(_header), width(_w), height(_h),depth(_d)
    {
        if(_header=="P2"||_header=="P5")
            pixels=readPGM(_ifs);
        else if(_header=="P3"||_header=="P6")
            pixels = readPPM(_ifs);
    }
    ~Image()
    {
        if (pixels != NULL)
        {
            for(int i=0; i<height; i++)
                delete [] pixels[i];
            delete [] pixels;
        }
    }

    Image* cropImage(int x1, int y1, int x2, int y2)
    {
        try
        {
            if (x1<0 || y1<0 || x2>=width || y2>=height)
            {
                throw("Crop out of bounds");
            }
            int w=x2-x1+1,
                h=y2-y1+1;
            if(w<=0 || h<=0)
            {
                throw("Wrong order");
            }
            Rgb **pix=new Rgb*[h];
            for(int i=0; i<h; i++)
            {
                pix[i]=new Rgb[w];
                for(int j=0; j<w; j++)
                {
					//przypisanie kolejnych wartosci z fragmentu obrazka
                    pix[i][j]=Rgb(pixels[y1+i][x1+j]);
                }
            }
            return new Image(header,w,h,depth,pix);
        }
        catch(const char *err)
        {
            fprintf(stderr, "%s\n", err);
            return NULL;
        }
    }
    Image* zoomInImage(int scale)
    {
        try
        {
            if(scale<=0)
                throw("Unsupported negative scale");
            int w=width*scale,
                h=height*scale;
            if(w>500||h>500)
                throw("Too large image after zooming");
            Rgb **pix=new Rgb*[h];
            for(int i=0; i<height; i++)
            {
                for(int k=i*scale; k<(i + 1) * scale; k++)
                {
                    pix[k]=new Rgb[w];
                    for(int j=0; j<width; j++)
                    {
                        for(int l=j*scale; l<(j+1)*scale; l++)
                        {
							//kilkukrotne przypisanie jednej wartosci z obrazka
                            pix[k][l]=Rgb(pixels[i][j]);
                        }
                    }
                }
            }
            return new Image(header,w,h,depth,pix);
        }
        catch(const char *err)
        {
            fprintf(stderr, "%s\n", err);
            return NULL;
        }
    }
    Image* flipHorizontal()
    {
        Rgb **pix=new Rgb*[height];
        for(int i=0; i<height; i++)
        {
            pix[i]=new Rgb[width];
        }
        for(int i=0; i<height; i++)
        {
            for(int j=0; j<width; j++)
            {
				//przypisanie ostatniej wartosci wiersza, pierwszej wartosci wiersza w nowej tablicy
                pix[i][j]=Rgb(pixels[i][width-1-j]);
            }
        }
        return new Image(header,width,height,depth,pix);;
    }

    Image* flipVertical()
    {
        Rgb **pix=new Rgb*[height];
        for(int i=0; i<height; i++)
        {
            pix[i]=new Rgb[width];
        }
        for(int i=0; i<height; i++)
        {
            for(int j=0; j<width; j++)
            {
				//przypisanie ostatniej wartosci kolumny, pierwszej wartosci kolumny w nowej tablicy
                pix[i][j]=Rgb(pixels[height-1-i][j]);
            }
        }
		Image *img=new Image(header,width,height,depth,pix);
		delete this;
        return img;
    }

    void save(const char *filename)
    {
        ofstream ofs;
        try
        {
            if (width == 0 || height == 0)
            {
                throw("Can't save an empty image");
            }
            ofs.open(filename, ios::binary);

            if (ofs.fail())
                throw("Can't open output file");
            if(header=="P2"||header=="P5")
                savePGM(ofs);
            else if(header=="P3"||header=="P6")
                savePPM(ofs);
            ofs.close();
        }
        catch (const char *err)
        {
            fprintf(stderr, "%s\n", err);
            ofs.close();
        }
    }

    void printInfo(){
        cout<<"\ntype: "<<header
            <<"\nsize: "<<width<<", "<<height
            <<"\ncolor depth: "<<depth<<endl;
    }

    static Image* read(const char *filename)
    {
        ifstream ifs;
        ifs.open(filename, ios::binary);
        Image *img=NULL;
        try
        {
            if (ifs.fail())
            {
                throw("Can't open input file");
            }
            string header;
            int w, h;
            float d;

            ifs >> header;

            if (strcmp(header.c_str(), "P6") != 0)
                if (strcmp(header.c_str(), "P5") != 0)
                    if (strcmp(header.c_str(), "P3") != 0)
                        if (strcmp(header.c_str(), "P2") != 0)
                            throw("Can't read input file");

            ifs >> w >> h >> d;
            //ifs.get();
            if(w>500||h>500)
            {
                throw("Image is out of bounds");
            }
            img=new Image(header,w,h,d,ifs);
            ifs.close();
        }
        catch (const char *err)
        {
            fprintf(stderr, "%s\n", err);
            ifs.close();
        }
        return img;
    }
private:

    Rgb** readPPM(ifstream &ifs)
    {
        Rgb **pixels = new Rgb*[height];
        unsigned char P6[3];//pomocnicza tablica do odczytu znakow dla naglowka P6
        unsigned int P3[3];//pomocnicza zmienne do odczytu znakow dla naglowka P3

        for (int i = 0; i <height; i++)
        {
            pixels[i]=new Rgb[width];
            for(int j=0; j<width; j++)
            {
                if(header=="P6")
                {
					//pobieranie kolejnych trzech wartosci binarnych do tablicy 3-znakowej
                    ifs.read(reinterpret_cast<char *>(P6), 3);//wykozystane ze zrodla
					//przypisanie koloru do pozycji w tablicy z zakresu 0-1
                    pixels[i][j]=Rgb(P6[0]/ (float)depth,P6[1]/ (float)depth,P6[2]/ (float)depth);
                }
                else if(header=="P3")
                {
					//pobieranie kolejnych trzech wartosci liczbowych do tablicy 3-liczbowej
                    ifs>>P3[0]>>P3[1]>>P3[2];
                    pixels[i][j]=Rgb(P3[0]/ (float)depth,P3[1]/ (float)depth,P3[2]/ (float)depth);
                }
            }
        }
        return pixels;
    }

    void savePPM(ofstream &ofs)
    {
        //zapisanie naglowka
        ofs << header<<endl<< width << " " << height <<endl<<depth<<endl;
        unsigned char p6[3];//pomocnicza tablica do odczytu znakow dla naglowka P6
        unsigned int p3[3];//pomocnicza tablica do odczytu znakow dla naglowka P3
        for (int i = 0; i < height; i++)
        {
            for(int j=0; j<width; j++)
            {
                if(header=="P6")
                {
					//pobieranie kolejnych trzech wartosci binarnych do tablicy 3-znakowej
                    p6[0] = static_cast<unsigned char>(min(1.f, pixels[i][j].r) * (float)depth);//wykozystane ze zrodla
                    p6[1] = static_cast<unsigned char>(min(1.f, pixels[i][j].g) * (float)depth);
                    p6[2] = static_cast<unsigned char>(min(1.f, pixels[i][j].b) * (float)depth);
                    ofs << p6[0]<<p6[1]<<p6[2];
                }
                else if(header=="P3")
                {
					//pobieranie kolejnych trzech wartosci liczbowych do tablicy 3-liczbowej
                    p3[0]=pixels[i][j].r*(float)depth;
                    p3[1]=pixels[i][j].g*(float)depth;
                    p3[2]=pixels[i][j].b*(float)depth;
                    ofs << p3[0] <<" "<< p3[1] <<" "<< p3[2]<<" ";
                }
            }
        }
    }

    Rgb** readPGM(ifstream &ifs)
    {
        Rgb **pixels = new Rgb*[height];
        char *g1;//pomocnicza zmienna do odczytu znaku dla naglowka P5
        unsigned int g2;//pomocnicza zmienna do odczytu znaku dla naglowka P2

        for (int i = 0; i <height; i++)
        {
            pixels[i]=new Rgb[width];
            for(int j=0; j<width; j++)
            {
                if(header=="P5")
                {
					//pobieranie kolejnej wartosci binarnej do zmiennej znakowej
                    ifs.read(g1, 1);
                    pixels[i][j]=Rgb(g1[0]/ (float)depth);
                }
                else if(header=="P2")
                {
					//pobieranie kolejnej wartosci liczbowej do zmiennej liczbowej
                    ifs>>g2;
                    pixels[i][j]=Rgb(g2/ (float)depth);
                }
            }
        }
        return pixels;
    }

    void savePGM(ofstream &ofs)
    {
        ofs << header<<"\n" << width << " " << height << "\n"<<depth<<"\n";
        unsigned char g1;//pomocnicza zmienna do odczytu znaku dla naglowka P5
        unsigned int g2;//pomocnicza zmienna do odczytu znaku dla naglowka P2

        for (int i = 0; i < height; i++)
        {
            for(int j=0; j<width; j++)
            {
                if(header=="P5")
                {
					//pobieranie kolejnej wartosci binarnej do zmiennej znakowej
                    g1 = static_cast<unsigned char>(min(1.f, pixels[i][j].g) * (float)depth);//wykozystane ze zrodla
                    ofs <<g1;
                }
                else if(header=="P2")
                {
					//pobieranie kolejnej wartosci liczbowej do zmiennej liczbowej
                    g2=pixels[i][j].g*(float)depth;
                    ofs << g2 <<" ";
                }
            }
        }
    }
};
