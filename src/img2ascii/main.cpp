#include <iostream>
#include <sys/stat.h>
#include <regex>
#include <fstream>
#include <chrono>
#include <dirent.h>
using namespace std;
using namespace std::chrono;

const string version = "v2.1.1";

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
    int width = (size.find('x') != string::npos) ? ((!sizeRead_list[0].empty() && sizeRead_list[0].find_first_not_of("0123456789")) ? stoi(sizeRead_list[0]) : 100) : 100;
    int height = (size.find('x') != string::npos) ? ((!sizeRead_list[1].empty() && sizeRead_list[0].find_first_not_of("0123456789")) ? stoi(sizeRead_list[1]) : 50) : 50;

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

    system(("title img2ascii " + version + "   [CONVERTING]").c_str());

    // start execution stopwatch
    auto stopwatch_start = high_resolution_clock::now();

    system(("mkdir \"" + name + "\" 2> nul").c_str());
    system(("mkdir \"" + name + "\\_raw\" 2> nul").c_str());
    system(("mkdir \"" + name + "\\frames\" 2> nul").c_str());
    system(("mkdir \"" + name + "\\stats\" 2> nul").c_str());

    cout << "\nCONVERTING TO RAW SEQUENCE\n";
    system(("img2ascii\\ffmpeg.exe -loglevel verbose -i \"" + path_fix + "\" -vf scale=" + to_string(width) + ":" + to_string(height) + fps_controller + " -q:v " + to_string(jpegCompression) + " \"" + name + "\\_raw\\frame.raw.%d.jpg\"").c_str());

    cout << "\nCONVERTING TO ASCII SEQUENCE\n";
    int frameIndex = 1; 
    if (auto dir = opendir((name + "\\_raw").c_str()))
    {
        while (auto f = readdir(dir))
        {
            if (!f->d_name || f->d_name[0] == '.') continue;
    
            // load raw frame
            int img_width, img_height;
            vector<unsigned char> img;
            bool success = load_frame_raw(img, name + "\\_raw\\frame.raw." + to_string(frameIndex) + ".jpg", img_width, img_height);
            if (!success)
            {
                cout << " Error loading frame!\n";
                system("pause");
                _Exit(1);
            }
    
            int x_pos = 0;
            int y_pos = 0;
            const size_t RGB = 3;
            string asciiImage = "";

            // convert jpeg to ascii
            for (int i = 0; i < area; i++)
            {
                // calculate rgb value for each pixel
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
            printf((" Converting [frame.raw." + to_string(frameIndex) + ".jpg] to ASCII\n").c_str());
            ofstream asciiFile;
            asciiFile.open(name + "\\frames\\frame.ascii." + to_string(frameIndex) + ".txt");
            asciiFile << asciiImage;
            asciiFile.close();

            frameIndex++; 
        }
        closedir(dir);
    }

    // stop execution stopwatch
    auto stopwatch_stop = high_resolution_clock::now();

    system(("title img2ascii " + version + "   [DONE]").c_str());

    // END

    // process stats and wrap project
    string stats[] =
    {
        to_string((duration_cast<seconds>(stopwatch_stop - stopwatch_start)).count()), "stat_time",
        to_string(frameIndex - 1), "stat_frames",
        to_string(width), "stat_resolution_width",
        to_string(height), "stat_resolution_height",
        fps == "!" ? "Original" : fps, "stat_framerate",
        chars, "stat_characters",
        !asciiQuantize_str.empty() && asciiQuantize_str.find_first_not_of("0123456789") ? asciiQuantize_str : "n/a", "stat_compression_user",
        to_string(asciiQuantize), "stat_compression_quantize",
        to_string(chars.length()), "stat_compression_factor"
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

    string formattedStats = " - Conversion Time: " + stats[0] + " seconds"
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
    cout << "\n Conversion Finished!\n" + formattedStats + "\n\n ";
    system("pause");

    return 0;
}