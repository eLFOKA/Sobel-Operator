#include <iostream>
#include <fstream>

using namespace std;

struct BITMAPFILEHEADER
{
    int bfType{ 0 };
    int bfSize{ 0 };
    int bfReserved1{ 0 };
    int bfReserved2{ 0 };
    int bfoffBits{ 0 };
};

struct BITMAPINFOHEADER
{
    int biSize{ 0 };
    int biWidth{ 0 };
    int biHeight{ 0 };
    int biPlanes{ 0 };
    int biBitCount{ 0 };
    int biCompression{ 0 };
    int biSizeImage{ 0 };
    int biXpelsPerMeter{ 0 };
    int biYpelsPerMeter{ 0 };
    int biCrlUses{ 0 };
    int biCrlImport{ 0 };

};

//odczytywanie nagłówka 
void readBFH(ifstream& ifs, BITMAPFILEHEADER& bfh);
void readBIH(ifstream& ifs, BITMAPINFOHEADER& bih);

//wypisanie na ekranie nagłówka
void writeBFIH(BITMAPFILEHEADER& bfh, BITMAPINFOHEADER& bih);

//zapisanie nagłówka
void saveBFH(ofstream& ofs, BITMAPFILEHEADER& bfh);
void saveBIH(ofstream& ofs, BITMAPINFOHEADER& bih);

//Engine
int engine();

int main() {

    engine();

    return 0;
}

