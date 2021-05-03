#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termio.h>
#include <time.h>
#include <ctype.h>
#include "btree.h"
#include "bt.h"
#include "bc.h"
#define SIZE_WORD 150
#define SIZE_MEAN 1500
#define N 100000

typedef struct
{
    char *s;
} String;

void readFile(BTA *root, BTA *btSoundex, FILE *f);
void insert(BTA *root, BTA *btSoundex);
void update(BTA *root);
void deleteWord(BTA *root, BTA *btSoundex);
int search_tab(BTA *root, char *word, String *output);
void search_soundex(BTA *root, BTA *btSoundex, String string[], int n);
void search_full(BTA *root, BTA *btSoundex);
int menu();
char getch(void);

int SoundEx(char *SoundEx, char *WordString, int LengthOption, int CensusOption);
void soundex_insert(char *word, BTA *btSoundex);
void soundex_deleteWord(char *word, BTA *btSoundex);
int soundex_search(char *word, BTA *btSoundex, String string[]);

int main()
{
    FILE *f;
    BTA *root, *btSoundex;
    char d, e;
    char word[SIZE_WORD];
    char mean[SIZE_MEAN];
    char soundex[10];
    //char filename[]="sample.dic";
    char filename[30];
    time_t start, end;
    String *string;
    int c, n, i, rsize, j, k;

    root = btopn("dictionary.dat", 0, 0);
    btSoundex = btopn("Soundex.dat", 0, 0);
    if (root == NULL || btSoundex == NULL)
    {

        printf("nhap file:");
        fgets(filename, 30, stdin);
        filename[strlen(filename) - 1] = '\0';
        f = fopen(filename, "r");
        if (f == NULL)
        {
            printf("Khong mo duoc file %s\n", filename);
            exit(1);
        }
        btinit();
        root = btcrt("dictionary.dat", 0, 0);
        btSoundex = btcrt("Soundex.dat", 0, 0);
        start = time(NULL);
        readFile(root, btSoundex, f);
        end = time(NULL);
    }

    do
    {
        c = menu();
        switch (c)
        {
        case 1:
            search_full(root, btSoundex);
            break;
        case 2:
            insert(root, btSoundex);
            break;
        case 3:
            deleteWord(root, btSoundex);
            break;
        case 4:
            update(root);

            break;

        case 5:
            printf("Ban co muon thoat khong(y/n):");
            scanf("%s", &d);
            if (d == 'y')
            {
                c = 10;
                printf("Cam on ban da su dung chuong trinh!\n");
                printf("Have a nice day!\n");
            }
            break;
            
        }

    } while (c != 10);

    btcls(root);
    btcls(btSoundex);
    return 0;
}

char getch(void)
{
    char ch;
    int fd = fileno(stdin);
    struct termio old_tty, new_tty;
    ioctl(fd, TCGETA, &old_tty);
    new_tty = old_tty;
    new_tty.c_lflag &= ~(ICANON | ECHO | ISIG);
    ioctl(fd, TCSETA, &new_tty);
    fread(&ch, 1, sizeof(ch), stdin);
    ioctl(fd, TCSETA, &old_tty);

    return ch;
}

