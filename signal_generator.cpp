#include <iostream>
#include <cstring>
#include <cmath>
#include "GL/glut.h"

using namespace std;


int* current_signal = NULL;
int signal_length = 0;
char signal_title[100] = "";
bool is_manchester_encoding = false;

// LINE CODING FUNCTIONS:-

void encode_NRZ_L(char* data, int* output, int len) {
    for (int i = 0; i < len; i++) {
        output[i] = (data[i] == '1') ? 1 : -1;
    }
}

void encode_NRZ_I(char* data, int* output, int len) {
    int current_level = -1;
    for (int i = 0; i < len; i++) {
        if (data[i] == '1') current_level = -current_level;
        output[i] = current_level;
    }
}

void encode_MANCHESTER(char* data, int* output, int len) {
    for (int i = 0; i < len; i++) {
        if (data[i] == '0') {
            output[2*i] = 1;
            output[2*i + 1] = -1;
        } else {
            output[2*i] = -1;
            output[2*i + 1] = 1;
        }
    }
}

void encode_DIFF_MANCHESTER(char* data, int* output, int len) {
    int last_level = -1;
    for (int i = 0; i < len; i++) {
        if (data[i] == '0') {
            last_level = -last_level;
            output[2*i] = last_level;
            output[2*i + 1] = -last_level;
        } else {
            output[2*i] = last_level;
            output[2*i + 1] = -last_level;
        }
        last_level = -last_level;
    }
}

void encode_AMI(char* data, int* output, int len) {
    int last_polarity = 1;
    for (int i = 0; i < len; i++) {
        if (data[i] == '0') {
            output[i] = 0;
        } else {
            output[i] = last_polarity;
            last_polarity = -last_polarity;
        }
    }
}

// SCRAMBLING FUNCTIONS:-


void apply_B8ZS(int* signal, int len) {
    int last_polarity = 1;
    
    for (int i = 0; i <= len - 8; i++) {
        if (signal[i] != 0) {
            last_polarity = signal[i];
        }
        
        bool has_eight_zeros = true;
        for (int j = i; j < i + 8; j++) {
            if (signal[j] != 0) {
                has_eight_zeros = false;
                break;
            }
        }
        
        if (has_eight_zeros) {
            signal[i+3] = last_polarity;
            signal[i+4] = -last_polarity;
            signal[i+6] = last_polarity;
            signal[i+7] = -last_polarity;
            last_polarity = -last_polarity;
            i += 7;
        }
    }
}


void apply_HDB3(int* signal, int len) {
    int ones_count = 0;
    int last_polarity = 1;
    
    for (int i = 0; i <= len - 4; i++) {
        if (signal[i] != 0) {
            last_polarity = signal[i];
            ones_count++;
        }
        
        bool has_four_zeros = true;
        for (int j = i; j < i + 4; j++) {
            if (signal[j] != 0) {
                has_four_zeros = false;
                break;
            }
        }
        
        if (has_four_zeros) {
            if (ones_count % 2 == 0) {
                signal[i] = -last_polarity;
                signal[i+3] = last_polarity;
            } else {
                signal[i+3] = last_polarity;
            }
            
            last_polarity = -last_polarity;
            ones_count = 1;
            i += 3;
        }
    }
}

// MODULATION FUNCTIONS:-


int pcm_encode(double* analog_signal, int samples, char* output, int bits) {
    double max_val = analog_signal[0], min_val = analog_signal[0];
    for (int i = 1; i < samples; i++) {
        if (analog_signal[i] > max_val) max_val = analog_signal[i];
        if (analog_signal[i] < min_val) min_val = analog_signal[i];
    }
    
    int levels = pow(2, bits);
    double step = (max_val - min_val) / levels;
    int output_len = 0;
    
    for (int i = 0; i < samples; i++) {
        int quantized = (int)((analog_signal[i] - min_val) / step);
        if (quantized >= levels) quantized = levels - 1;
        
        for (int j = bits - 1; j >= 0; j--) {
            output[output_len++] = ((quantized >> j) & 1) ? '1' : '0';
        }
    }
    output[output_len] = '\0';
    return output_len;
}


int delta_modulation(double* analog_signal, int samples, char* output) {
    double prediction = 0.0, delta = 0.5;
    for (int i = 0; i < samples; i++) {
        if (analog_signal[i] > prediction) {
            output[i] = '1';
            prediction += delta;
        } else {
            output[i] = '0';
            prediction -= delta;
        }
    }
    output[samples] = '\0';
    return samples;
}

//ANALYTICAL FUNCTIONS:-