int engine() {

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    string inPutFile;

    cout << "Enter the name of the input file: ";
    cin >> inPutFile;
    inPutFile = inPutFile + ".bmp";

    ifstream ifs;
    ifs.open(inPutFile, ios::binary | ios::in);

    if (!ifs) {
        cout << "Error! PLik o podanej nazwie nie istnieje, podaj nazwe jeszcze raz:" << endl;
        system("PAUSE");
        return 0;
    }

    readBFH(ifs, bfh);
    readBIH(ifs, bih);

    //dynamiczna tablice dwuwymiarowa przed konwersją
    int** tabB = new int* [bih.biHeight];
    for (int i = 0; i < bih.biHeight; ++i)
        tabB[i] = new int[bih.biWidth];

    int** tabG = new int* [bih.biHeight];
    for (int i = 0; i < bih.biHeight; ++i)
        tabG[i] = new int[bih.biWidth];

    int** tabR = new int* [bih.biHeight];
    for (int i = 0; i < bih.biHeight; ++i)
        tabR[i] = new int[bih.biWidth];

    int ilosc_zer = ((bfh.bfSize - bfh.bfoffBits) - 3 * bih.biHeight * bih.biWidth) / bih.biHeight;

    //wczytywanie pikseli
    for (int i = 0; i < bih.biHeight; ++i) {
        for (int j = 0; j < bih.biWidth; ++j) {
            ifs.read(reinterpret_cast<char*>(&tabB[i][j]), 1);
            ifs.read(reinterpret_cast<char*>(&tabG[i][j]), 1);
            ifs.read(reinterpret_cast<char*>(&tabR[i][j]), 1);
        }
        ifs.ignore(ilosc_zer);

    }
    ifs.close();

    writeBFIH(bfh, bih);

    //dynamiczna tablice trójwymarowe przechowujące wyniki operacji sobela
    int*** sobel_tabB = new int** [8];
    for (int i = 0; i < 8; ++i) {
        sobel_tabB[i] = new int* [bih.biHeight - 2];
        for (int j = 0; j < bih.biHeight - 2; ++j)
            sobel_tabB[i][j] = new int[bih.biWidth - 2];
    }

    int*** sobel_tabG = new int** [8];
    for (int i = 0; i < 8; ++i) {
        sobel_tabG[i] = new int* [bih.biHeight - 2];
        for (int j = 0; j < bih.biHeight - 2; ++j)
            sobel_tabG[i][j] = new int[bih.biWidth - 2];
    }

    int*** sobel_tabR = new int** [8];
    for (int i = 0; i < 8; ++i) {
        sobel_tabR[i] = new int* [bih.biHeight - 2];
        for (int j = 0; j < bih.biHeight - 2; ++j)
            sobel_tabR[i][j] = new int[bih.biWidth - 2];
    }

    //Table of Masks 
    int mask[8][3][3] = { { { -1, 0, 1 } , { -2, 0, 2 } , { -1, 0, 1 } },
                                                    { {0, 1, 2} , { -1, 0, 1} , { -2, -1, 0} } ,
                                                    { { 1, 2, 1} , { 0, 0, 0} , { -1, -2, -1} } ,
                                                    { { +2, +1, 0} , { +1, 0, -1} , { 0, -1, -2} } ,
                                                    { { 1, 0, -1} , { 2, 0, -2} , { 1, 0, -1} } ,
                                                    { { 0, -1, -2} , { 1, 0, -1} , { 2, 1, 0} } ,
                                                    { { -1 ,-2, -1} , { 0, 0, 0} , { 1, 2, 1} } ,
                                                    { { -2, -1, 0} , { -1, 0, 1} , { 0, 1, 2} } };

    // wykonanie operatora sobela
    for (int k = 0; k < 8; ++k) {

        for (int i = 0; i < bih.biHeight - 2; ++i) {
            for (int j = 0; j < bih.biWidth - 2; ++j) {

                // nowe powinno dzialać
                sobel_tabB[k][i][j] = mask[k][0][0] * tabB[i][j] +
                    mask[k][1][0] * tabB[i + 1][j] +
                    mask[k][2][0] * tabB[i + 2][j] +
                    mask[k][0][1] * tabB[i][j + 1] +
                    mask[k][1][1] * tabB[i + 1][j + 1] +
                    mask[k][2][1] * tabB[i + 2][j + 1] +
                    mask[k][0][2] * tabB[i][j + 2] +
                    mask[k][1][2] * tabB[i + 1][j + 2] +
                    mask[k][2][2] * tabB[i + 2][j + 2];

                if (sobel_tabB[k][i][j] > 255) {
                    sobel_tabB[k][i][j] = 255;
                }
                else if (sobel_tabB[k][i][j] < 0) {
                    sobel_tabB[k][i][j] = 0;
                }

                sobel_tabG[k][i][j] = mask[k][0][0] * tabG[i][j] +
                    mask[k][1][0] * tabG[i + 1][j] +
                    mask[k][2][0] * tabG[i + 2][j] +
                    mask[k][0][1] * tabG[i][j + 1] +
                    mask[k][1][1] * tabG[i + 1][j + 1] +
                    mask[k][2][1] * tabG[i + 2][j + 1] +
                    mask[k][0][2] * tabG[i][j + 2] +
                    mask[k][1][2] * tabG[i + 1][j + 2] +
                    mask[k][2][2] * tabG[i + 2][j + 2];

                if (sobel_tabG[k][i][j] > 255) {
                    sobel_tabG[k][i][j] = 255;
                }
                else if (sobel_tabG[k][i][j] < 0) {
                    sobel_tabG[k][i][j] = 0;
                }

                sobel_tabR[k][i][j] = mask[k][0][0] * tabR[i][j] +
                    mask[k][1][0] * tabR[i + 1][j] +
                    mask[k][2][0] * tabR[i + 2][j] +
                    mask[k][0][1] * tabR[i][j + 1] +
                    mask[k][1][1] * tabR[i + 1][j + 1] +
                    mask[k][2][1] * tabR[i + 2][j + 1] +
                    mask[k][0][2] * tabR[i][j + 2] +
                    mask[k][1][2] * tabR[i + 1][j + 2] +
                    mask[k][2][2] * tabR[i + 2][j + 2];

                if (sobel_tabR[k][i][j] > 255) {
                    sobel_tabR[k][i][j] = 255;
                }
                else if (sobel_tabR[k][i][j] < 0) {
                    sobel_tabR[k][i][j] = 0;
                }

            }
        }
        cout << "progress: " << double((k + 1) * 12.5) << '%'<< endl;
    }

    //deleting loaded image
    for (int i = 0; i < bih.biHeight; ++i) {
        delete[] tabB[i];
    }
    delete[] tabB;

    for (int i = 0; i < bih.biHeight; ++i) {
        delete[] tabG[i];
    }
    delete[] tabG;

    for (int i = 0; i < bih.biHeight; ++i) {
        delete[] tabR[i];
    }
    delete[] tabR;

    //dynamic 2D arrays for a new image
    int** new_tabB = new int* [bih.biHeight];
    for (int i = 0; i < bih.biHeight; ++i)
        new_tabB[i] = new int[bih.biWidth];

    int** new_tabG = new int* [bih.biHeight];
    for (int i = 0; i < bih.biHeight; ++i)
        new_tabG[i] = new int[bih.biWidth];

    int** new_tabR = new int* [bih.biHeight];
    for (int i = 0; i < bih.biHeight; ++i)
        new_tabR[i] = new int[bih.biWidth];

    //creating new image from received pixels after sobel operation
    int tmp1 = 0;
    int tmp2 = 0;
    int tmp3 = 0;

    for (int i = 0; i < bih.biHeight - 2; ++i) {
        for (int j = 0; j < bih.biWidth - 2; ++j) {

            for (int k = 0; k < 8; ++k) {
                tmp1 = tmp1 + sobel_tabB[k][i][j];
                tmp2 = tmp2 + sobel_tabG[k][i][j];
                tmp3 = tmp3 + sobel_tabR[k][i][j];
            }

            tmp1 = tmp1 / 8;
            tmp2 = tmp2 / 8;
            tmp3 = tmp3 / 8;

            new_tabB[i + 1][j + 1] = tmp1;
            new_tabG[i + 1][j + 1] = tmp2;
            new_tabR[i + 1][j + 1] = tmp3;
            tmp1 = 0;
            tmp2 = 0;
            tmp3 = 0;
        }
    }

    //deleting dynamic tables used by sobel operator
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < bih.biHeight - 2; j++)
            delete[] sobel_tabB[i][j];
        delete[] sobel_tabB[i];
    }
    delete[] sobel_tabB;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < bih.biHeight - 2; j++)
            delete[] sobel_tabG[i][j];
        delete[] sobel_tabG[i];
    }
    delete[] sobel_tabG;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < bih.biHeight - 2; j++)
            delete[] sobel_tabR[i][j];
        delete[] sobel_tabR[i];
    }
    delete[] sobel_tabR;

    string outPutFile;
    cout << "Enter the name of the output file: ";
    cin >> outPutFile;
    outPutFile = outPutFile + ".bmp";

    ofstream ofs;
    ofs.open(outPutFile, ios::binary | ios::out | ios::trunc);

    saveBFH(ofs, bfh);
    saveBIH(ofs, bih);

    //wczytywanie pikseli do nowego obrazu
    int tabTmp1 = 0;
    for (int i = 0; i < bih.biHeight; i++) {
        for (int j = 0; j < bih.biWidth; j++) {
            ofs.write(reinterpret_cast<char*>(&new_tabB[i][j]), 1);
            ofs.write(reinterpret_cast<char*>(&new_tabG[i][j]), 1);
            ofs.write(reinterpret_cast<char*>(&new_tabR[i][j]), 1);
        }
        for (int k = 0; k < ilosc_zer; k++)
        {
            ofs.write(reinterpret_cast<char*>(&tabTmp1), 1);
        }
    }
    ofs.close();

    //deleting loaded inPutFile
    for (int i = 0; i < bih.biHeight; ++i) {
        delete[] new_tabB[i];
    }
    delete[] new_tabB;

    for (int i = 0; i < bih.biHeight; ++i) {
        delete[] new_tabG[i];
    }
    delete[] new_tabG;

    for (int i = 0; i < bih.biHeight; ++i) {
        delete[] new_tabR[i];
    }
    delete[] new_tabR;

}