int menu()
{
    int c, i;
    char d, k;
    char s[300];
    printf("\n******************TU DIEN ANH VIET******************\n");
    printf("                   BANG CHUC NANG                    \n");
    printf("1.Tim kiem.\n2.Them tu.\n3.Xoa tu.\n4.Cap nhat.\n5.Thoat.\n");
    printf("****************************************************\n");
    do
    {
        printf("Ban chon chuc nang:");
        scanf("%s", s);
        k = 0;
        if (strlen(s) == 1)
        {
            if (s[0] >= '1' && s[0] <= '9')
                c = atoi(s);
            else
                c = 0;
        }
        else
            c = 0;
        //printf("c=%d\n",c );
        while (getchar() != '\n')
            ;
    } while (c < 1 || c > 5);
    return c;
}
///*
void readFile(BTA *root, BTA *btSoundex, FILE *f)
{
    char line[SIZE_MEAN];
    char word[SIZE_WORD];
    char mean[SIZE_MEAN];
    int word_index = 0, mean_index = 0;
    int i = 0;
    while (1)
    {
        fgets(line, SIZE_MEAN, f);
    //bo qua comments trong file
        line[strlen(line) - 1] = '\0';
        if (line[0] == '/' && line[1] == '/')
            continue;
        if (line[0] == '[')
            continue;
        if (line[0] == ']')
            break;
        if (feof(f) == 1)
            break;
        char *ptr;
        //cat string khi gap ki tu {}:
        ptr = strtok(line, "{}:,");
        
        strcpy(word, ptr);
        ptr = strtok(NULL, "{}:,");

        strcpy(mean, ptr);
        i++;
        printf("%d\n", i);
        //them word mean vo btree root
        int result = btins(root, word, mean, SIZE_MEAN);
        if (result == 0)
            soundex_insert(word, btSoundex);
    }
    fclose(f);
}
void insert(BTA *root, BTA *btSoundex)
{
    char word[30];
    char mean[SIZE_MEAN];
    int i, rsize;
    printf("Nhap tu muon them:");
    fgets(word, SIZE_WORD, stdin);
    for (i = 0; i < strlen(word); i++)
        word[i] = tolower(word[i]);
    word[strlen(word) - 1] = '\0';
    if (btsel(root, word, mean, SIZE_MEAN, &rsize) == 0)
    {
        printf("Da co \'%s\' trong tu dien!\n", word);
    }
    else
    {
        printf("Nhap nghia cua tu:");
        fgets(mean, SIZE_MEAN, stdin);
        mean[strlen(mean) - 1] = '\0';
        btins(root, word, mean, SIZE_MEAN);
        soundex_insert(word, btSoundex);
        printf("Da them tu \'%s\' thanh cong!\n", word);
    }
}

void update(BTA *root)
{
    char word[30];
    char mean[SIZE_MEAN];
    int i, rsize;
    printf("Nhap tu muon cap nhat: ");
    fgets(word, SIZE_WORD, stdin);
    word[strlen(word) - 1] = '\0';
    for (i = 0; i < strlen(word); i++)
        word[i] = tolower(word[i]);
    if (btsel(root, word, mean, SIZE_MEAN, &rsize) == 0)
    {
        printf("Nhap nghia moi:");
        fgets(mean, SIZE_MEAN, stdin);
        mean[strlen(mean) - 1] = '\0';
        btupd(root, word, mean, SIZE_MEAN);
        printf("Da cap nhat \'%s\' thanh cong!\n", word);
    }
    else
    {

        printf("Khong co \'%s\' trong tu dien!\n", word);
    }
}

void deleteWord(BTA *root, BTA *btSoundex)
{
    char word[30];
    char mean[SIZE_MEAN];
    int i, rsize;
    printf("Nhap tu muon xoa:");
    fgets(word, SIZE_WORD, stdin);
    word[strlen(word) - 1] = '\0';
    for (i = 0; i < strlen(word); i++)
        word[i] = tolower(word[i]);
    i = btdel(root, word);
    if (i == 0)
    {
        printf("Da xoa \'%s\' thanh cong!\n", word);
        soundex_deleteWord(word, btSoundex);
    }
    else
        printf("Khong co \'%s\' trong tu dien!\n", word);
}
 
void search_full(BTA *root, BTA *btSoundex)
{
    char *word, mean[SIZE_MEAN];
    int i, j, k, n, rsize;
    char e;
    String *string;
    word = (char *)malloc(sizeof(char) * SIZE_WORD);
    string = (String *)malloc(sizeof(String) * 1000);
    printf("Nhap tu can tim:");
    j = 0;
    i = 0;
    while (1)
    {
        e = tolower(getch());
        //neu khong phai xuong dong tab delete esc thi them vao word
        if (e != '\n' && e != 127 && e != '\t' && e != 27)
        {
            word[j++] = e;
            putchar(e);
            i = 0;
        }
        //neu an tab
        if (e == '\t')
        {//neu da co nhieu hon 1 ki tu
            if (j != 0)
            {
                if (i == 0)
                {
                    word[j] = '\0';
                    free(string);
                    string = (String *)malloc(sizeof(String) * 1000);
                    n = search_tab(root, word, string);
                    if (n != 0)
                    {
                        for (k = 0; k < j; k++)
                        {
                            putchar('\b');
                            printf("%c[0K", 27);
                        };
                        free(word);
                        word = (char *)malloc(sizeof(char) * SIZE_WORD);
                        strcpy(word, string[i].s);
                        for (k = 0; k < strlen(word); k++)
                        {
                            putchar(word[k]);
                        }
                        j = strlen(word);
                        i++;
                    }
                }
                else
                {
                    for (k = 0; k < strlen(word); k++)
                    {
                        putchar('\b');
                        printf("%c[0K", 27);
                    }
                    if (i == n)
                        i = 0; //tim den tu cuoi cung
                    free(word);
                    word = (char *)malloc(sizeof(char) * SIZE_WORD);
                    strcpy(word, string[i].s);
                    for (k = 0; k < strlen(word); k++)
                    {
                        putchar(word[k]);
                    }
                    j = strlen(word);
                    i++;
                }
            }
        }
        if (e == 127)
        {
            if (j > 0)
            {
                putchar('\b');
                printf("%c[0K", 27);
                word[j--] = '\0';
                i = 0;
            }
        }
        if (e == '\n')
        {
            printf("\n");
            word[j] = '\0';
            if (btsel(root, word, mean, SIZE_MEAN, &rsize) == 0)
            {

                printf("Nghia cua tu \'%s\' la: %s\n", word, mean);
            }
            else
            {
                printf("Khong co tu \'%s\' trong tu dien.\n", word);
                free(string);
                string = (String *)malloc(sizeof(String) * 1000);
                i = soundex_search(word, btSoundex, string);
                if (i != 0) 
                    search_soundex(root, btSoundex, string, i);
            }
            free(string);
            free(word);
            break;
        }
    }
}