void find_longest_palindrome(char* data, int len) {
    int max_len = 1, start = 0;
    char* transformed = new char[2 * len + 3];
    int t_len = 0;
    transformed[t_len++] = '^';
    for (int i = 0; i < len; i++) {
        transformed[t_len++] = '|';
        transformed[t_len++] = data[i];
    }
    transformed[t_len++] = '|';
    transformed[t_len++] = '$';
    
    int* P = new int[t_len];
    for (int i = 0; i < t_len; i++) P[i] = 0;
    int center = 0, right = 0;
    
    for (int i = 1; i < t_len - 1; i++) {
        int mirror = 2 * center - i;
        if (i < right) P[i] = min(right - i, P[mirror]);
        while (transformed[i + P[i] + 1] == transformed[i - P[i] - 1]) P[i]++;
        if (i + P[i] > right) {
            center = i;
            right = i + P[i];
        }
        if (P[i] > max_len) {
            max_len = P[i];
            start = (i - P[i]) / 2;
        }
    }
    
    cout << "\nLongest Palindrome: ";
    for (int i = start; i < start + max_len; i++) cout << data[i];
    cout << " (Length: " << max_len << ")" << endl;
    
    delete[] transformed;
    delete[] P;
}




void find_longest_zero_sequence(int* signal, int len) {
    int max_count = 0, current_count = 0, max_start = 0, current_start = 0;
    for (int i = 0; i < len; i++) {
        if (signal[i] == 0) {
            if (current_count == 0) current_start = i;
            current_count++;
        } else {
            if (current_count > max_count) {
                max_count = current_count;
                max_start = current_start;
            }
            current_count = 0;
        }
    }
    if (current_count > max_count) max_count = current_count;
    if (max_count > 0) {
        cout << "Longest zero sequence: " << max_count 
             << " zeros starting at position " << max_start << endl;
    }
}

// OPENGL DISPLAY FUNCTIONS :-

void draw_text(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
    }
}

void draw_bold_text(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (current_signal == NULL || signal_length == 0) {
        glFlush();
        return;
    }
    
    float y_scale = 0.35;
    
    glColor3f(0.0, 0.0, 0.0);
    draw_bold_text(-0.95, 0.92, signal_title);
    
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
        glVertex2f(-0.9, 1.0 * y_scale);
        glVertex2f(0.9, 1.0 * y_scale);
        glVertex2f(-0.9, -1.0 * y_scale);
        glVertex2f(0.9, -1.0 * y_scale);
    glEnd();
    
    glColor3f(0.88, 0.88, 0.88);
    glLineWidth(0.5);
    glBegin(GL_LINES);
    float x_step = 1.8 / signal_length;
    for (int i = 0; i <= signal_length; i++) {
        float x = -0.9 + i * x_step;
        glVertex2f(x, -0.8);
        glVertex2f(x, 0.8);
    }
    glEnd();
    
    glColor3f(0.0, 0.0, 0.0);
    draw_text(-0.99, 1.0 * y_scale - 0.02, "+1");
    draw_text(-0.97, -0.03, "0");
    draw_text(-0.99, -1.0 * y_scale - 0.02, "-1");
    
    glColor3f(0.0, 0.0, 0.0);
    int label_step = 1;
    
    if (is_manchester_encoding) {
        if (signal_length > 40) label_step = 4;
        else if (signal_length > 20) label_step = 2;
        
        for (int i = 0; i <= signal_length; i += label_step) {
            char label[10];
            sprintf(label, "%.1f", i * 0.5);
            float x = -0.9 + i * x_step - 0.02;
            draw_text(x, -0.88, label);
        }
        
        glColor3f(0.5, 0.0, 0.5);
        draw_text(-0.3, -0.95, "Bit Position (mid-transitions at 0.5, 1.5, 2.5...)");
    } else {
        if (signal_length > 30) label_step = 5;
        else if (signal_length > 15) label_step = 2;
        
        for (int i = 0; i <= signal_length; i += label_step) {
            char label[10];
            sprintf(label, "%d", i);
            float x = -0.9 + i * x_step - 0.015;
            draw_text(x, -0.88, label);
        }
        
        glColor3f(0.0, 0.0, 0.0);
        draw_text(-0.08, -0.95, "Bit Position");
    }
    
    glColor3f(0.0, 0.0, 0.0);
    draw_text(-0.99, 0.85, "Voltage");
    
    glColor3f(0.0, 0.0, 1.0);
    glLineWidth(3.0);
    
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < signal_length; i++) {
        float x1 = -0.9 + i * x_step;
        float x2 = -0.9 + (i + 1) * x_step;
        float y = current_signal[i] * y_scale;
        
        glVertex2f(x1, y);
        glVertex2f(x2, y);
        
        if (i < signal_length - 1) {
            float next_y = current_signal[i + 1] * y_scale;
            glVertex2f(x2, y);
            glVertex2f(x2, next_y);
        }
    }
    glEnd();
    
    glColor3f(0.6, 0.0, 0.0);
    if (signal_length <= 25) {
        for (int i = 0; i < signal_length; i++) {
            float x = -0.9 + (i + 0.5) * x_step - 0.015;
            float y = current_signal[i] * y_scale;
            
            char val_label[5];
            if (current_signal[i] == 1) {
                sprintf(val_label, "+1");
                draw_text(x, y + 0.08, val_label);
            } else if (current_signal[i] == -1) {
                sprintf(val_label, "-1");
                draw_text(x, y - 0.12, val_label);
            } else {
                sprintf(val_label, "0");
                draw_text(x, y + 0.05, val_label);
            }
        }
    }
    
    glFlush();
}