void readBFH(ifstream& ifs, BITMAPFILEHEADER& bfh) {
    ifs.read(reinterpret_cast<char*>(&bfh.bfType), 2);
    ifs.read(reinterpret_cast<char*>(&bfh.bfSize), 4);
    ifs.read(reinterpret_cast<char*>(&bfh.bfReserved1), 2);
    ifs.read(reinterpret_cast<char*>(&bfh.bfReserved2), 2);
    ifs.read(reinterpret_cast<char*>(&bfh.bfoffBits), 4);
}

void readBIH(ifstream& ifs, BITMAPINFOHEADER& bih) {
    ifs.read(reinterpret_cast<char*>(&bih.biSize), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biWidth), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biHeight), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biPlanes), 2);
    ifs.read(reinterpret_cast<char*>(&bih.biBitCount), 2);
    ifs.read(reinterpret_cast<char*>(&bih.biCompression), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biSizeImage), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biXpelsPerMeter), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biYpelsPerMeter), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biCrlUses), 4);
    ifs.read(reinterpret_cast<char*>(&bih.biCrlImport), 4);
}

void writeBFIH(BITMAPFILEHEADER& bfh, BITMAPINFOHEADER& bih) {

    cout << "bfType = " << bfh.bfType << endl;
    cout << "bfSize = " << bfh.bfSize << endl;
    cout << "bfReserved1 = " << bfh.bfReserved1 << endl;
    cout << "bfReserved2 = " << bfh.bfReserved2 << endl;
    cout << "bfoffBits = " << bfh.bfoffBits << endl;
    cout << endl;
    cout << "biSize = " << bih.biSize << endl;
    cout << "biWidth = " << bih.biWidth << endl;
    cout << "biHeight = " << bih.biHeight << endl;
    cout << "biPlanes = " << bih.biPlanes << endl;
    cout << "biBitCount = " << bih.biBitCount << endl;
    cout << "biCompression = " << bih.biCompression << endl;
    cout << "biSizeImage = " << bih.biSizeImage << endl;
    cout << "biXpelsPerMeter = " << bih.biXpelsPerMeter << endl;
    cout << "biYpelsPerMeter = " << bih.biYpelsPerMeter << endl;
    cout << "biCrlUses = " << bih.biCrlUses << endl;
    cout << "biCrlImport = " << bih.biCrlImport << endl << endl;

}

