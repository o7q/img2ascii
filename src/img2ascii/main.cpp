#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <dirent.h>
using namespace std;

const string version = "v1.0.0";

main()
{
    system(("title img2ascii " + version).c_str());

    system("rmdir \"img2ascii\\working\" /s /q");

    system("mkdir \"img2ascii\\working\" 2> nul");
    system("mkdir \"img2ascii\\working\\img\" 2> nul");
    system("mkdir \"img2ascii\\working\\rgb\" 2> nul");
    system("mkdir \"output\" 2> nul");

    string path;
    getline(cin, path);

    string width;
    getline(cin, width);

    string height;
    getline(cin, height);

    cout << "\n";
    system(("ffmpeg.exe -i \"" + path + "\" -vf scale=" + width + ":" + height + " \"img2ascii\\working\\img\\img%d.png\"").c_str());

    ofstream widthFile;
    widthFile.open("img2ascii\\working\\width");
    widthFile << width;
    widthFile.close();

    ofstream heightFile;
    heightFile.open("img2ascii\\working\\height");
    heightFile << height;
    heightFile.close();

    cout << "\n";
    system("img2ascii\\img2rgb.exe");
    cout << "\n";

    int width_int = stoi(width);
    int height_int = stoi(height);
    int area = width_int * height_int;

    int imgIndex = 1;

    if (auto dir = opendir("img2ascii\\working\\rgb"))
    {
        while (auto f = readdir(dir))
        {
            if (!f->d_name || f->d_name[0] == '.') continue;

            ifstream imgRGB_path("img2ascii\\working\\rgb\\img_rgb" + to_string(imgIndex));
            string line;
            string imgRGB;
            while (getline(imgRGB_path, line))
            {
                imgRGB += line;
                imgRGB.push_back('\n');
            }

            stringstream rgbData(imgRGB);
            string pixelRead;
            vector<std::string> pixelRead_list;

            while (getline(rgbData, pixelRead, '|')) pixelRead_list.push_back(pixelRead);

            int rs[128000];
            int gs[128000];
            int bs[128000];

            int readRGB_index = 0;

            for (int i = 0; i < area; i++)
            {
                stringstream rgbData(pixelRead_list[i]);
                string RGBRead;
                vector<std::string> RGBRead_list;

                while (getline(rgbData, RGBRead, ',')) RGBRead_list.push_back(RGBRead);

                rs[readRGB_index] = stoi(RGBRead_list[0]);
                gs[readRGB_index] = stoi(RGBRead_list[1]);
                bs[readRGB_index] = stoi(RGBRead_list[2]);

                readRGB_index++;
            }
            
            string asciiChars[] = { ".", ",", "-", "_", "`", "'", "\"", "^", "*", ">", "~", "+", "?", "#", "$", "&", "%", "@" };
            int asciiQuantize = 15;
            int pixelAverage[128000];
            int widthIndex = 0;
            string asciiOut = "";

            for (int i = 0; i < readRGB_index; i++)
            {
                if (widthIndex == width_int)
                {
                    asciiOut += "\n";
                    widthIndex = 0;
                }
                widthIndex++;
                pixelAverage[i] = (rs[i] + gs[i] + bs[i]) / 3;
                asciiOut += asciiChars[pixelAverage[i] / asciiQuantize] + asciiChars[pixelAverage[i] / asciiQuantize];
            }

            cout << "Rasterizing img_rgb" + to_string(imgIndex) + " to ASCII\n";
            ofstream asciiFile;
            asciiFile.open("output\\img_ascii" + to_string(imgIndex) + ".txt");
            asciiFile << asciiOut;
            asciiFile.close();

            imgIndex++;
        }
        closedir(dir);
    }

    system("rmdir \"img2ascii\\working\" /s /q");

    cout << "\n";
    system("pause");
    return 0;
}