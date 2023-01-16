#include <iostream>
#include <sys/stat.h>
#include <regex>
#include <fstream>
#include <chrono>
#include <dirent.h>
#include <windows.h>
using namespace std;
using namespace std::chrono;

string formNum(int rawNum);

const string version = "v2.2.0";
bool persistent = true;

// code forked from: https://cplusplus.com/forum/beginner/267364
extern "C"
{
    #define STB_IMAGE_IMPLEMENTATION
    #include "includes/stb_image/stb_image.h"
}
bool load_frame_raw(vector<unsigned char>& image, const string& filename, int& x, int&y)
{
    int n;
    unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 3);
    if (data != nullptr) image = vector<unsigned char>(data, data + x * y * 3);
    stbi_image_free(data);
    return (data != nullptr);
}
//

int main()
{
    while (persistent)
    {
        struct stat scan_ffmpeg;
        if (stat("img2ascii\\ffmpeg.exe", &scan_ffmpeg) != 0)
        {
            cout << "\n FFmpeg was not found! Please ensure that it's in the directory \"img2ascii\\ffmpeg.exe\"\n ";
            system("pause");
            _Exit(1);
        }

        system(("title img2ascii " + version).c_str());
        cout << "  _            ___             _ _ \n"
                " (_)_ __  __ _|_  )__ _ ___ __(_|_)\n"
                " | | '  \\/ _` |/ // _` (_-</ _| | |\n"
                " |_|_|_|_\\__, /___\\__,_/__/\\__|_|_| " + version + "\n"
                "         |___/                      by o7q\n\n";

        cout << " PROJECT NAME\n -> ";
        string name;
        getline(cin, name);

        cout << "\n MEDIA INPUT PATH (an image or video)\n -> ";
        string path;
        getline(cin, path);
        string path_fix = regex_replace(path, regex("\\\""), "");
    
        cout << "\n FRAMESIZE (WIDTHxHEIGHT, example: 80x40)\n -> ";
        string size;
        getline(cin, size);

        cout << "\n FRAMERATE (type ! to use the original fps or if it is an image)\n -> ";
        string fps;
        getline(cin, fps);

        if(!(!fps.empty() && fps.find_first_not_of("0123456789")) && fps != "!") fps = "15";
        string fps_controller = fps == "!" ? "" : " -r " + fps;

        // read size parameters and split them into width*height
        stringstream sizeData(size);
        string sizeRead;
        vector<string> sizeRead_list;
        while (getline(sizeData, sizeRead, 'x')) sizeRead_list.push_back(sizeRead);

        // process width/height values
        int width = (size.find('x') != string::npos) ? ((!sizeRead_list[0].empty() && sizeRead_list[0].find_first_not_of("0123456789")) ? stoi(sizeRead_list[0]) : 80) : 80;
        int height = (size.find('x') != string::npos) ? ((!sizeRead_list[1].empty() && sizeRead_list[1].find_first_not_of("0123456789")) ? stoi(sizeRead_list[1]) : 40) : 40;

        int area = width * height;

        cout << "\n ASCII CHARACTERS (choose a number or enter your own):\n"
                "  [1] ascii (standard ascii special characters only)\n"
                "  [2] upper (uppercase letters only)\n"
                "  [3] lower (lowercase letters only)\n"
                "  [4] both (upper and lowercase letters only)\n"
                "  [5] num (numbers only)\n"
                "  [6] all (all characters, very chaotic)\n"
                " -> ";
        string chars;
        getline(cin, chars);

        int chars_length = chars.length();

        // ascii colorspace based on: https://stackoverflow.com/a/74186686
        //  `.-'":_,^=;><+!rc*\/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@
        string chars_index = " `.-'\":_,^=;><+!rc*\\/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";

        if (!chars.empty() && chars.find_first_not_of("0123456789"))
        {
            switch(stoi(chars))
            {
                case 1: chars = " `.-'\":_,^=;><+!*\\/?)(|{}[]#$%&@"; break;
                case 2: chars = "LTJFCIZYESPVOGUAKXHRDBMNWQ"; break;
                case 3: chars = "ltjfcizyespvoguakxhrdbmnwq"; break;
                case 4: chars = "rczsLTvJFiCfItluneoZYxjyaESwqkPhdVpOGbUAKXHmRDBgMNWQ"; break;
                case 5: chars = "7315269480"; break;
                case 6: chars = chars_index; break;
            }
        }
        else
        {
            // sort custom characters from darkest to brightest

            // create indexes
            vector<char> char_index_vector(chars_index.begin(), chars_index.end());
            vector<char> chars_vector(chars.begin(), chars.end());
            string chars_temp;
            // sort characters with jamesort™
            for (size_t i = 0; i < chars_index.length(); i++)
                for(int j = 0; j < chars_length; j++)
                    if (chars_vector[j] == chars_index[i])
                        chars_temp += chars_vector[j];
            // remove duplicate characters
            chars = "";
            vector<char> chars_sorted_vector(chars_temp.begin(), chars_temp.end());
            for (int i = 0; i < chars_length; i++)
                if (chars_sorted_vector[i] != chars_sorted_vector[i + 1])
                    chars += chars_sorted_vector[i];
        }

        chars_length = chars.length();

        // calculate ascii colorspace
        // split characters string into a char array
        char chars_split[chars_length];
        strcpy(chars_split, chars.c_str());
        // convert the char array into a string array (to fix issues with encoding on file output)
        string asciiChars[chars_length] = {};
        for (int i = 0; i < chars_length; i++) asciiChars[i] = chars_split[i];
        int asciiQuantize = (255 / chars_length) + 1;

        cout << "\n ASCII COMPRESSION (Between 0 - 255, A value of 0 would utilize " + to_string((255 / (asciiQuantize + 0)) + 1) + " characters. A value of 25 would utilize " + to_string((255 / (asciiQuantize + 25)) + 1) + " characters.)\n -> ";
        string asciiQuantize_str;
        getline(cin, asciiQuantize_str);

        asciiQuantize = !asciiQuantize_str.empty() && asciiQuantize_str.find_first_not_of("0123456789") ? asciiQuantize + stoi(asciiQuantize_str) : asciiQuantize + 25;

        cout << "\n JPEG COMPRESSION (Between 1 - 31, A lower value means higher quality and less artifacts.)\n -> ";
        string jpegCompression_str;
        getline(cin, jpegCompression_str);

        int jpegCompression = !jpegCompression_str.empty() && jpegCompression_str.find_first_not_of("0123456789") ? stoi(jpegCompression_str) : 1;

        // START

        // start execution stopwatch
        auto stopwatch_start = high_resolution_clock::now();
        system(("title img2ascii " + version + "   [CONVERTING]").c_str());
        system("cls");

        system(("mkdir \"" + name + "\" 2> nul").c_str());
        system(("mkdir \"" + name + "\\_raw\" 2> nul").c_str());
        system(("mkdir \"" + name + "\\frames\" 2> nul").c_str());
        system(("mkdir \"" + name + "\\stats\" 2> nul").c_str());

        cout << "\n [1/2] CREATING JPEG SEQUENCE\n";
        system(("img2ascii\\ffmpeg.exe -loglevel quiet -i \"" + path_fix + "\" -vf scale=" + to_string(width) + ":" + to_string(height) + fps_controller + " -q:v " + to_string(jpegCompression) + " \"" + name + "\\_raw\\frame.raw.%d.jpg\"").c_str());

        system("cls");

        int rawFrameCount = 0;
        if (auto dir = opendir((name + "\\_raw").c_str()))
        {
            while (auto f = readdir(dir))
            {
                if (!f->d_name || f->d_name[0] == '.') continue;

                rawFrameCount++;
            }
            closedir(dir);
        }

        int frameIndex = 1; 
        if (auto dir = opendir((name + "\\_raw").c_str()))
        {
            while (auto f = readdir(dir))
            {
                if (!f->d_name || f->d_name[0] == '.') continue;
    
                // display conversion stats
                HANDLE oHandle;
                COORD pos;
                oHandle = GetStdHandle(STD_OUTPUT_HANDLE);
                pos.X = 0;
                pos.Y = 0;
                SetConsoleCursorPosition(oHandle, pos);
                printf(("\n [2/2] CONVERTING TO ASCII SEQUENCE\n - Converting [" + to_string(frameIndex) + "/" + to_string(rawFrameCount) + "] to ASCII\n").c_str());

                // load raw frame
                int img_width, img_height;
                vector<unsigned char> img;
                bool success = load_frame_raw(img, name + "\\_raw\\frame.raw." + to_string(frameIndex) + ".jpg", img_width, img_height);
                if (!success)
                {
                    cout << " Error loading frame!\n";
                    system("pause");
                }
    
                int x_pos = 0;
                int y_pos = 0;
                const size_t RGB = 3;
                string asciiImage = "";

                // convert jpeg to ascii
                for (int i = 0; i < area; i++)
                {
                    // read image line by line
                    if (x_pos == width)
                    {
                        x_pos = 0;
                        y_pos++;
                        asciiImage += "\n";
                    }

                    // calculate ascii pixel from rgb value
                    int pixelAverage = 0;
                    size_t pixelIndex = RGB * (y_pos * img_width + x_pos);
                    for (int j = 0; j < 3; j++) pixelAverage += static_cast<int>(img[pixelIndex + j]);
                    for (int j = 0; j < 2; j++) asciiImage += asciiChars[(pixelAverage / 3) / asciiQuantize];

                    x_pos++;
                }

                // write ascii frame
                ofstream asciiFile;
                asciiFile.open(name + "\\frames\\frame.ascii." + to_string(frameIndex) + ".txt");
                asciiFile << asciiImage;
                asciiFile.close();

                frameIndex++; 
            }
            closedir(dir);
        }

        system("cls");
        system(("title img2ascii " + version + "   [DONE]").c_str());
        // stop execution stopwatch
        auto stopwatch_stop = high_resolution_clock::now();

        // END

        // process stats and wrap project
        string stats[] =
        {
            to_string((duration_cast<milliseconds>(stopwatch_stop - stopwatch_start)).count()), "stat_time",
            formNum(frameIndex - 1), "stat_frames",
            formNum(width), "stat_resolution_width",
            formNum(height), "stat_resolution_height",
            fps == "!" ? "n/a" : formNum(stoi(fps)), "stat_framerate",
            chars, "stat_characters",
            !asciiQuantize_str.empty() && asciiQuantize_str.find_first_not_of("0123456789") ? asciiQuantize_str : "n/a", "stat_compression_user",
            to_string(asciiQuantize), "stat_compression_quantize",
            to_string(chars.length()), "stat_compression_factor",
            to_string(jpegCompression), "stat_compression_jpeg"
        };
        int statsIndex = 0;
        while (size_t(statsIndex) < sizeof(stats) / sizeof(string))
        {
            ofstream writeStats;
            writeStats.open(name + "\\stats\\" + stats[statsIndex + 1]);
            writeStats << stats[statsIndex];
            writeStats.close();
            statsIndex += 2;
        }

        // compute time
        int min = stoi(stats[0]) / 60000;
        int sec = (stoi(stats[0]) - min * 60000) / 1000;
        int milisec = stoi(stats[0]) - min * 60000 - sec * 1000;
        string min_str = min >= 10 ? to_string(min) : "0" + to_string(min);
        string sec_str = sec >= 10 ? to_string(sec) : "0" + to_string(sec);
        string milisec_str = to_string(milisec);
        if (milisec < 10) milisec_str = "000" + milisec_str;
        if (milisec < 100 && milisec >= 10) milisec_str = "00" + milisec_str;
        if (milisec < 1000 && milisec >= 100 ) milisec_str = "0" + milisec_str;

        string formattedStats = " - Conversion Time: " + min_str + ":" + sec_str + "." + milisec_str +
                                "\n - Total Frames: " + stats[2] +
                                "\n - Resolution: " + stats[4] + "x" + stats[6] +
                                "\n - Framerate: " + stats[8] +
                                "\n - Characters: " + stats[10] +
                                "\n - Compression: " + stats[12] + " (quantize: " + stats[14] + ", factor: " + stats[16] + ")";

        ofstream lockFile;
        lockFile.open(name + "\\lock");
        lockFile.close();

        ofstream readmeFile;
        readmeFile.open(name + "\\readme.txt");
        readmeFile << "Hello! Thank you for using my program to make ASCII stuff!\n\n"
                      "Info for this project:\n" +
                      formattedStats +
                      "\n\nWhat each file/folder does:\n"
                      " - _raw: Stores the temporary raw conversion files (you can delete this if you want)\n"
                      " - frames: This is where the ASCII frames are stored, you can play them with \"asciiPlayer.exe\"\n"
                      " - stats: This stores conversion and config information for the project that is used by \"asciiPlayer.exe\"\n"
                      " - lock: Verifies that this project is genuine and will function properly with \"asciiPlayer.exe\"\n"
                      " - readme.txt: This file!";
        readmeFile.close();

        // display stats
        cout << "\n Conversion Finished!\n" + formattedStats + "\n\n CONVERT ANOTHER? (y = yes, anything else = no)\n -> ";
        string prompt;
        getline(cin, prompt);

        system("cls");
        if (prompt != "Y" && prompt != "y") persistent = false;
    }

    return 0;
}

// format number function
string formNum(int rawNum)
{
    string numForm;
    string num = to_string(rawNum);
  
    int count = 0;
  
    for (int i = num.size() - 1; i >= 0; i--)
    {
        count++;
        numForm.push_back(num[i]);
  
        if (count == 3)
        {
            numForm.push_back(',');
            count = 0;
        }
    }
  
    reverse(numForm.begin(), numForm.end());
  
    if (numForm.size() % 4 == 0) numForm.erase(numForm.begin());
  
    return numForm;
}