void search_soundex(BTA *root, BTA *btSoundex, String string[], int n)
{
    char *word, mean[SIZE_MEAN];
    int i, j, k, rsize;
    char e;
    word = (char *)malloc(sizeof(char) * SIZE_WORD);
    printf("Nhap tu can tim:");
    j = 0;
    i = 0;
    while (1)
    {
        e = tolower(getch());
        if (e != '\n' && e != 127 && e != '\t' && e != 27)
        {
            word[j++] = e;
            putchar(e);
            i = 0;
        }
        if (e == '\t')
        {
            if (j != 0)
            {
                word[j] = '\0';
                if (i == 0)
                {
                    while (1)
                    {
                        if (strncmp(string[i].s, word, strlen(word)) == 0)
                        {
                            for (k = 0; k < strlen(word); k++)
                            {
                                putchar('\b');
                                printf("%c[0K", 27);
                            }
                            //tim den tu cuoi cung
                            free(word);
                            word = (char *)malloc(sizeof(char) * SIZE_WORD);
                            strcpy(word, string[i].s);
                            //j=0;
                            for (k = 0; k < strlen(word); k++)
                            {
                        
                                putchar(word[k]);
                
                            }
                            j = strlen(word);
                            i++;
                            break;
                        }
                        i++;
                        if (i == n)
                            break;
                    }
                }
                else
                {
                    for (k = 0; k < strlen(word); k++)
                    {
                        putchar('\b');
                        printf("%c[0K", 27);
                    }
                    if (i == n)
                        i = 0; //tim den tu cuoi cung
                    free(word);
                    word = (char *)malloc(sizeof(char) * SIZE_WORD);
                    strcpy(word, string[i].s);
                    //j=0;
                    for (k = 0; k < strlen(word); k++)
                    {
                        //e=word[k];
                        putchar(word[k]);
                        //j++;
                    }
                    j = strlen(word);
                    i++;
                }
            }
        }
        if (e == 127)
        {
            if (j > 0)
            {
                putchar('\b');
                printf("%c[0K", 27);
                word[j--] = '\0';
                i = 0;
            }
        }
        if (e == '\n')
        {
            printf("\n");
            word[j] = '\0';
            if (btsel(root, word, mean, SIZE_MEAN, &rsize) == 0)
            {

                printf("Nghia cua tu \'%s\' la: %s\n", word, mean);
            }
            else
            {
                printf("Khong co tu \'%s\' trong tu dien.\n", word);
                i = soundex_search(word, btSoundex, string);
                if (i != 0) 
                    search_soundex(root, btSoundex, string, i);
            }
            free(word);
            break;
        }
    }
}
int search_tab(BTA *root, char *word, String *output)
{
    
    int i, total = 0, value;
    char key[50];
    i = bfndky(root, word, &value); 
    if (i == 0)
    { 
        output[total++].s = strdup(word);
    }
    while (1)
    {
        i = bnxtky(root, key, &value); 
        if (i != 0 || strncmp(key, word, strlen(word)) != 0)
            break;
        output[total++].s = strdup(key); 
        if (total == 1000)
            break;
    }
    return total;
}

