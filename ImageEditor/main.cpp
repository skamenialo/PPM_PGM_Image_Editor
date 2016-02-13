#include <iostream>
#include "Image.cpp"

using namespace std;

void printHash(){
    cout<<"\n##################################\n";
}

int main()
{
    int select;//wybory uzytkownika
    Image *img=NULL,//wskazni na obiekt obrazka
        *imgTmp=NULL;//pomocniczy wskaznik obrazka
    string pathIn=new char[256],//sciezka do obrazka
           pathOut=new char[256];//sciezka do esportu obrazka

    cout<<"Enter file path: ";
    do{
        cin>>pathIn;
        img=Image::read(pathIn.c_str());
    }while(img==NULL);
    printHash();
    cout<<"\nReaded image: "<<pathIn<<endl;
    img->printInfo();
    printHash();

        cout<<"\n\nChoose an option:\n"<<endl;
        cout<<"1)Flip horizontal"<<endl;
        cout<<"2)Flip vertical"<<endl;
        cout<<"3)Crop"<<endl;
        cout<<"4)Zoom in"<<endl;
        cout<<"5)Save and exit"<<endl;

    do
    {
        cout<<"\nGive your selection: ";
        cin>>select;
        cout<<endl;
        switch(select)
        {
        case 1:
            imgTmp=img->flipHorizontal();
            delete img;
            img=imgTmp;
            imgTmp=NULL;
            cout<<"done\n";
            break;
        case 2:
            imgTmp=img->flipVertical();
            delete img;
            img=imgTmp;
            imgTmp=NULL;
            cout<<"done\n";
            break;
        case 3:
            int x1,y1,x2,y2;
            do{
                cout<<"Set left, top corner:\nx: ";
                cin>>x1;
                cout<<"y :";
                cin>>y1;
                cout<<"Set right, bottom corner:\nx: ";
                cin>>x2;
                cout<<"y :";
                cin>>y2;
                imgTmp=img->cropImage(x1,y1,x2,y2);
            }while(imgTmp==NULL);
			delete img;
            img=imgTmp;
            imgTmp=NULL;
            cout<<"done\n";
            break;
        case 4:
            int s;
            do{
                cout<<"Set scale: ";
                cin>>s;
                imgTmp=img->zoomInImage(s);
            }while(imgTmp==NULL);
			delete img;
            img=imgTmp;
            imgTmp=NULL;
            cout<<"done\n";
            break;
        case 5:
            cout<<"Enter path to save file: ";
            cin>>pathOut;
            img->save(pathOut.c_str());

            printHash();
            cout<<"\nSaved image: "<<pathOut<<endl;
            img->printInfo();
            printHash();
            break;
        default:
            break;
        }
    }
    while(select!=5);

    return 0;
}
