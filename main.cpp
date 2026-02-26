#include <windows.h>
#include <conio.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <keycodes.h>
using namespace std;

#define null NULL // I'm just used to it being lowercase
typedef void (event)();

HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);






//#
//## Logging/Console-Related Function Declarations
//#
#pragma region
void echo(char* str) {
    std::cout << str << std::endl;
}

void _echo(char* str) {
    std::cout << str;
}

void moveConsoleCursor(int y, int x)
{
    printf("\033[%d;%dH", y, x);
}

void moveConsoleCursor()
{
    std::cout << "\033[1;1H"; // \x1B | \033
}




void clear()
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(handle, &info);

    int x = info.dwSize.X;
    int height = info.srWindow.Bottom + 1;

    // Allocate some space to write the string
    char* str = (char*) malloc(x + 1);

    // Build the string
    for(int i = 0; i < x; ++i)
    {
        *(str + i) = (int8_t) 0x20;
    }
    *(str + x) = 0x00;

    // Print it to each row in the output window
    for (int i = 0; i < height; i++)
    {
        moveConsoleCursor(i, 1);
        echo(str);
    }

    free(str);
}




/// @brief Enable ANSI escape sequence processing from both user input and program output
void enableEscapeSequences()
{
    if (handle == INVALID_HANDLE_VALUE)
    {
        printf("ERROR: Invalid handle value returned from GetStdHandle(STD_OUTPUT_HANDLE) (%d)\n", STD_OUTPUT_HANDLE);
        exit(1);
    }

    // Get the existing console flags
    DWORD baseMode = 0;
    if (!GetConsoleMode(handle, &baseMode)) {
        GetLastError();
    }

    // Enable escape sequence processing for carrot X/Y crap
    SetConsoleMode(handle, baseMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
}
#pragma endregion












//#
//## Main Functionality Declarations 
//#

/// @brief Run a loop of printing the key codes for the detected key, as well as whether it had an initial null byte
void inputTest()
{
    int ch;
    char* ext = "";
    bool isSpecialChar;
    do {
        ch = _getch();
        if (isSpecialChar = ch == 224)
            ch = _getch();


        // Lazily clear the screen
        for(int i = 0; i < 15 && (printf("\033[1;1H") + 1); i++)
        {
            std::cout << "                                                                                                                        ";
        }
        std::cout << "\033[1;1H";
        

        if (isSpecialChar)
        {
            ext = " (spc)";
        }
        else ext = "";

        printf ("Code: %d%s\n\x1B[1;1H", ch, ext);

    } while( 1 );
}






void runMenuLoop (char* outputLoop[], size_t outputArrayLen, event* eventHandlers[], size_t handlerArrayLen, char* title, int StartingSelectionIndex)
{
    int32_t SelectionIndex;

    if (StartingSelectionIndex != -1)
    {
        SelectionIndex = StartingSelectionIndex;
    }
    else {
        SelectionIndex = 0;
    }


    int reply;
    bool isSpecialKey;
    char* prepend;
    
    while (true)
    {
        echo(title);
        
        for (int i = 0; i < outputArrayLen; i++)
        {
            if (SelectionIndex == i)
            {
                prepend = "> ";
            }
            else {
                prepend = "  ";
            }

            printf ("%s%s\n", prepend, outputLoop[i]);
        }
        echo("\n"); // Add some spacing so printed text isn't immediately overwritten by the menu




        read:
        isSpecialKey = (reply = _getch()) == 224;
        if (isSpecialKey)
        {
            // Run a second time to get the key code for special keys
            reply = _getch();
        }


        switch (reply) // Watch out for overlapping key codes if expanding
        {
            case ASCIISpecialKeyCodes::UP:
                SelectionIndex--;

                if (SelectionIndex < 0)
                {
                    SelectionIndex = outputArrayLen - 1;
                }

                break;

            case ASCIISpecialKeyCodes::DOWN:
                SelectionIndex++;

                if (SelectionIndex >= outputArrayLen)
                {
                    SelectionIndex = 0;
                }
                break;

            case ASCIISpecialKeyCodes::LEFT:
                break;
                
            case ASCIISpecialKeyCodes::RIGHT:
                clear();
                break;



                
            case ASCIIKeyCodes::ENTER:
                printf("%s\n", typeid(*eventHandlers[SelectionIndex]).name());

                if (SelectionIndex < handlerArrayLen && eventHandlers[SelectionIndex] != NULL)
                {
                    eventHandlers[SelectionIndex]();
                }
                else {
                    printf("\nNull function ptr for item #%d.", SelectionIndex);
                }
                break;




            // Off we fuck
            case ASCIIKeyCodes::ESCAPE:
                return;

            default:
                goto read;
        }

        moveConsoleCursor();
    }
}






//#
//## Event Handlers
//#

// Tst 1
static void test1()
{
    echo("test func  ");
}

// Tst 2
static void test2()
{
    echo("test func 2");
}

// Tst 3
static void test3()
{
    echo("test func 3");
}

// Tst 4
static void test4()
{
    echo("test func 4");
}


static void runInputTest()
{
    inputTest();
}










//#
//## Entry Point & Main
//#

void entry()
{
    std::cout << "\033[?25l"; // Disable the console cursor
    int8_t SelectionIndex = 0;
    
    char* outputLoop[] =
    {
        "Option 1",
        "Option 2",
        "Option 3",
        "Option 4",
        "Null Option",
        "OOB Option"
    };
    
    event* eventHandlers[] =
    {
        &test1,
        &test2,
        &test3,
        &test4,
        null
    };
    
    char* title = "Title Card";

    runMenuLoop(outputLoop, sizeof(outputLoop) / 8, eventHandlers, sizeof(eventHandlers) / 8, title, 0);
}




// Process Entry Point 
int main()
{   
    enableEscapeSequences();

    return atexit(entry);
}