void saveBFH(ofstream& ofs, BITMAPFILEHEADER& bfh) {
    ofs.write(reinterpret_cast<char*>(&bfh.bfType), 2);
    ofs.write(reinterpret_cast<char*>(&bfh.bfSize), 4);
    ofs.write(reinterpret_cast<char*>(&bfh.bfReserved1), 2);
    ofs.write(reinterpret_cast<char*>(&bfh.bfReserved2), 2);
    ofs.write(reinterpret_cast<char*>(&bfh.bfoffBits), 4);

}

void saveBIH(ofstream& ofs, BITMAPINFOHEADER& bih) {
    ofs.write(reinterpret_cast<char*>(&bih.biSize), 4);
    ofs.write(reinterpret_cast<char*>(&bih.biWidth), 4);
    ofs.write(reinterpret_cast<char*>(&bih.biHeight), 4);
    ofs.write(reinterpret_cast<char*>(&bih.biPlanes), 2);
    ofs.write(reinterpret_cast<char*>(&bih.biBitCount), 2);
    ofs.write(reinterpret_cast<char*>(&bih.biCompression), 4);
    ofs.write(reinterpret_cast<char*>(&bih.biSizeImage), 4);
    ofs.write(reinterpret_cast<char*>(&bih.biXpelsPerMeter), 4);
    ofs.write(reinterpret_cast<char*>(&bih.biYpelsPerMeter), 4);
    ofs.write(reinterpret_cast<char*>(&bih.biCrlUses), 4);
    ofs.write(reinterpret_cast<char*>(&bih.biCrlImport), 4);
}