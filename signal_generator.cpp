#include <iostream>
#include <cstring>
#include <cmath>
#include "GL/glut.h"
using namespace std;

// Global variables
int* signalData = NULL;
int signalSize = 0;
char windowTitle[100] = "";
bool isManchester = false;

// LINE CODING :-

void encodeNRZL(char* bits, int* encoded, int n) {
    for (int i = 0; i < n; i++) {
        encoded[i] = (bits[i] == '1') ? 1 : -1;
    }
}

void encodeNRZI(char* bits, int* encoded, int n) {
    int level = -1;
    for (int i = 0; i < n; i++) {
        if (bits[i] == '1') level = -level;
        encoded[i] = level;
    }
}

void encodeManchester(char* bits, int* encoded, int n) {
    for (int i = 0; i < n; i++) {
        if (bits[i] == '0') {
            encoded[2*i] = 1;
            encoded[2*i + 1] = -1;
        } else {
            encoded[2*i] = -1;
            encoded[2*i + 1] = 1;
        }
    }
}

void encodeDiffManchester(char* bits, int* encoded, int n) {
    int prevLevel = -1;
    for (int i = 0; i < n; i++) {
        if (bits[i] == '0') {
            prevLevel = -prevLevel;
            encoded[2*i] = prevLevel;
            encoded[2*i + 1] = -prevLevel;
        } else {
            encoded[2*i] = prevLevel;
            encoded[2*i + 1] = -prevLevel;
        }
        prevLevel = -prevLevel;
    }
}

void encodeAMI(char* bits, int* encoded, int n) {
    int lastPolarity = 1;
    for (int i = 0; i < n; i++) {
        if (bits[i] == '0') encoded[i] = 0;
        else {
            encoded[i] = lastPolarity;
            lastPolarity = -lastPolarity;
        }
    }
}

//SCRAMBLING:-

void scrambleB8ZS(int* signal, int n) {
    int lastPolarity = 1;
    for (int i = 0; i <= n - 8; i++) {
        if (signal[i] != 0) lastPolarity = signal[i];

        bool allZero = true;
        for (int j = i; j < i + 8; j++) {
            if (signal[j] != 0) { allZero = false; break; }
        }

        if (allZero) {
            signal[i+3] = lastPolarity;
            signal[i+4] = -lastPolarity;
            signal[i+6] = lastPolarity;
            signal[i+7] = -lastPolarity;
            lastPolarity = -lastPolarity;
            i += 7;
        }
    }
}

void scrambleHDB3(int* signal, int n) {
    int oneCount = 0;
    int lastPolarity = 1;

    for (int i = 0; i <= n - 4; i++) {
        if (signal[i] != 0) {
            lastPolarity = signal[i];
            oneCount++;
        }

        bool allZero = true;
        for (int j = i; j < i + 4; j++) {
            if (signal[j] != 0) { allZero = false; break; }
        }

        if (allZero) {
            if (oneCount % 2 == 0) {
                signal[i] = -lastPolarity;
                signal[i+3] = lastPolarity;
            } else {
                signal[i+3] = lastPolarity;
            }
            lastPolarity = -lastPolarity;
            oneCount = 1;
            i += 3;
        }
    }
}

//MODULATION:-

int encodePCM(double* analog, int samples, char* bits, int bitsPerSample) {
    double maxVal = analog[0], minVal = analog[0];
    for (int i = 1; i < samples; i++) {
        if (analog[i] > maxVal) maxVal = analog[i];
        if (analog[i] < minVal) minVal = analog[i];
    }

    int levels = pow(2, bitsPerSample);
    double step = (maxVal - minVal) / levels;
    int bitIndex = 0;

    for (int i = 0; i < samples; i++) {
        int quantized = (int)((analog[i] - minVal) / step);
        if (quantized >= levels) quantized = levels - 1;

        for (int j = bitsPerSample - 1; j >= 0; j--) {
            bits[bitIndex++] = ((quantized >> j) & 1) ? '1' : '0';
        }
    }
    bits[bitIndex] = '\0';
    return bitIndex;
}