int SoundEx(char *SoundEx, char *WordString, int LengthOption, int CensusOption)
{
    int InSz = 31;
    char WordStr[32];
    int SoundExLen, WSLen, i;
    char FirstLetter, *p, *p2;

    SoundExLen = WSLen = 0;
    SoundEx[0] = 0;

    if (CensusOption)
    {
        LengthOption = 4;
    }

    if (LengthOption)
    {
        SoundExLen = LengthOption;
    }
    if (SoundExLen > 10)
    {
        SoundExLen = 10;
    }
    if (SoundExLen < 4)
    {
        SoundExLen = 4;
    }

    if (!WordString)
    {
        return (0);
    }
    for (p = WordString, p2 = WordStr, i = 0; (*p); p++, p2++, i++)
    {
        if (i >= InSz)
            break;
        (*p2) = (*p);
    }
    (*p2) = 0;
    for (p = WordStr; (*p); p++)
    {
        if ((*p) >= 'a' && (*p) <= 'z')
        {
            (*p) -= 0x20;
        }
    }
    for (p = WordStr; (*p); p++)
    {
        if ((*p) < 'A' || (*p) > 'Z')
        {
            (*p) = ' ';
        }
    }
    for (i = 0, p = p2 = WordStr; (*p); p++)
    {
        if (!i)
        {
            if ((*p) != ' ')
            {
                (*p2) = (*p);
                p2++;
                i++;
            }
        }
        else
        {
            (*p2) = (*p);
            p2++;
        }
    }
    (*p2) = 0;
    for (i = 0, p = WordStr; (*p); p++)
        i++;
    for (; i; i--)
    {
        if (WordStr[i] == ' ')
        {
            WordStr[i] = 0;
        }
        else
        {
            break;
        }
    }
    for (WSLen = 0, p = WordStr; (*p); p++)
        WSLen++;

    if (!WSLen)
    {
        return (0);
    }
    if (!CensusOption)
    {
        if (WordStr[0] == 'P' && WordStr[1] == 'S')
        {
            WordStr[0] = '_';
        }

        if (WordStr[0] == 'P' && WordStr[1] == 'F')
        {
            WordStr[0] = '_';
        }
        if (WordStr[0] == 'G' && WordStr[1] == 'H')
        {
            WordStr[1] = '_';
        }

        for (i = 0; i < WSLen; i++)
        {
            if (WordStr[i] == 'D' && WordStr[i + 1] == 'G')
            {
                WordStr[i] = '_';
                i++;
                continue;
            }
            if (WordStr[i] == 'G' && WordStr[i + 1] == 'H')
            {
                WordStr[i] = '_';
                i++;
                continue;
            }
            if (WordStr[i] == 'K' && WordStr[i + 1] == 'N')
            {
                WordStr[i] = '_';
                i++;
                continue;
            }
            if (WordStr[i] == 'G' && WordStr[i + 1] == 'N')
            {
                WordStr[i] = '_';
                i++;
                continue;
            }
            if (WordStr[i] == 'M' && WordStr[i + 1] == 'B')
            {
                WordStr[i + 1] = '_';
                i++;
                continue;
            }

            if (WordStr[i] == 'P' && WordStr[i + 1] == 'H')
            {
                WordStr[i] = 'F';
                WordStr[i + 1] = '_';
                i++;
                continue;
            }
            if (WordStr[i] == 'T' &&
                WordStr[i + 1] == 'C' &&
                WordStr[i + 2] == 'H')
            {

                WordStr[i] = '_';
                i++;
                i++;
                continue;
            }
            if (WordStr[i] == 'M' && WordStr[i + 1] == 'P' && (WordStr[i + 2] == 'S' || WordStr[i + 2] == 'T' || WordStr[i + 2] == 'Z'))
            {
                WordStr[i + 1] = '_';
                i++;
            }
        }
    }
    for (p = p2 = WordStr; (*p); p++)
    {
        (*p2) = (*p);
        if ((*p2) != '_')
        {
            p2++;
        }
    }
    (*p2) = 0;
    FirstLetter = WordStr[0];
    if (FirstLetter == 'H' || FirstLetter == 'W')
    {
        WordStr[0] = '-';
    }
    if (CensusOption == 1)
    {
        for (p = &(WordStr[1]); (*p); p++)
        {
            if ((*p) == 'H' || (*p) == 'W')
            {
                (*p) = '.';
            }
        }
    }

    for (p = WordStr; (*p); p++)
    {
        if ((*p) == 'A' ||
            (*p) == 'E' ||
            (*p) == 'I' ||
            (*p) == 'O' ||
            (*p) == 'U' ||
            (*p) == 'Y' ||
            (*p) == 'H' ||
            (*p) == 'W')
        {
            (*p) = '0';
        }
        if ((*p) == 'B' ||
            (*p) == 'P' ||
            (*p) == 'F' ||
            (*p) == 'V')
        {
            (*p) = '1';
        }
        if ((*p) == 'C' ||
            (*p) == 'S' ||
            (*p) == 'G' ||
            (*p) == 'J' ||
            (*p) == 'K' ||
            (*p) == 'Q' ||
            (*p) == 'X' ||
            (*p) == 'Z')
        {
            (*p) = '2';
        }
        if ((*p) == 'D' ||
            (*p) == 'T')
        {
            (*p) = '3';
        }
        if ((*p) == 'L')
        {
            (*p) = '4';
        }

        if ((*p) == 'M' ||
            (*p) == 'N')
        {
            (*p) = '5';
        }
        if ((*p) == 'R')
        {
            (*p) = '6';
        }
    }

    if (CensusOption == 1)
    {
        for (p = p2 = &WordStr[1]; (*p); p++)
        {
            (*p2) = (*p);
            if ((*p2) != '.')
            {
                p2++;
            }
        }
        (*p2) = 0;
    }
    for (p = p2 = &(WordStr[0]); (*p); p++)
    {
        (*p2) = (*p);
        if ((*p2) != p[1])
        {
            p2++;
        }
    }
    (*p2) = 0;
    for (p = p2 = &WordStr[1]; (*p); p++)
    {
        (*p2) = (*p);
        if ((*p2) != ' ' && (*p2) != '0')
        {
            p2++;
        }
    }
    (*p2) = 0;
    for (WSLen = 0, p = WordStr; (*p); p++)
        WSLen++;
    for (i = WSLen; i < SoundExLen; i++)
    {
        WordStr[i] = '0';
    }
    WordStr[SoundExLen] = 0;
    WordStr[0] = FirstLetter;
    for (p2 = SoundEx, p = WordStr; (*p); p++, p2++)
    {
        (*p2) = (*p);
    }
    (*p2) = 0;
    return (SoundExLen);
}

