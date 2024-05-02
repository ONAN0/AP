//Jozef Matula

//**gcc blok_3.c -o blok_3 -g -lwsock32 -lWs2_32

#define _WIN32_WINNT 0x0501                                          //! bez tohto gcc vypisuje "implicit declaration of function getaddrinfo" error, 
                                                                     // v knižnici "ws2tcpip" sa nachádza v podmienke (riadok 297)

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "blok_3.h"

#define DEFAULT_BUFLEN 4096                                          // makro, kde definujeme velkost prijimacieho buffera

int recvbuflen = DEFAULT_BUFLEN;
char recvbuf[DEFAULT_BUFLEN];

FILE *fw;

int main()
{
    system("cls");

    HANDLE hconsole = GetStdHandle(STD_OUTPUT_HANDLE);               // "handler" na koznolu
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    
    GetConsoleScreenBufferInfo(hconsole, &consoleInfo);              /* uloží "default" nastavenie farby textu 
                                                                        a pozadia aby ho na konci programu 
                                                                        nastavilo do pôvodného stavu */

    SetConsoleOutputCP(CP_UTF8);                                     // nastavenie formatovania konzoly na UTF8
    SetConsoleTextAttribute(hconsole, 2);                            // zelena farba CLI

    if ((fw = fopen("log_chat.txt", "w")) == NULL)
    {
        SetConsoleTextAttribute(hconsole, 4);
        printf("Nepodarilo sa otvorit subor na zapis: \"log_chat.txt\" ");
        SetConsoleTextAttribute(hconsole, 2);
        return 1;
    }

    // uvodne nastavovacky =================================================================================

    WSADATA wsaData;                                                 //struktura WSADATA pre pracu s Winsock
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);                  //zakladna inicializacia
    if (iResult != 0)                                                //kontrola, ci nestala chyba
    {
        SetConsoleTextAttribute(hconsole, 4);
        printf("WSAStartup failed: % d\n", iResult);
        SetConsoleTextAttribute(hconsole, 2);
        fprintf(fw, "WSAStartup failed: % d\n", iResult);
        return 1;
    }

    struct addrinfo *result = NULL, *ptr = NULL;                     //struktura pre pracu s adresami
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;                                 //pracujeme s protokolom TCP/IP

    // Resolve the server address and port
    iResult = getaddrinfo("147.175.115.34", "777", &hints, &result);

    if (iResult != 0)                                                //kontrola, ci nenastala chyba
    {
        SetConsoleTextAttribute(hconsole, 4);
        printf("getaddrinfo failed: % d\n", iResult);
        SetConsoleTextAttribute(hconsole, 2);

        fprintf(fw, "getaddrinfo failed: % d\n", iResult);
        WSACleanup();
        return 1;
    }
    else
    {
        printf("getaddrinfo did not fail...OK\n");
        fprintf(fw, "getaddrinfo did not fail...OK\n");
    }

    //======================================================================================================

    // vytvorenie socketu a pripojenie sa ==================================================================
    SOCKET ConnectSocket = INVALID_SOCKET;

    // Attempt to connect to the first address returned by the call to getaddrinfo
    ptr = result;

    // Create a SOCKET for connecting to server => pokus o vytvorenie socketu ==============================
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET)                             //kontrola, ci nenastala chyba
    {
        SetConsoleTextAttribute(hconsole, 4);
        printf("Error at socket: %ld\n", WSAGetLastError());
        SetConsoleTextAttribute(hconsole, 2);

        fprintf(fw, "Error at socket: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    else
    {
        printf("Error at socket DID NOT occur...OK\n");
        fprintf(fw, "Error at socket DID NOT occur...OK\n");
    }

    //======================================================================================================

    // Connect to server. => pokus o pripojenie sa na server ===============================================

    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)                                     //kontrola, ci nenastala chyba
    {
        SetConsoleTextAttribute(hconsole, 4);
        printf("Not connected to server...\n");
        SetConsoleTextAttribute(hconsole, 2);
        fprintf(fw, "Not connected to server...\n");
    }
    else
    {
        printf("Connected to server!\n");
        fprintf(fw, "Connected to server!\n");
    }

    if (iResult == SOCKET_ERROR)                                     //osetrenie chyboveho stavu
    {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        WSACleanup();
        return 1;
    }

    Sleep(100);

    //======================================================================================================

    char sendbuf[4096];                                      /* treba zavolit vhodnu velkost, napr. 4096
                                                                buffer (v zasade retazec), kam sa budu
                                                                ukladat data, ktore chcete posielat */

    char messager [17][18] = {"Hello ?","120538","OK","8484848","753422","120538","333222333","123","Statue of Liberty","40","-74","S.O.L.","PRIMENUMBER"," ","Trinity","polyadicke","half-duplex"};

    for (int i = 0; i < 17; i++)
    {
        for (int j = 0; j < (int)strlen(messager[i]); j++)
        {
            strcpy(&sendbuf[j],&messager[i][j]);
        }

        if (i == 5)
        {
            int sum = 0;

            for (int k = 0; k < 5; k++)
            {
                sum += ((sendbuf[k]) - 48);
            }
        
            sendbuf[0] = (sum % ((sendbuf[4]) - 48)) + 48;
            sendbuf[1] = '\0';
            
        }
        
        if (i == 13)
        {
            int place = 0;
            int size = strlen(recvbuf);
    
            for (int num = 0; num < size; num++)
            {
                if (isPrimeNumber(num + 1))
                {
                    sendbuf[place] = recvbuf[num];
                    place++;
                }
            }
            sendbuf[place] = '\0';
        }
        
        SetConsoleTextAttribute(hconsole, 1);
        printf("Messager: ");
        printf("%s\n", sendbuf);
        SetConsoleTextAttribute(hconsole, 2);
        fprintf(fw,"Messager: ");
        fprintf(fw, "%s\n", sendbuf);

        // posielanie ======================================================================================
    
        iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed: %d\n", WSAGetLastError());
            fprintf(fw, "send failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        SetConsoleTextAttribute(hconsole, 6);
        printf("\nBytes Sent: %ld\n", iResult);                          //vypisanie poctu odoslanych dat
        SetConsoleTextAttribute(hconsole, 2);
        fprintf(fw, "\nBytes Sent: %ld\n", iResult);
        Sleep(100);

        //==================================================================================================

        // prijatie ========================================================================================

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);           //funkcia na príjimanie
        verifyData(iResult);
        printRecvbuf();
        Sleep(100);

        //==================================================================================================

        // dešífrovanie 8. správy ==========================================================================

        if (i == 7)
        {
            for (int l = 0; l < 150; l++)
            {
                recvbuf[l] = 55 ^ (int)recvbuf[l];
            }
            
            recvbuf[150] = '\0';
            printRecvbuf();
            Sleep(100);
        }
        

        //==================================================================================================

    }

    //zavretie socketu =================================================================================

    closesocket(ConnectSocket);
    WSACleanup();
    printf("Connection Closed\n");
    fprintf(fw, "Connection Closed\n");
    SetConsoleTextAttribute(hconsole, consoleInfo.wAttributes);

    fclose(fw);

    scanf("?");

    return 0;
}