int encodeDeltaMod(double* analog, int samples, char* bits) {
    double prediction = 0.0, delta = 0.5;
    for (int i = 0; i < samples; i++) {
        if (analog[i] > prediction) {
            bits[i] = '1';
            prediction += delta;
        } else {
            bits[i] = '0';
            prediction -= delta;
        }
    }
    bits[samples] = '\0';
    return samples;
}

//ANALYTICAL:-

void findLongestPalindrome(char* str, int n) {
    int maxLen = 1, start = 0;
    char* temp = new char[2 * n + 3];
    int tLen = 0;
    temp[tLen++] = '^';
    for (int i = 0; i < n; i++) {
        temp[tLen++] = '|';
        temp[tLen++] = str[i];
    }
    temp[tLen++] = '|';
    temp[tLen++] = '$';

    int* p = new int[tLen];
    for (int i = 0; i < tLen; i++) p[i] = 0;
    int center = 0, right = 0;

    for (int i = 1; i < tLen - 1; i++) {
        int mirror = 2 * center - i;
        if (i < right) p[i] = min(right - i, p[mirror]);
        while (temp[i + p[i] + 1] == temp[i - p[i] - 1]) p[i]++;
        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }
        if (p[i] > maxLen) {
            maxLen = p[i];
            start = (i - p[i]) / 2;
        }
    }

    cout << "\nLongest Palindrome: ";
    for (int i = start; i < start + maxLen; i++) cout << str[i];
    cout << " (Length: " << maxLen << ")" << endl;

    delete[] temp;
    delete[] p;
}

void findLongestZeroRun(int* signal, int n) {
    int maxCount = 0, count = 0, maxStart = 0, currStart = 0;
    for (int i = 0; i < n; i++) {
        if (signal[i] == 0) {
            if (count == 0) currStart = i;
            count++;
        } else {
            if (count > maxCount) {
                maxCount = count;
                maxStart = currStart;
            }
            count = 0;
        }
    }
    if (count > maxCount) maxCount = count;
    if (maxCount > 0)
        cout << "Longest zero sequence: " << maxCount
             << " zeros starting at position " << maxStart << endl;
}

//OPENGL:-

void drawText(float x, float y, const char* txt) {
    glRasterPos2f(x, y);
    for (int i = 0; txt[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, txt[i]);
}

void drawBoldText(float x, float y, const char* txt) {
    glRasterPos2f(x, y);
    for (int i = 0; txt[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, txt[i]);
}

void renderDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (signalData == NULL || signalSize == 0) {
        glFlush();
        return;
    }

    float yScale = 0.35;
    glColor3f(0.0, 0.0, 0.0);
    drawBoldText(-0.95, 0.92, windowTitle);

    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(2.5);
    glBegin(GL_LINES);
        glVertex2f(-0.9, 0.0);
        glVertex2f(0.9, 0.0);
        glVertex2f(-0.9, -0.8);
        glVertex2f(-0.9, 0.8);
    glEnd();

    glColor3f(0.7, 0.7, 0.7);
    glLineWidth(1.0);
    glBegin(GL_LINES);
        glVertex2f(-0.9, yScale);
        glVertex2f(0.9, yScale);
        glVertex2f(-0.9, -yScale);
        glVertex2f(0.9, -yScale);
    glEnd();

    float xStep = 1.8 / signalSize;
    glColor3f(0.88, 0.88, 0.88);
    glBegin(GL_LINES);
    for (int i = 0; i <= signalSize; i++) {
        float x = -0.9 + i * xStep;
        glVertex2f(x, -0.8);
        glVertex2f(x, 0.8);
    }
    glEnd();

    glColor3f(0.0, 0.0, 1.0);
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < signalSize; i++) {
        float x1 = -0.9 + i * xStep;
        float x2 = -0.9 + (i + 1) * xStep;
        float y = signalData[i] * yScale;
        glVertex2f(x1, y);
        glVertex2f(x2, y);
        if (i < signalSize - 1) {
            float nextY = signalData[i + 1] * yScale;
            glVertex2f(x2, y);
            glVertex2f(x2, nextY);
        }
    }
    glEnd();

    glFlush();
}