void soundex_insert(char *word, BTA *btSoundex)
{ 
    char s[N], soundEx[10];
    int i, k, h, l, rsize;
    l = SoundEx(soundEx, word, 4, 4);
    soundEx[l] = '\0';
    i = btsel(btSoundex, soundEx, s, N, &rsize);
    if (i != 0)
    {
        if (btins(btSoundex, soundEx, word, SIZE_WORD) != 0)
            printf("Nhap soundex that bai!");
    }
    else
    {
        strcat(s, "\n");
        strcat(s, word);
        s[strlen(s)] = '\0';
        btupd(btSoundex, soundEx, s, N);
    }
}

void soundex_deleteWord(char *word, BTA *btSoundex)
{ 
    char s[N], soundEx[10], *ptr;
    int l, i, rsize, j;
   
    l = SoundEx(soundEx, word, 4, 4);
    soundEx[l] = '\0';
   
    i = btsel(btSoundex, soundEx, s, N, &rsize);
    if (i != 0)
    {
        //printf("Da xoa tu va khong tim thay soundex!\n");
    }
    else
    {

        if (strcmp(s, word) == 0)
        {
            btdel(btSoundex, soundEx); 
        }

        else
        {
            ptr = strtok(s, "\n");
            btdel(btSoundex, soundEx);
            while (ptr)
            {
                if (strcmp(ptr, word) != 0)
                {
                    soundex_insert(ptr, btSoundex);
                }
                ptr = strtok(NULL, "\n");
            }
            free(ptr);
        }
    }

    //free(s);
}
int soundex_search(char *word, BTA *btSoundex, String string[])
{
    int l, bn, rsize, i;
    char en[N], soundEx[10], *ptr;
    l = SoundEx(soundEx, word, 4, 4);
    soundEx[l] = '\0';
    bn = btsel(btSoundex, soundEx, en, N, &rsize);
    if (bn == 0)
    {
        printf("Y cua ban co phai la:\n");
        printf("%s\n", en);
        if (strcmp(en, word) == 0)
        {
            string[0].s = strdup(word);
            return 1;
        }
        else
        {
            ptr = strtok(en, "\n");
            i = 0;
            while (ptr)
            {
                string[i++].s = strdup(ptr);
                ptr = strtok(NULL, "\n");
            }
            free(ptr);
            return i;
        }
    }
    return 0;
}

