#include <iostream>
#include <fstream>
#include <string>
#include<windows.h>

using namespace std;

struct Country //Наша структура
{
    string Name; //Имя государства
    string NationalLanguage; //Язык государства
    string Currency; //Валюта государства
    double DollareExchangeRate; //Курс валюты по отношению к доллару

    void FillIn(string name, string nl, string cur, double der) //Метод для инициализация структуру 
    {
        Name = name;
        NationalLanguage = nl;
        Currency = cur;
        DollareExchangeRate = der;
    }

    void PrintOut() //Метод для печати структуры
    {
        cout << "Имя: " << Name << endl;
        cout << "Язык: " << NationalLanguage << endl;
        cout << "Валюта: " << Currency << endl;
        cout << "Курс валюты по отношению к доллару: " << DollareExchangeRate << endl;
    }
};

void SerString(ofstream* f, string line) //Метод для сириализация строки
{
   size_t Length = line.length(); //Получаем размер строк
    f->write((char*) (&Length), sizeof(Length)); //Записываем в файл размер строки
    f->write((char*) (line.data()), Length); //Записываем саму строку
}

string DesString(ifstream *f)  //Метод для дисириализации строки
{
    //Получаем размер дисирнализируемой строки
    size_t Length = 0;
    f->read((char*) (&Length), sizeof(Length));

    //Объявляем строку с нужным размером
    string Line;
    Line.resize(Length);
    f->read((char*) (Line.data()), Length); //Получаем строку

    return Line; //И возвращаем её
}

void Serialize(ofstream* f, Country country) //Метод для сириализация структуры
{
    //Сириализируем строковые элементы структуры
    SerString(f, country.Name);
    SerString(f, country.NationalLanguage);
    SerString(f, country.Currency);

    //Сириализируем double элемент структуры
    double der = country.DollareExchangeRate;
    f->write((char*) &der, sizeof(der));
 }

Country DeSerialize(ifstream *f) //Метод для дисириализация структуры
{
    Country country;

    //Дисириализируем строковые элементы структуры
    country.Name = DesString(f);
    country.NationalLanguage = DesString(f);
    country.Currency = DesString(f);

    //Дисириализируем double элемент структуры
    double dex = 0.0;
    f->read((char*) &dex, sizeof(dex));
    country.DollareExchangeRate = dex;

    return country; //Возаращаем структуру
}

void WriteFile(ofstream *f, int n) //Метод для генераации и записи структуры в файл
{
    for (int i = 0; i < n; i++)
    {
        Country country;

        string name;
        string nl;
        string cur;
        double der;

        //Получаем данные от пользователя
        cout << "Введите название страны:" << endl; getline(cin, name);
        cout << "Введите национальный язык страны:" << endl; getline(cin, nl);
        cout << "Введите валюту страны:" << endl; getline(cin, cur);
        cout << "Введите курс валюты страны по отношению к доллару:" << endl; cin >> der;
        cin.ignore(); //Избаввляемся от лишнего пробела, который появился после ввода курсы валюты

        country.FillIn(name, nl, cur, der); //Инициализируем структуру
        Serialize(f, country); //Сириализируем структуру в файл
        cout << endl; 
    }
}

int main()
{
    //Подключаем русский язык
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    int Amount;
    cout << "Введите кол-во стран" << endl;
    cin >> Amount; //Вводим кол-во стран

    if (Amount < 1) //Проверка на валидность
    {
        cout << "Кол-во стран должно быть >= 1" << endl;
        return 0;
    }

    ofstream OutputStream("F1.bin", ios::binary); //Открываем фаил в out поток

    if (!OutputStream.is_open())
    {
        cout << "Не удалось открыть фаил!" << endl;
        return 0;
    }

    cin.ignore(); //Избаввляемся от лишнего пробела, который появился после ввода кол-ва стран

    cout << endl;
    WriteFile(&OutputStream, Amount); //Генерируем структуры и записываем их в бинарный фаил

    OutputStream.close(); //Закрываем входной поток

    ifstream InputStream("F1.bin", ios::binary); //Открываем фаил в входном потоке

    if (!InputStream.is_open())
    {
        cout << "Ошибка, фаил был удален во время выполнения программы!" << endl;
        return 0;
    }

    Country* Buffer = new Country[Amount]; //Буффер, для хранения структур, находящихся в файле

    cout << "Ваши страны:" << endl << endl;

    for (int i = 0; i < Amount; i++) //Считываем структуры из файла, записываем их в буффер
    {
        Country country;
        country = DeSerialize(&InputStream);

        country.PrintOut();
        Buffer[i] = country;
        cout << endl;
    }

    InputStream.close(); //Закрываем входной поток

    OutputStream.open("F1.bin", ios_base::binary); //Открываем бинарный фаил в входном потоке

    if (!OutputStream.is_open()) //Проверка на открытие
    {
        cout << "Ошибка, фаил был удален во время выполнения программы!" << endl;
        delete[] Buffer;
        return 0;
    }

    string BadName; 

    cout << "Введите имя страны, которую надо убрать из списка: " << endl;
    getline(cin, BadName); //Получаем имя страны, которое надо удалить
    cout << endl;

    int NewLenght = Amount;

    for (int i = 0; i < Amount; i++) //Перебираем все элементы буффера
    {
        if (Buffer[i].Name != BadName)  //Если имя страны не совподает с запретным
            Serialize(&OutputStream, Buffer[i]); //То записываем в файл
        else
            NewLenght--; //Иначе уменьшаем переменную нового размера
    }

    delete[] Buffer;

    cout << "Сколько еще стран вы хотите вести" << endl;

    int newElementsCount;
    cin >> newElementsCount; //Получаем кол-во стран, которое надо ввести дополнительно

    if (newElementsCount >= 1) //Если данное количество больше или равно 1
    {
        cin.ignore();
        cout << endl;
        WriteFile(&OutputStream, newElementsCount); //то дописываем в фаил сколько надо стран
    }
        

    OutputStream.close(); //Закрываем входной поток

    InputStream.open("F1.bin", ios::binary); //Отркрываем бинарный фаил в выходном потоке

    if (!InputStream.is_open())
    {
        cout << "Ошибка, фаил был удален во время выполнения программы!" << endl;
        return 0;
    }

    NewLenght += newElementsCount > -1 ? newElementsCount : 0; //Рассчитываем новое кол-во элементов

    cout << endl;

    if (NewLenght > 0) //Если новое кол-во элементов больше 0
    {
        cout << "Ваши страны:" << endl << endl;

        for (int i = 0; i < NewLenght; i++) //То выводим страны из файла
        {
            DeSerialize(&InputStream).PrintOut();
            cout << endl;
        } 
    }
    else
    {
        cout << "Стран не осталось" << endl;  //иначе сообщаем, что стран не осталось
    }

    InputStream.close(); //Закрываем выходной поток
}