void initGL() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

void showSignal(int* data, int len, const char* title, bool manchester) {
    signalData = data;
    signalSize = len;
    strcpy(windowTitle, title);
    isManchester = manchester;
    glutPostRedisplay();
}

//MAIN:-

int main(int argc, char** argv) {
    int modeChoice;
    cout << "----: Digital Signal Generator :----" << endl;
    cout << "1. Digital Input\n2. Analog Input (PCM/DM)\nChoice: ";
    cin >> modeChoice;

    char bitStream[1000];
    int bitLen = 0;

    if (modeChoice == 2) {
        cout << "\n1. PCM\n2. DM\nChoice: ";
        int modType;
        cin >> modType;

        int nSamples;
        cout << "Samples: ";
        cin >> nSamples;

        double* analog = new double[nSamples];
        cout << "Values: ";
        for (int i = 0; i < nSamples; i++) cin >> analog[i];

        if (modType == 1) {
            int bits;
            cout << "Bits/sample: ";
            cin >> bits;
            bitLen = encodePCM(analog, nSamples, bitStream, bits);
            cout << "\nPCM: " << bitStream << endl;
        } else {
            bitLen = encodeDeltaMod(analog, nSamples, bitStream);
            cout << "\nDM: " << bitStream << endl;
        }
        delete[] analog;
    } else {
        cout << "Binary data: ";
        cin >> bitStream;
        bitLen = strlen(bitStream);
    }

    findLongestPalindrome(bitStream, bitLen);

    cout << "\n1. NRZ-L\n2. NRZ-I\n3. Manchester\n4. Diff Manchester\n5. AMI\nChoice: ";
    int encChoice;
    cin >> encChoice;

    int* encoded = NULL;
    int encLen = 0;
    char title[100];
    bool manchesterFlag = false;

    switch (encChoice) {
        case 1:
            encLen = bitLen;
            encoded = new int[encLen];
            encodeNRZL(bitStream, encoded, bitLen);
            strcpy(title, "NRZ-L Encoding");
            break;

        case 2:
            encLen = bitLen;
            encoded = new int[encLen];
            encodeNRZI(bitStream, encoded, bitLen);
            strcpy(title, "NRZ-I Encoding");
            break;

        case 3:
            encLen = bitLen * 2;
            encoded = new int[encLen];
            encodeManchester(bitStream, encoded, bitLen);
            strcpy(title, "Manchester Encoding");
            manchesterFlag = true;
            break;

        case 4:
            encLen = bitLen * 2;
            encoded = new int[encLen];
            encodeDiffManchester(bitStream, encoded, bitLen);
            strcpy(title, "Differential Manchester");
            manchesterFlag = true;
            break;

        case 5:
            encLen = bitLen;
            encoded = new int[encLen];
            encodeAMI(bitStream, encoded, bitLen);
            strcpy(title, "AMI Encoding");

            cout << "\nScrambling? (1=Yes, 0=No): ";
            int scrambleChoice;
            cin >> scrambleChoice;

            if (scrambleChoice == 1) {
                cout << "1. B8ZS\n2. HDB3\nChoice: ";
                int scrType;
                cin >> scrType;

                if (scrType == 1) {
                    scrambleB8ZS(encoded, encLen);
                    strcpy(title, "AMI with B8ZS");
                } else {
                    scrambleHDB3(encoded, encLen);
                    strcpy(title, "AMI with HDB3");
                }

                findLongestZeroRun(encoded, encLen);
            }
            break;

        default:
            cout << "Invalid!" << endl;
            return 1;
    }

    cout << "\nSignal: ";
    for (int i = 0; i < encLen; i++) cout << encoded[i] << " ";
    cout << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1200, 700);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Digital Signal Visualization");

    initGL();
    showSignal(encoded, encLen, title, manchesterFlag);
    glutDisplayFunc(renderDisplay);

    cout << "\nOpenGL window opened. Close to exit..." << endl;
    glutMainLoop();

    delete[] encoded;
    return 0;
}