//======================================================================================================
//======================================================================================================


// nastavenie kurzora na ľavú stranu console ===========================================================

void setCursorLeft()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    COORD coord;
    coord.X = 0;
    coord.Y = csbi.dwCursorPosition.Y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

//======================================================================================================

// nastavenie kurzora na pravú stranu console ==========================================================

int setCursorRight()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    COORD coord;
    coord.X = (csbi.srWindow.Right - csbi.srWindow.Left + 1) / 2;
    coord.Y = csbi.dwCursorPosition.Y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    
    return coord.X;
}

//======================================================================================================

void verifyData(int Result)
{
    HANDLE hconsole;
    hconsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (Result > 0)
    {
        SetConsoleTextAttribute(hconsole, 6);
        printf("Bytes received: %d\n", Result);                             // prisli validne data, vypis poctu
        SetConsoleTextAttribute(hconsole, 2);
        fprintf(fw, "Bytes received: %d\n", Result);
    }
    else if (Result == 0)
    {
        printf("Connection closed\n");                                      // v tomto pripade server ukoncil komunikaciu
        fprintf(fw, "Connection closed\n");
    }
    else
    {
        printf("recv failed with error: %d\n", WSAGetLastError());          // ina chyba
        fprintf(fw, "recv failed with error: %d\n", WSAGetLastError());
    }
}

//======================================================================================================

// overenie prvočísla
int isPrimeNumber(int i)
{
    int flag = 0;
    for (int j = 2; j <= i / 2; ++j)
    {
        // podmienka pre ne-prvočísla
        if (i % j == 0)
        {
            flag = 1;
            break;
        }
    }

    if (flag == 0)
    {
        if (i == 1)
        {
            return 0;
        }
        else
        {
            return i;
        }
    }
}

//======================================================================================================

// formatted output
void printRecvbuf()
{
    fprintf(fw, "\n");
    int row = 0;
    int center = setCursorRight();
    center -= 10;
    int newline[20];

    // kontrola kde by sa mal nachádzať nový riadok
    for (int i = 0; i < (int)strlen(recvbuf); i++)
    {
        if (i > (center + (row * center - 1)))
        {
            if (recvbuf[i] == ' ')
            {
                newline[row] = i;
                row++;
            }
            else
            {
                while (recvbuf[i] != ' ')
                {
                    i--;
                }
                newline[row] = i;
                row++;
            }
        }
    }

    // print text, nový riadok
    
    printf("Morpheus: ");
    fprintf(fw, "Morpheus: ");

    for (int i = 0; i < (int)strlen(recvbuf); i++)
    {
        for (int j = 0; j < row; j++)
        {
            if (newline[j] == i)
            {
                printf("\n");
                setCursorRight();
                i++;
            }
        }
        printf("%c", recvbuf[i]);
        fprintf(fw, "%c", recvbuf[i]);
        Sleep(50);
    }

    setCursorLeft(); // reset cursor position
    fprintf(fw, "\n");
}