void init_gl() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

void display_signal(int* signal, int len, const char* title, bool is_manchester) {
    current_signal = signal;
    signal_length = len;
    strcpy(signal_title, title);
    is_manchester_encoding = is_manchester;
    glutPostRedisplay();
}

//MAIN PROGRAM :-

int main(int argc, char** argv) {
    int input_type;
    cout << "----: Digital Signal Generator :----" << endl;
    cout << "1. Digital Input\n2. Analog Input (PCM/DM)\nChoice: ";
    cin >> input_type;
    
    char data[1000];
    int data_len = 0;
    
    if (input_type == 2) {
        cout << "\n1. PCM\n2. DM\nChoice: ";
        int mod_choice;
        cin >> mod_choice;
        
        int samples;
        cout << "Samples: ";
        cin >> samples;
        
        double* analog_signal = new double[samples];
        cout << "Values: ";
        for (int i = 0; i < samples; i++) cin >> analog_signal[i];
        
        if (mod_choice == 1) {
            int bits;
            cout << "Bits/sample: ";
            cin >> bits;
            data_len = pcm_encode(analog_signal, samples, data, bits);
            cout << "\nPCM: " << data << endl;
        } else {
            data_len = delta_modulation(analog_signal, samples, data);
            cout << "\nDM: " << data << endl;
        }
        delete[] analog_signal;
    } else {
        cout << "Binary data: ";
        cin >> data;
        data_len = strlen(data);
    }
    
    find_longest_palindrome(data, data_len);
    
    cout << "\n1. NRZ-L\n2. NRZ-I\n3. Manchester\n4. Diff Manchester\n5. AMI\nChoice: ";
    
    int encoding_choice;
    cin >> encoding_choice;
    
    int* encoded = NULL;
    int encoded_len = 0;
    char title[100];
    bool is_manchester = false;
    
    switch (encoding_choice) {
        case 1: 
            encoded_len = data_len; 
            encoded = new int[encoded_len];
            encode_NRZ_L(data, encoded, data_len); 
            strcpy(title, "NRZ-L Encoding"); 
            break;
            
        case 2: 
            encoded_len = data_len; 
            encoded = new int[encoded_len];
            encode_NRZ_I(data, encoded, data_len); 
            strcpy(title, "NRZ-I Encoding"); 
            break;
            
        case 3: 
            encoded_len = data_len * 2; 
            encoded = new int[encoded_len];
            encode_MANCHESTER(data, encoded, data_len); 
            strcpy(title, "Manchester Encoding"); 
            is_manchester = true;
            break;
            
        case 4: 
            encoded_len = data_len * 2; 
            encoded = new int[encoded_len];
            encode_DIFF_MANCHESTER(data, encoded, data_len); 
            strcpy(title, "Differential Manchester"); 
            is_manchester = true;
            break;
            
        case 5:
            encoded_len = data_len; 
            encoded = new int[encoded_len];
            encode_AMI(data, encoded, data_len); 
            strcpy(title, "AMI Encoding");
            
            cout << "\nScrambling? (1=Yes, 0=No): ";
            int sc; 
            cin >> sc;
            
            if (sc == 1) {
                cout << "1. B8ZS\n2. HDB3\nChoice: ";
                int st; 
                cin >> st;
                
                if (st == 1) { 
                    apply_B8ZS(encoded, encoded_len); 
                    strcpy(title, "AMI with B8ZS"); 
                } else { 
                    apply_HDB3(encoded, encoded_len); 
                    strcpy(title, "AMI with HDB3"); 
                }
                
                find_longest_zero_sequence(encoded, encoded_len);
            }
            break;
            
        default: 
            cout << "Invalid!" << endl; 
            return 1;
    }
    
    cout << "\nSignal: ";
    for (int i = 0; i < encoded_len; i++) cout << encoded[i] << " ";
    cout << endl;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1200, 700);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Digital Signal Visualization");
    
    init_gl();
    display_signal(encoded, encoded_len, title, is_manchester);
    glutDisplayFunc(display);
    
    cout << "\nOpenGL window opened. Close to exit..." << endl;
    glutMainLoop();
    
    delete[] encoded;
    return 0;
}
